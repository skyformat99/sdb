#ifndef SDB_TABLE_READER_H_
#define SDB_TABLE_READER_H_

#include <string>
#include "file_reader.h"

namespace sdb
{
	class TableReader
	{
	public:
		~TableReader();
		static TableReader* open(const std::string &filename);

		void seek(const std::string &target);
		bool get(char **data, int *size);
	private:
		FileReader *_reader;
		TableReader();
	};

}; // end namespace

#endif