#ifndef SDB_MEMTABLE_H_
#define SDB_MEMTABLE_H_

#include <string>
#include <map>
#include "record.h"

class Memtable
{
public:
	Memtable();
	~Memtable();
	int size() const;
	void set(const std::string &key, const std::string &val);
	void del(const std::string &key);
private:
	int _size;
	std::map<std::string, Record> _records;

	void set(RecordType type, const std::string &key, const std::string &val);
};

#endif
