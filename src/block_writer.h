#ifndef SDB_BLOCK_WRITER_H_
#define SDB_BLOCK_WRITER_H_

#include <string>

namespace sdb
{
	class BlockWriter
	{
	public:
		~BlockWriter();
		static BlockWriter* open(const std::string &filename);

		int add(const char *data, int size);
	private:
		int _fd;
		BlockWriter();
	};

}; // end namespace

#endif