#ifndef SDB_DB_AOF_H_
#define SDB_DB_AOF_H_

#include <string>
#include <vector>
#include <map>

class Db;
class AofWriter;

class DbAof
{
public:
	~DbAof();
	static DbAof* create(Db *db);

	int set(const std::string &key, const std::string &val);
	int del(const std::string &key);

private:
	Db *_db;
	AofWriter *_writer;
	// TODO: memtable
	
	DbAof();
	int merge_files(const std::vector<int> &files);
};

#endif
