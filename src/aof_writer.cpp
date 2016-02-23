#include <stdlib.h>
#include <map>
#include <set>
#include "aof_writer.h"
#include "aof_reader.h"
#include "block_writer.h"
#include "block.h"

AofWriter::AofWriter(){
	_writer = NULL;
}

AofWriter::~AofWriter(){
	delete _writer;
}

AofWriter* AofWriter::open(const std::string &filename){
	BlockWriter *bw = BlockWriter::open(filename);
	if(!bw){
		return NULL;
	}
	AofWriter *ret = new AofWriter();
	ret->_filename = filename;
	ret->_writer = bw;
	return ret;
}

int AofWriter::size() const{
	return _writer->size();
}

std::string AofWriter::filename() const{
	return _filename;
}

int AofWriter::set(const std::string &key, const std::string &val){
	char btype = (char)BLOCK_SET;
	int ret = 0;
	// TODO: write batch
	ret += _writer->append(&btype, 1);
	ret += _writer->append(key.data(), key.size());
	ret += _writer->append(val.data(), val.size());
	return ret;
}
int AofWriter::del(const std::string &key){
	char btype = (char)BLOCK_DEL;
	int ret = 0;
	// TODO: write batch
	ret += _writer->append(&btype, 1);
	ret += _writer->append(key.data(), key.size());
	return ret;
}

int AofWriter::merge_files(const std::vector<std::string> &src, const std::string &dst){
	std::map<std::string, std::string> sets;
	std::set<std::string> dels;
	
	for(int i=0; i<src.size(); i++){
		std::string filename = src[i];
		AofReader *reader = AofReader::open(filename);
		for(std::map<std::string, std::string>::iterator m_it=reader->sets.begin(); m_it!=reader->sets.end(); m_it++){
			const std::string &key = m_it->first;
			const std::string &val = m_it->second;
			sets[key] = val;
			dels.erase(key);
		}
		for(std::set<std::string>::iterator s_it=reader->dels.begin(); s_it!=reader->dels.end(); s_it++){
			const std::string &key = *s_it;
			sets.erase(key);
			dels.insert(key);
		}
		delete reader;
	}
	
	AofWriter *writer = AofWriter::open(dst);;
	for(std::map<std::string, std::string>::iterator m_it=sets.begin(); m_it!=sets.end(); m_it++){
		const std::string &key = m_it->first;
		const std::string &val = m_it->second;
		writer->set(key, val);
	}
	for(std::set<std::string>::iterator s_it=dels.begin(); s_it!=dels.end(); s_it++){
		const std::string &key = *s_it;
		writer->del(key);
	}
	delete writer;
	
	return 0;
}

