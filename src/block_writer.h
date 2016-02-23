#ifndef SDB_BLOCK_WRITER_H_
#define SDB_BLOCK_WRITER_H_

#include <string>

class BlockWriter
{
public:
	~BlockWriter();
	static BlockWriter* open(const std::string &filename);

	int size() const;
	int append(const char *data, int size);
private:
	int _fd;
	int _size;
	BlockWriter();
};

#endif