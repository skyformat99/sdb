#ifndef SDB_LOG_DB_H_
#define SDB_LOG_DB_H_

#include <string>
#include <vector>
#include <set>
#include "aof_writer.h"
#include "memtable.h"

/*
LogDb 是一个基于 aof(append only file) + memtable 的持久化数据库,
aof 用于持久化, 而 memtable 是持久化的全部数据在内存中的映射.
*/

class LogDb
{
public:
	~LogDb();
	void var_dump();
	static LogDb* open(const std::string &path, const std::string &ext);

	bool contains(const std::string &key);
	bool set(const std::string &key, const std::string &val);
	bool del(const std::string &key);
	
	// 尝试切换新的 log, 如果已切换, 返回新文件的 seq(>=0), 否则返回0
	int try_rotate_log();

private:
	std::string _path;
	std::string _ext;
	
	AofWriter *_log;
	MemTable *_mm;
	MemTable *_imm;

	std::set<int> _files;

	LogDb();

	int init_filesystem();
	int merge_logs();
	
	int next_file_seq();
	std::string make_filename(int seq);
	AofWriter* create_file(int *ret_seq=NULL);
	int remove_file(int seq);

};

#endif
