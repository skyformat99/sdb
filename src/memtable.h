#ifndef SDB_MEMTABLE_H_
#define SDB_MEMTABLE_H_

#include <string>
#include <map>
#include "record.h"
#include "aof_writer.h"

class MemTable
{
public:
	MemTable();
	~MemTable();
	void var_dump();
	
	int size() const; // appoximated memory usage
	bool contains(const std::string &key) const;
	void set(const std::string &key, const std::string &val);
	void del(const std::string &key);

	void load(const std::string &filename);
	int save(AofWriter *writer) const;
private:
	int _size;
	std::map<std::string, Record> _records;

	void set(const Record &rec);
	void set(RecordType type, const std::string &key, const std::string &val);
};

#endif
