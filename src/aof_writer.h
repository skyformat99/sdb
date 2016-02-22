#ifndef SDB_AOF_WRITER_H_
#define SDB_AOF_WRITER_H_

#include <string>

namespace sdb
{
	class BlockWriter;

	class AofWriter
	{
	public:
		~AofWriter();
		static AofWriter* open(const std::string &filename);

		int set(const std::string &key, const std::string &val);
		int del(const std::string &key);
	private:
		BlockWriter *_writer;
		AofWriter();
	};

}; // end namespace

#endif