#ifndef SDB_BLOCK_READER_H_
#define SDB_BLOCK_READER_H_

#include <string>

class FileReader;

class BlockReader
{
public:
	~BlockReader();
	static BlockReader* open(const std::string &filename);

	void seek(const std::string &target);
	bool next(char **data, int *size);
private:
	FileReader *_reader;
	BlockReader();
};

#endif