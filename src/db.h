#ifndef SDB_DB_H_
#define SDB_DB_H_

#include <string>
#include <vector>
#include <map>
#include "storage.h"
#include "db_meta.h"
#include "db_aof.h"

class Db
{
public:
	~Db();
	static Db* open(const std::string &path);

	bool set(const std::string &key, const std::string &val);
	bool del(const std::string &key);
private:
	friend class DbMeta;
	friend class DbAof;
	
	std::string _path;
	
	Storage *_store;
	DbMeta *_meta;
	DbAof *_aof;

	Db();
	
	int init();
	void load_root();
	void merge_roots(const std::vector<int> &roots);
};

#endif
