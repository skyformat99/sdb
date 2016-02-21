#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "table_writer.h"
#include <stdio.h>

namespace sdb
{
	TableWriter::TableWriter(){
		_fd = -1;
	}

	TableWriter::~TableWriter(){
		if(_fd >= 0){
			close(_fd);
		}
	}

	TableWriter* TableWriter::open(const std::string &filename){
		int fd = ::open(filename.c_str(), O_CREAT | O_APPEND, 0644);
		if(fd == -1){
			return NULL;
		}
		TableWriter *ret = new TableWriter();
		ret->_fd = fd;
		return ret;
	}

	int TableWriter::add(const char *data, int size){
		int header_len = 4;
		// TODO: 内存优化
		std::string buf;
		buf.resize(header_len + size);
		
		buf.append((char *)&size, 4);
		buf.append(data, size);
		
		int ret = ::write(_fd, buf.data(), buf.size());
		if(ret == -1){
			return -1;
		}
		return 0;
	}

}; // end namespace

