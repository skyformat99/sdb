#include <stdlib.h>
#include "aof_writer.h"
#include "block_writer.h"
#include "block.h"

namespace sdb
{
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
		ret->_writer = bw;
		return ret;
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

}; // end namespace

