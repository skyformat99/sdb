#include "db_meta.h"
#include "util/log.h"
#include "util/strings.h"
#include "db.h"
#include "aof_reader.h"
#include "aof_writer.h"

#define MAX_META_FILE_SIZE 1024

DbMeta::DbMeta(){
	_writer = NULL;
}

DbMeta::~DbMeta(){
	delete _writer;
}

void DbMeta::var_dump(){
	log_trace("=== all files ===");
	for(std::map<int, std::string>::iterator it=_files.begin(); it!=_files.end(); it++){
		int seq = it->first;
		const std::string &ext = it->second;
		log_trace("  %s", _db->_store->make_filename(seq, ext).c_str());
	}
	log_trace("=== total %d ===", _files.size());
}

DbMeta* DbMeta::create(Db *db){
	DbMeta *ret = new DbMeta();
	ret->_db = db;

	std::vector<int> files;
	files = ret->_db->_store->find_files_by_ext("meta");
	if(files.empty()){
		log_trace("empty db");
		ret->_writer = ret->_db->_store->create_file("meta");
	}else{
		if(files.size() > 1){
			ret->merge_files(files);
			files = ret->_db->_store->find_files_by_ext("meta");
		}
		int root_seq = files[0];
		std::string name = ret->_db->_store->make_filename(root_seq, "meta");

		log_trace("load meta file: %s", name.c_str());
		ret->load_file(name);
		ret->_writer = AofWriter::open(name);
	}
	
	return ret;
}

int DbMeta::load_file(const std::string &filename){
	AofReader *reader = AofReader::open(filename);
	Record rec;
	while(reader->next(&rec)){
		const std::string &key = rec.key;
		const std::string &val = rec.val;
		int seq = str_to_int(key);
		if(rec.is_set()){
			this->_files[seq] = val;
		}else{
			this->_files.erase(seq);
		}
	}
	delete reader;
	return 0;
}

int DbMeta::merge_files(const std::vector<int> &files){
	log_trace("merge meta files:");
	int dst_seq = _db->_store->merge_files(files, "meta", true);
	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		_db->_store->remove_file(seq);
	}
	return dst_seq;
}

///////////////////////////////////////////////////////

std::vector<int> DbMeta::find_files_by_ext(const std::string &ext) const{
	std::vector<int> ret;
	for(std::map<int, std::string>::const_iterator it=_files.begin(); it!=_files.end(); it++){
		if(it->second == ext){
			ret.push_back(it->first);
		}
	}
	return ret;
}

int DbMeta::add_file(int seq, const std::string &ext){
	if(_writer->size() > MAX_META_FILE_SIZE){
		delete _writer;
		_writer = _db->_store->create_file("meta");
	}
	int ret = _writer->set(str(seq), ext);
	if(ret == -1){
		return -1;
	}
	_files[seq] = ext;
	log_trace("add file: %d", seq);
	return 0;
}

int DbMeta::del_file(int seq){
	if(_files.find(seq) == _files.end()){
		return 0;
	}
	
	if(_writer->size() > MAX_META_FILE_SIZE){
		delete _writer;
		_writer = _db->_store->create_file("meta");
	}
	int ret = _writer->del(str(seq));
	if(ret == -1){
		return -1;
	}
	
	ret = _db->_store->remove_file(seq);
	if(ret == -1){
		return -1;
	}
	
	_files.erase(seq);
	log_trace("del file: %d", seq);
	return 0;
}

