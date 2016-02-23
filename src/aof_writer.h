#ifndef SDB_AOF_WRITER_H_
#define SDB_AOF_WRITER_H_

#include <string>
#include <vector>

class BlockWriter;

class AofWriter
{
public:
	~AofWriter();
	static AofWriter* open(const std::string &filename);
	static int merge_files(const std::vector<std::string> &src, const std::string &dst);
	
	int size() const;
	std::string filename() const;

	int set(const std::string &key, const std::string &val);
	int del(const std::string &key);
private:
	BlockWriter *_writer;
	std::string _filename;
	AofWriter();
};

#endif