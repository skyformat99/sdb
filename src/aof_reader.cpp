#include <stdlib.h>
#include "aof_reader.h"
#include "block_reader.h"
#include "block.h"

namespace sdb
{
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
		
		while(1){
			BlockType type;
			char *buf;
			int buf_size;
			if(!ret->_reader->next(&buf, &buf_size)){
				break;
			}
			type = (BlockType)buf[0];
		
			if(!ret->_reader->next(&buf, &buf_size)){
				break;
			}
			std::string key(buf, buf_size);
		
			if(type == BLOCK_DEL){
				ret->dels.insert(key);
				ret->sets.erase(key);
			}else{
				if(!ret->_reader->next(&buf, &buf_size)){
					break;
				}
				std::string val(buf, buf_size);
				ret->dels.erase(key);
				ret->sets[key] = val;
			}
		}
		
		return ret;
	}
	
}; // end namespace

