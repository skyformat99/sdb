#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "block_writer.h"
#include "util/log.h"

namespace sdb
{
	BlockWriter::BlockWriter(){
		_fd = -1;
	}

	BlockWriter::~BlockWriter(){
		if(_fd >= 0){
			close(_fd);
		}
	}

	BlockWriter* BlockWriter::open(const std::string &filename){
		int fd = ::open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if(fd == -1){
			return NULL;
		}
		BlockWriter *ret = new BlockWriter();
		ret->_fd = fd;
		ret->_size = lseek(fd, 0, SEEK_END);
		return ret;
	}
	
	int BlockWriter::size() const{
		return _size;
	}

	int BlockWriter::append(const char *data, int size){
		// TODO: 内存优化
		std::string buf;
		buf.reserve(4 + size);

		int header_len = 0;
		if(size <= (0xff>>1)){
			// leading bit: 0
			header_len = 1;
			char b0 = (char)(size & 0xff);
			buf.push_back(b0);
		}else if(size <= (0xffff>>2)){
			// leading bit: 10
			header_len = 2;
			char b0 = (char)(size & 0xff);
			char b1 = (char)((size >> 8) & 0xff);
			b1 |= (1<<7);
			buf.push_back(b1);
			buf.push_back(b0);
		}else{
			// leading bit: 11
			header_len = 4;
			char b0 = (char)(size & 0xff);
			char b1 = (char)((size >>  8) & 0xff);
			char b2 = (char)((size >> 16) & 0xff);
			char b3 = (char)((size >> 24) & 0xff);
			b3 |= (1<<7) | (1<<6);
			buf.push_back(b3);
			buf.push_back(b2);
			buf.push_back(b1);
			buf.push_back(b0);
		}
		buf.append(data, size);
		
		int ret = ::write(_fd, buf.data(), buf.size());
		if(ret == -1){
			return -1;
		}
		_size += ret;
		return ret;
	}

}; // end namespace

