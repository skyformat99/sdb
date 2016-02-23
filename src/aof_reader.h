#ifndef SDB_AOF_READER_H_
#define SDB_AOF_READER_H_

#include <string>
#include <map>
#include <set>

class BlockReader;

class AofReader
{
public:
	std::set<std::string> dels;
	std::map<std::string, std::string> sets;
	~AofReader();
	static AofReader* open(const std::string &filename);
private:
	BlockReader *_reader;
	AofReader();
};

#endif