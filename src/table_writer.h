#ifndef SDB_TABLE_WRITER_H_
#define SDB_TABLE_WRITER_H_

#include <string>
#include "file_reader.h"

namespace sdb
{
	class TableWriter
	{
	public:
		~TableWriter();
		static TableWriter* open(const std::string &filename);

		int add(const char *data, int size);
	private:
		int _fd;
		TableWriter();
	};

}; // end namespace

#endif