#include <map>
#include <set>
#include "db.h"
#include "aof_writer.h"
#include "aof_reader.h"
#include "util/file.h"
#include "util/strings.h"
#include "util/log.h"

#define MAX_DB_SIZE       (1 * 1024)

Db::Db(){
	_root = NULL;
	_aof = NULL;
}

Db::~Db(){
	delete _root;
	delete _aof;
}

Db* Db::open(const std::string &path){
	std::string dir = path;
	if(path[path.size() - 1] != '/'){
		dir = path + "/";
	}
	Db *db = new Db();
	db->_path = dir;
	if(db->init() == -1){
		delete db;
		return NULL;
	}
	return db;
}

int Db::init(){
	_store = Storage::open(_path);
	this->load_root();
	return 0;
}

void Db::load_root(){
	std::vector<int> roots;
	
	roots = _store->find_files_by_ext("root");
	if(roots.empty()){
		_store->create_file("root");
	}else if(roots.size() > 1){
		this->merge_roots(roots);
	}
	roots = _store->find_files_by_ext("root");

	int root_seq = roots[0];
	std::string name = _store->make_filename(root_seq, "root");
	_root = Root::load(name);
}

void Db::merge_roots(const std::vector<int> &roots){
	std::vector<std::string> src;
	for(int i=0; i<roots.size(); i++){
		int seq = roots[i];
		std::string name = _store->make_filename(seq, "root");
		src.push_back(name);
	}
	
	AofWriter *writer = _store->create_file("root");
	std::string dst = writer->filename();
	delete writer;
	AofWriter::merge_files(src, dst);

	for(int i=0; i<roots.size(); i++){
		int seq = roots[i];
		_store->remove_file(seq);
	}
}

/////////////////////////////////////////////////////////

bool Db::set(const std::string &key, const std::string &val){
	if(_aof->size() > MAX_DB_SIZE){
		delete _aof;
		// TODO: _root->log();
		_aof = _store->create_file("aof");
	}
	_aof->set(key, val);
	return true;
}

bool Db::del(const std::string &key){
	if(_aof->size() > MAX_DB_SIZE){
		delete _aof;
		// TODO: _root->log();
		_aof = _store->create_file("aof");
	}
	_aof->del(key);
	return true;
}
