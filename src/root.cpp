#include "root.h"
#include "aof_reader.h"
#include "util/strings.h"

Root::Root(){	
}

Root::~Root(){
}

Root* Root::load(const std::string &filename){
	Root *ret = new Root();
	
	AofReader *reader = AofReader::open(filename);
	for(std::map<std::string, std::string>::iterator m_it=reader->sets.begin(); m_it!=reader->sets.end(); m_it++){
		const std::string &key = m_it->first;
		const std::string &val = m_it->second;
		int seq = str_to_int(key);
		ret->_items[seq] = val;
		ret->_del_items.erase(seq);
	}
	for(std::set<std::string>::iterator s_it=reader->dels.begin(); s_it!=reader->dels.end(); s_it++){
		const std::string &key = *s_it;
		int seq = str_to_int(key);
		ret->_items.erase(seq);
		ret->_del_items.insert(seq);
	}
	delete reader;
	
	return ret;
}
