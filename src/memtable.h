#ifndef SDB_MEMTABLE_H_
#define SDB_MEMTABLE_H_

#include <string>
#include <map>
#include "record.h"

class MemTable
{
public:
	MemTable();
	~MemTable();
	void var_dump();
	
	int size() const;
	void set(const std::string &key, const std::string &val);
	void del(const std::string &key);

	void load(const std::string &filename);
private:
	int _size;
	std::map<std::string, Record> _records;

	void set(const Record &rec);
	void set(RecordType type, const std::string &key, const std::string &val);
};

#endif
