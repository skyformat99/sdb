#ifndef SDB_DB_STORE_H_
#define SDB_DB_STORE_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include "aof_writer.h"

class DbStore
{
public:
	~DbStore();
	static DbStore* open(const std::string &path);
	
	std::string make_filename(int seq, const std::string &ext);
	std::vector<int> find_files_by_ext(const std::string &ext) const;
	
	AofWriter* create_file(const std::string &ext, int *ret_seq=NULL);
	int remove_file(int seq);
	// return new file seq
	int merge_files(const std::vector<int> &src, const std::string &ext);

private:
	std::string _path;
	std::map<int, std::string> _files; // seq => ext
	DbStore();
	int next_file_seq();
	void load_file_seqs();
};

#endif
