#include "log_db.h"
#include "util/file.h"
#include "util/strings.h"
#include "util/log.h"

#define MIN_FILENAME_SEQ  1
#define MAX_FILENAME_SEQ  100
#define MAX_LOG_FILE_SIZE 1024
#define MAX_LOG_FILES     3

LogDb::LogDb(){
	_log = NULL;
	_mm = NULL;
	_imm = NULL;
}

LogDb::~LogDb(){
	delete _log;	
	delete _mm;
	delete _imm;
}

void LogDb::var_dump(){
	std::string msg;
	msg.append("[");
	int i = 0;
	for(std::set<int>::iterator it=_files.begin(); it!=_files.end(); it++){
		int seq = *it;
		msg.append(str(seq));
		if(i != _files.size()-1){
			msg.append(",");
		}
		i++;
	}
	msg.append("]");
	
	log_trace("db[%s] files: %s", _ext.c_str(), msg.c_str());
	_mm->var_dump();
}

LogDb* LogDb::open(const std::string &path, const std::string &ext){
	std::string dir = path;
	if(path[path.size() - 1] != '/'){
		dir = path + "/";
	}
	
	LogDb *ret = new LogDb();
	ret->_path = dir;
	ret->_ext = ext;
	
	ret->init_filesystem();
	
	if(ret->_files.empty()){
		log_trace("empty db");
		ret->_log = ret->create_file();
	}else{
		ret->merge_logs();
		int seq = *(ret->_files.begin());
		std::string name = ret->make_filename(seq);
		ret->_log = AofWriter::open(name);
	}

	ret->_mm = new MemTable();
	ret->_mm->load(ret->_log->filename());
	
	return ret;
}

int LogDb::merge_logs(){
	std::set<int> olds = _files;
	
	std::vector<std::string> names;
	for(std::set<int>::iterator it=_files.begin(); it!=_files.end(); it++){
		int seq = *it;
		std::string name = this->make_filename(seq);
		names.push_back(name);
	}
	
	int dst_seq;
	AofWriter *writer = this->create_file(&dst_seq);
	AofWriter::merge_files(names, writer->filename());
	delete writer;
	
	for(std::set<int>::iterator it=olds.begin(); it!=olds.end(); it++){
		int seq = *it;
		this->remove_file(seq);
	}
	
	// logging stuff
	std::string msg;
	msg.append("merge files [");
	int i = 0;
	for(std::set<int>::iterator it=olds.begin(); it!=olds.end(); it++){
		int seq = *it;
		msg.append(str(seq));
		if(i != olds.size()-1){
			msg.append(",");
		}
		i ++;
	}
	msg.append("] => [");
	msg.append(str(dst_seq));
	msg.append("]");
	log_trace("%s", msg.c_str());
	
	return 0;
}

////////////// interface //////////////

bool LogDb::contains(const std::string &key){
	return _mm->contains(key);
}

bool LogDb::set(const std::string &key, const std::string &val){
	_log->set(key, val);
	_mm->set(key, val);
	return true;
}

bool LogDb::del(const std::string &key){
	_log->del(key);
	_mm->del(key);
	return true;
}

int LogDb::try_rotate_log(){
	if(_log->size() <= MAX_LOG_FILE_SIZE){
		return 0;
	}
	
	if(_files.size() >= MAX_LOG_FILES || _mm->size() > MAX_LOG_FILE_SIZE){
		std::set<int> olds = _files;

		if(_imm){
			delete _imm;
		}
		_imm = _mm;
		_mm = new MemTable();
		
		int seq = 0;
		AofWriter *merge = this->create_file(&seq);
		_imm->save(merge);
		log_trace("dump memtable to: %d", seq);
		delete merge;
		
		for(std::set<int>::iterator it=olds.begin(); it!=olds.end(); it++){
			int seq = *it;
			this->remove_file(seq);
		}
	}
	
	int seq = 0;
	delete _log;
	_log = this->create_file(&seq);
	return seq;
}

bool LogDb::get(const std::string &key, Record *rec){
	if(_mm->get(key, rec)){
		return true;
	}
	if(_imm && _imm->get(key, rec)){
		return true;
	}
	return false;
}

////////////// filesystem //////////////

int LogDb::init_filesystem(){
	if(!file_exists(_path)){
		mkdir(_path.c_str(), 0744);
	}
	
	std::vector<std::string> files = scandir(_path);
	std::vector<std::string>::iterator it;
	for(it=files.begin(); it!=files.end(); it++){
		const std::string &name = *it;
		int num = str_to_int(name);
		std::string::size_type pos;
		pos = name.find_last_of(".");
		if(pos == std::string::npos){
			continue;
		}
		std::string ext = name.substr(pos + 1);
		if(ext == _ext){
			_files.insert(num);
		}
	}
	return 0;
}

std::string LogDb::make_filename(int seq){
	char buf[256];
	snprintf(buf, sizeof(buf), "%s%08d.%s", _path.c_str(), seq, _ext.c_str());
	return buf;
}

int LogDb::next_file_seq(){
	if(_files.empty()){
		return MIN_FILENAME_SEQ;
	}
	int ret = *(_files.rbegin()) + 1;
	if(ret <= MAX_FILENAME_SEQ){
		return ret;
	}
	ret = MIN_FILENAME_SEQ;
	for(std::set<int>::iterator it=_files.begin(); it!=_files.end(); it++){
		if(*it != ret){
			break;
		}else{
			ret = *it + 1;
		}
	}
	return ret;
}

AofWriter* LogDb::create_file(int *ret_seq){
	int seq = this->next_file_seq();
	std::string name = this->make_filename(seq);
	
	AofWriter *ret = AofWriter::open(name);
	log_trace("new file: %s", name.c_str());
	
	if(ret_seq){
		*ret_seq = seq;
	}
	_files.insert(seq);
	return ret;
}

int LogDb::remove_file(int seq){
	std::set<int>::iterator it;
	it = _files.find(seq);
	if(it == _files.end()){
		return 0;
	}
	std::string name = this->make_filename(seq);
	if(unlink(name.c_str()) == -1){
		return -1;
	}
	_files.erase(it);
	log_trace("remove file: %s", name.c_str());
	return 1;
}
