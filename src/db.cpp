#include <map>
#include <set>
#include "db.h"
#include "util/file.h"
#include "util/strings.h"
#include "util/log.h"

Db::Db(){
	_meta = NULL;
	_aof = NULL;
}

Db::~Db(){
	delete _meta;
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
	_store = DbStore::open(_path);
	_meta = DbMeta::create(this);
	_aof = DbAof::create(this);
	
	_meta->var_dump();
	_aof->var_dump();
	return 0;
}


/////////////////////////////////////////////////////////

bool Db::set(const std::string &key, const std::string &val){
	_aof->set(key, val);
	return true;
}

bool Db::del(const std::string &key){
	_aof->del(key);
	return true;
}
