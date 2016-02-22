#include "db.h"
#include "aof_writer.h"
#include "util/file.h"
#include "util/strings.h"
#include "util/log.h"

#define MIN_FILENAME_SEQ  1
#define MAX_FILENAME_SEQ  100
#define MAX_DB_SIZE       (1 * 1024)

namespace sdb
{
	Db::Db(){
		_root = NULL;
		_aof = NULL;
	}
	
	Db::~Db(){
		delete _root;
		delete _aof;
	}
	
	std::string Db::make_filename(int seq, const std::string &ext){
		char buf[256];
		snprintf(buf, sizeof(buf), "%s%08d.%s", _path.c_str(), seq, ext.c_str());
		return buf;
	}
	
	void Db::init_filenames(){
		std::vector<std::string> files = scandir(_path);
		std::vector<std::string>::iterator it;
		for(it=files.begin(); it!=files.end(); it++){
			const std::string &name = *it;
			int num = str_to_int(name);
			std::string::size_type pos;
			pos = name.find_last_of(".");
			if(pos == std::string::npos){
				continue;
			}
			std::string ext = name.substr(pos + 1);
			if(ext == "root" || ext == "aof" || ext == "db"){
				_filenames[num] = ext;
			}
		}
	}
	
	std::string Db::find_newest_filename(const std::string &ext){
		std::map<int, std::string>::reverse_iterator rit;
		for(rit=_filenames.rbegin(); rit!=_filenames.rend(); rit++){
			if(rit->second == ext){
				return make_filename(rit->first, rit->second);
			}
		}
		return "";
	}
	
	int Db::next_filename_seq(){
		int ret = MIN_FILENAME_SEQ;
		std::map<int, std::string>::reverse_iterator rit;
		rit = _filenames.rbegin();
		if(rit != _filenames.rend()){
			ret = rit->first + 1;
			if(ret > MAX_FILENAME_SEQ){
				ret = MIN_FILENAME_SEQ;
				std::map<int, std::string>::iterator it;
				for(it=_filenames.begin(); it!=_filenames.end(); it++){
					if(it->first != ret){
						break;
					}else{
						ret = it->first + 1;
					}
				}
			}
		}
		return ret;
	}
	
	Db* Db::open(const std::string &path){
		std::string dir = path;
		if(path[path.size() - 1] != '/'){
			dir = path + "/";
		}
		Db *db = new Db();
		db->_path = dir;
		if(db->init() == -1){
			delete db;
			return NULL;
		}
		return db;
	}
	
	int Db::init(){
		if(!file_exists(_path)){
			mkdir(_path.c_str(), 0744);
		}
		
		this->init_filenames();
		// TODO: recover();
		
		std::string root = this->find_newest_filename("root");
		if(root.empty()){
			int seq = next_filename_seq();
			// TODO: if(seq < min_root_seq){ migrate_root_files();}
			root = make_filename(seq, "root");
			_filenames[seq] = "root";
		}
		_root = AofWriter::open(root);
		if(!_root){
			log_trace("error open root: %s", root.c_str());
			return -1;
		}

		std::string aof = this->find_newest_filename("aof");
		if(aof.empty()){
			_aof = this->make_aof();
		}else{
			_aof = AofWriter::open(aof);
		}
		if(!_aof){
			log_trace("error open aof: %s", aof.c_str());
			return -1;
		}

		return 0;
	}
	
	AofWriter* Db::make_aof(){
		// TODO: if(seq < min_aof_seq){ migrate_aof_files();}
		int seq = next_filename_seq();
		std::string aof = make_filename(seq, "aof");
		_filenames[seq] = "aof";
		AofWriter *ret = AofWriter::open(aof);
		log_trace("new oaf: %s", aof.c_str());
		_root->set(aof, "");
		return ret;
	}
	
	bool Db::set(const std::string &key, const std::string &val){
		_aof->set(key, val);
		if(_aof->size() > MAX_DB_SIZE){
			delete _aof;
			_aof = this->make_aof();
		}
		return true;
	}
	
	bool Db::del(const std::string &key){
		_aof->del(key);
		if(_aof->size() > MAX_DB_SIZE){
			delete _aof;
			_aof = this->make_aof();
		}
		return true;
	}

}; // end namespace
