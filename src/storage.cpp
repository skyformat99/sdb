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

AofWriter* Storage::create_file(const std::string &ext){
	int seq = this->next_file_seq();
	std::string name = this->make_filename(seq, ext);
	_files[seq] = ext;
	
	AofWriter *ret = AofWriter::open(name);
	log_trace("new file: %s", name.c_str());
	
	// TODO
	//_root->set(str(seq), "aof");
	
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
