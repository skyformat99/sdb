#ifndef SDB_DB_AOF_H_
#define SDB_DB_AOF_H_

#include <string>
#include <vector>
#include <map>
#include "memtable.h"

class Db;
class AofWriter;

class DbAof
{
public:
	~DbAof();
	void var_dump();
	static DbAof* create(Db *db);

	int set(const std::string &key, const std::string &val);
	int del(const std::string &key);

private:
	Db *_db;
	AofWriter *_writer;
	MemTable *_mm;
	
	DbAof();
	
	int merge_files(const std::vector<int> &files);
	int may_rotate_aof();
};

#endif
