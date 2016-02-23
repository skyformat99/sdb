#ifndef SDB_DB_META_H_
#define SDB_DB_META_H_

#include <string>
#include <vector>
#include <map>

class Db;
class AofWriter;

class DbMeta
{
public:
	DbMeta();
	~DbMeta();
	static DbMeta* create(Db *db);
	
	int add_file(int seq, const std::string &ext);
	int del_file(int seq);

	std::vector<int> find_files_by_ext(const std::string &ext) const;
	
private:
	Db *_db;
	AofWriter *_writer;
	std::map<int, std::string> _files;
	
	int load_file(const std::string &filename);
	int merge_files(const std::vector<int> &files);
};

#endif
