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

DbMeta* DbMeta::create(Db *db){
	DbMeta *ret = new DbMeta();
	ret->_db = db;

	std::vector<int> files;
	files = ret->_db->_store->find_files_by_ext("meta");
	if(files.empty()){
		ret->_writer = ret->_db->_store->create_file("meta");
	}else{
		if(files.size() > 1){
			ret->merge_files(files);
			files = ret->_db->_store->find_files_by_ext("meta");
		}
		int root_seq = files[0];
		std::string name = ret->_db->_store->make_filename(root_seq, "meta");

		ret->load_file(name);
		ret->_writer = AofWriter::open(name);
	}
	
	return ret;
}

int DbMeta::load_file(const std::string &filename){
	AofReader *reader = AofReader::open(filename);
	for(std::map<std::string, std::string>::iterator m_it=reader->sets.begin(); m_it!=reader->sets.end(); m_it++){
		const std::string &key = m_it->first;
		const std::string &val = m_it->second;
		int seq = str_to_int(key);
		this->_files[seq] = val;
	}
	for(std::set<std::string>::iterator s_it=reader->dels.begin(); s_it!=reader->dels.end(); s_it++){
		const std::string &key = *s_it;
		int seq = str_to_int(key);
		this->_files.erase(seq);
	}
	delete reader;
	return 0;
}

int DbMeta::merge_files(const std::vector<int> &files){
	int dst_seq = _db->_store->merge_files(files, "meta");

	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		_db->_store->remove_file(seq);
	}
	
	// logging stuff
	std::string msg;
	msg.append("merge meta files [");
	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		msg.append(str(seq));
		if(i != files.size()-1){
			msg.append(",");
		}
	}
	msg.append("] => [");
	msg.append(str(dst_seq));
	msg.append("]");
	log_trace("%s", msg.c_str());
	
	return 0;
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

