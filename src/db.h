#ifndef SDB_DB_H_
#define SDB_DB_H_

#include <string>
#include <map>

namespace sdb
{
	class AofWriter;
	
	class Db
	{
	public:
		~Db();
		static Db* open(const std::string &path);

		bool set(const std::string &key, const std::string &val);
		bool del(const std::string &key);
	private:
		std::string _path;
		AofWriter *_root;
		AofWriter *_aof;
		std::map<int, std::string> _filenames;
		Db();
		
		int init();
		
		void init_filenames();
		std::string find_newest_filename(const std::string &ext);
		int next_filename_seq();
		std::string make_filename(int seq, const std::string &ext);
		
		AofWriter* make_aof();
		AofWriter* make_table(int *seq);
		void compact_aofs();
	};

}; // end namespace

#endif
