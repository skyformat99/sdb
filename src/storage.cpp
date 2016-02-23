#include "storage.h"
#include "util/file.h"
#include "util/strings.h"
#include "util/log.h"

#define MIN_FILENAME_SEQ  1
#define MAX_FILENAME_SEQ  100

Storage::Storage(){
}

Storage::~Storage(){
}

Storage* Storage::open(const std::string &path){
	if(!file_exists(path)){
		mkdir(path.c_str(), 0744);
	}
	Storage *ret = new Storage();
	ret->_path = path;
	ret->load_file_seqs();
	return ret;
}

std::string Storage::make_filename(int seq, const std::string &ext){
	char buf[256];
	snprintf(buf, sizeof(buf), "%s%08d.%s", _path.c_str(), seq, ext.c_str());
	return buf;
}

void Storage::load_file_seqs(){
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
		if(ext == "root" || ext == "aof" || ext == "sst"){
			_files[num] = ext;
		}
	}
}

std::vector<int> Storage::find_files_by_ext(const std::string &ext) const{
	std::vector<int> ret;
	for(std::map<int, std::string>::const_iterator it=_files.begin(); it!=_files.end(); it++){
		if(it->second == ext){
			ret.push_back(it->first);
		}
	}
	return ret;
}

int Storage::next_file_seq(){
	int ret = MIN_FILENAME_SEQ;
	std::map<int, std::string>::reverse_iterator rit;
	rit = _files.rbegin();
	if(rit != _files.rend()){
		ret = rit->first + 1;
		if(ret > MAX_FILENAME_SEQ){
			ret = MIN_FILENAME_SEQ;
			std::map<int, std::string>::iterator it;
			for(it=_files.begin(); it!=_files.end(); it++){
				if(it->first != ret){
					break;
				}else{
					ret = it->first + 1;
				}
			}
		}
	}
	return ret;
}

AofWriter* Storage::create_file(const std::string &ext, int *ret_seq){
	int seq = this->next_file_seq();
	std::string name = this->make_filename(seq, ext);
	
	AofWriter *ret = AofWriter::open(name);
	log_trace("create file: %s", name.c_str());
	
	if(ret_seq){
		*ret_seq = seq;
	}
	_files[seq] = ext;
	return ret;
}

int Storage::remove_file(int seq){
	std::map<int, std::string>::iterator it;
	it = _files.find(seq);
	if(it == _files.end()){
		return 0;
	}
	std::string ext = it->second;
	std::string name = this->make_filename(seq, ext);
	unlink(name.c_str());
	_files.erase(it);
	return 1;
}

// return new file seq
int Storage::merge_files(const std::vector<int> &src, const std::string &ext){
	std::map<std::string, std::string> sets;
	std::set<std::string> dels;
	
	for(int i=0; i<src.size(); i++){
		int seq = src[i];
		std::string filename = this->make_filename(seq, ext);
		AofReader *reader = AofReader::open(filename);
		for(std::map<std::string, std::string>::iterator m_it=reader->sets.begin(); m_it!=reader->sets.end(); m_it++){
			const std::string &key = m_it->first;
			const std::string &val = m_it->second;
			sets[key] = val;
			dels.erase(key);
		}
		for(std::set<std::string>::iterator s_it=reader->dels.begin(); s_it!=reader->dels.end(); s_it++){
			const std::string &key = *s_it;
			sets.erase(key);
			dels.insert(key);
		}
		delete reader;
	}
	
	int dst_seq;
	AofWriter *writer = this->create_file(ext, &dst_seq);
	for(std::map<std::string, std::string>::iterator m_it=sets.begin(); m_it!=sets.end(); m_it++){
		const std::string &key = m_it->first;
		const std::string &val = m_it->second;
		writer->set(key, val);
	}
	for(std::set<std::string>::iterator s_it=dels.begin(); s_it!=dels.end(); s_it++){
		const std::string &key = *s_it;
		writer->del(key);
	}
	delete writer;
	
	return dst_seq;
}
