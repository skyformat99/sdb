#include <stdlib.h>
#include "aof_reader.h"

AofReader::AofReader(){
	_reader = NULL;
}

AofReader::~AofReader(){
	delete _reader;
}

AofReader* AofReader::open(const std::string &filename){
	BlockReader *br = BlockReader::open(filename);
	if(!br){
		return NULL;
	}
	AofReader *ret = new AofReader();
	ret->_reader = br;
	return ret;
}

void AofReader::reset(){
	_reader->reset();
}

bool AofReader::next(Record *rec){
	char *buf;
	int buf_size;
	
	// type
	if(!_reader->next(&buf, &buf_size)){
		return false;
	}
	rec->type = (RecordType)buf[0];

	// key
	if(!_reader->next(&buf, &buf_size)){
		return false;
	}
	rec->key.assign(buf, buf_size);

	// val
	if(rec->is_set()){
		if(!_reader->next(&buf, &buf_size)){
			return false;
		}
		rec->val.assign(buf, buf_size);
	}

	return true;
}

void AofReader::read_all(std::map<std::string, Record> &records){
	Record rec;
	while(this->next(&rec)){
		records[rec.key] = rec;
	}
}
