#ifndef SDB_AOF_READER_H_
#define SDB_AOF_READER_H_

#include <string>
#include <map>
#include <set>
#include "record.h"
#include "block_reader.h"

class AofReader
{
public:
	~AofReader();
	static AofReader* open(const std::string &filename);
	
	void reset();
	bool next(Record *rec);
	
	void read_all(std::map<std::string, Record> &records);
private:
	BlockReader *_reader;
	AofReader();
};

#endif