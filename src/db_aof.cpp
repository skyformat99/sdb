#include "db_aof.h"
#include "db.h"
#include "util/log.h"
#include "util/strings.h"
#include "aof_reader.h"
#include "aof_writer.h"

#define MAX_AOF_FILE_SIZE       (1 * 1024)

DbAof::DbAof(){
	_writer = NULL;
}

DbAof::~DbAof(){
	delete _writer;
}

void DbAof::var_dump(){
	std::vector<int> files;
	files = _db->_meta->find_files_by_ext("aof");
	std::string msg;
	msg.append("aof files [");
	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		msg.append(str(seq));
		if(i != files.size()-1){
			msg.append(",");
		}
	}
	msg.append("]");
	log_trace("%s", msg.c_str());
	_mm->var_dump();
}

DbAof* DbAof::create(Db *db){
	DbAof *ret = new DbAof();
	ret->_db = db;
	ret->_mm = new MemTable();

	std::vector<int> files;
	files = ret->_db->_meta->find_files_by_ext("aof");
	if(files.empty()){
		log_trace("empty aof");
		int seq;
		ret->_writer = ret->_db->_store->create_file("aof", &seq);
		ret->_db->_meta->add_file(seq, "aof");
	}else{
		ret->merge_files(files);
		files = ret->_db->_meta->find_files_by_ext("aof");
		int seq = files[0];
		std::string name = ret->_db->_store->make_filename(seq, "aof");
	
		ret->_mm->load(name);
		ret->_writer = AofWriter::open(name);
	}
	
	return ret;
}

int DbAof::merge_files(const std::vector<int> &files){
	log_trace("merge aof files:");
	int dst_seq = _db->_store->merge_files(files, "aof");
	_db->_meta->add_file(dst_seq, "aof");
	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		_db->_meta->del_file(seq);
	}
	return dst_seq;
}

///////////////////////////////////////////////////////

int DbAof::may_rotate_aof(){
	// compact aof files
	
	int seq = 0;
	if(_writer->size() > MAX_AOF_FILE_SIZE){
		delete _writer;
		_writer = _db->_store->create_file("aof", &seq);
		_db->_meta->add_file(seq, "aof");
	}
	return seq;
}

int DbAof::set(const std::string &key, const std::string &val){
	if(this->may_rotate_aof() == -1){
		return -1;
	}
	_writer->set(key, val);
	return 0;
}

int DbAof::del(const std::string &key){
	if(this->may_rotate_aof() == -1){
		return -1;
	}
	_writer->del(key);
	return 0;
}
