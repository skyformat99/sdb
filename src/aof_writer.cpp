#include <stdlib.h>
#include <map>
#include <set>
#include "aof_writer.h"
#include "aof_reader.h"
#include "block_writer.h"

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
	char btype = (char)RECORD_SET;
	int ret = 0;
	// TODO: write batch
	ret += _writer->append(&btype, 1);
	ret += _writer->append(key.data(), key.size());
	ret += _writer->append(val.data(), val.size());
	return ret;
}

int AofWriter::del(const std::string &key){
	char btype = (char)RECORD_DEL;
	int ret = 0;
	// TODO: write batch
	ret += _writer->append(&btype, 1);
	ret += _writer->append(key.data(), key.size());
	return ret;
}

int AofWriter::merge_files(const std::vector<std::string> &src, const std::string &dst){
	std::map<std::string, Record> records;

	for(int i=0; i<src.size(); i++){
		std::string filename = src[i];
		AofReader *reader = AofReader::open(filename);
		reader->read_all(records);
		delete reader;
	}
	
	AofWriter *writer = AofWriter::open(dst);
	for(std::map<std::string, Record>::iterator it=records.begin(); it!=records.end(); it++){
		Record &rec = it->second;
		if(rec.is_set()){
			writer->set(rec.key, rec.val);
		}else{
			writer->del(rec.key);
		}
	}
	delete writer;
	
	return 0;
}
