#include "db_aof.h"
#include "db.h"
#include "util/log.h"
#include "util/strings.h"
#include "aof_reader.h"
#include "aof_writer.h"

DbAof::DbAof(){
	_writer = NULL;
}

DbAof::~DbAof(){
	delete _writer;
}

DbAof* DbAof::create(Db *db){
	DbAof *ret = new DbAof();
	ret->_db = db;

	std::vector<int> files;
	files = ret->_db->_meta->find_files_by_ext("aof");
	if(files.empty()){
		int seq;
		ret->_writer = ret->_db->_store->create_file("aof", &seq);
		ret->_db->_meta->add_file(seq, "aof");
	}else{
		if(files.size() > 1){
			ret->merge_files(files);
			files = ret->_db->_meta->find_files_by_ext("aof");
		}
		int seq = files[0];
		std::string name = ret->_db->_store->make_filename(seq, "aof");

		ret->_writer = AofWriter::open(name);
	}
	
	return ret;
}

int DbAof::merge_files(const std::vector<int> &files){
	int dst_seq = _db->_store->merge_files(files, "aof");

	_db->_meta->add_file(dst_seq, "aof");
	for(int i=0; i<files.size(); i++){
		int seq = files[i];
		_db->_meta->del_file(seq);
	}
	
	// logging stuff
	std::string msg;
	msg.append("merge aof files [");
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

