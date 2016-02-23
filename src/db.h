#ifndef SDB_DB_H_
#define SDB_DB_H_

#include <string>
#include <vector>
#include <map>
#include "root.h"
#include "storage.h"

class Db
{
public:
	~Db();
	static Db* open(const std::string &path);

	bool set(const std::string &key, const std::string &val);
	bool del(const std::string &key);
private:
	std::string _path;
	Storage *_store;
	Root *_root;
	
	AofWriter *_aof;
	Db();
	
	int init();
	void load_root();
	void merge_roots(const std::vector<int> &roots);
};

#endif
