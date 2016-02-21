#include <stdlib.h>
#include "table_reader.h"

namespace sdb
{
	TableReader::TableReader(){
		_reader = NULL;
	}

	TableReader::~TableReader(){
		delete _reader;
	}

	TableReader* TableReader::open(const std::string &filename){
		FileReader *fr = FileReader::open(filename);
		if(!fr){
			return NULL;
		}
		TableReader *ret = new TableReader();
		ret->_reader = fr;
		return ret;
	}

	void TableReader::seek(const std::string &target){
		while(1){
			char *data;
			int size;
			if(!this->get(&data, &size)){
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
	
	bool TableReader::get(char **data, int *size){
		int header_len = 4;
		if(!_reader->prepare(header_len)){
			return false;
		}
		_reader->skip(header_len);
		*size = *((int32_t *)_reader->data());
		if(!_reader->prepare(*size)){
			return false;
		}
		*data = _reader->data();
		_reader->skip(*size);
		return true;
	}
	
}; // end namespace

