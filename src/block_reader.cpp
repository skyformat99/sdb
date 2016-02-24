#include <stdlib.h>
#include "block_reader.h"
#include "file_reader.h"

BlockReader::BlockReader(){
	_reader = NULL;
}

BlockReader::~BlockReader(){
	delete _reader;
}

BlockReader* BlockReader::open(const std::string &filename){
	FileReader *fr = FileReader::open(filename);
	if(!fr){
		return NULL;
	}
	BlockReader *ret = new BlockReader();
	ret->_reader = fr;
	return ret;
}

void BlockReader::reset(){
	_reader->reset();
}

void BlockReader::seek(const std::string &target){
	_reader->reset();
	while(1){
		char *data;
		int size;
		if(!this->next(&data, &size)){
			break;
		}
		
		int min_len = (size < target.size()) ? size : target.size();
		int r = memcmp(data, target.data(), min_len);
		if(r == 0){
			if (size < target.size()) r = -1;
			else if (size > target.size()) r = +1;
		}
		if(r == -1){
			continue;
		}else{
			break;
		}
	}
}

bool BlockReader::next(char **data, int *size){
	if(!_reader->prepare(1)){
		return false;
	}
	int header_len = 0;
	int flag = _reader->data()[0] >> 6;
	if(flag == 2){
		header_len = 2;
		if(!_reader->prepare(header_len)){
			return false;
		}
		char b1 = _reader->data()[0];
		char b0 = _reader->data()[1];
		b1 &= ~((1 << 7) | (1 << 6));
		*size = (b1 << 8) + b0;
	}else if(flag == 3){
		header_len = 4;
		if(!_reader->prepare(header_len)){
			return false;
		}
		char b3 = _reader->data()[0];
		char b2 = _reader->data()[1];
		char b1 = _reader->data()[2];
		char b0 = _reader->data()[3];
		b3 &= ~((1 << 7) | (1 << 6));
		*size = (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
	}else{
		header_len = 1;
		*size = _reader->data()[0];
	}
	_reader->skip(header_len);

	if(!_reader->prepare(*size)){
		return false;
	}
	*data = _reader->data();
	_reader->skip(*size);
	return true;
}
