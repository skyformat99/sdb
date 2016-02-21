#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "file_reader.h"

namespace sdb
{
	FileReader::FileReader(int fd){
		_fd = fd;
		_buf_size = 128 * 1024;
		_buf = (char *)malloc(_buf_size);
		_data = _buf;
		_data_size = 0;
		_read_pos = 0;
	}

	FileReader::~FileReader(){
		if(_fd >= 0){
			close(_fd);
		}
		free(_buf);
	}
	
	FileReader* FileReader::open(const std::string &filename){
		int fd = ::open(filename.c_str(), O_RDONLY);
		if(fd == -1){
			return NULL;
		}
		FileReader *ret = new FileReader(fd);
		return ret;
	}

	bool FileReader::prepare(int count){
		if(this->size() < count){
			if(this->space() + this->size() < count){
				memcpy(_buf, this->data(), this->size());
				_data = _buf;
			}
			if(_buf_size < count){
				this->grow();
			}
		
			int ret = ::read(_fd, this->slot(), this->space());
			if(ret == -1){
				return false;
			}
			_data_size += ret;
		}
		return this->size() >= count;
	}
	
	void FileReader::skip(int count){
		_data += count;
		_data_size -= count;
	}
	
	char* FileReader::data(){
		return _data;
	}

	int FileReader::size(){
		return _data_size;
	}

	char* FileReader::slot() const{
		return _data + _data_size;
	}
	
	int FileReader::space() const{
		return _buf + _buf_size - this->slot();
	}

	int FileReader::grow(){ // 扩大缓冲区
		int n;
		if(_buf_size < 8 * 1024){
			n = 8 * 1024;
		}else if(_buf_size < 512 * 1024){
			n = 8 * _buf_size;
		}else{
			n = 2 * _buf_size;
		}
		//log_debug("Buffer resize %d => %d", total_, n);
		char *p = (char *)realloc(_buf, n);
		if(p == NULL){
			return -1;
		}
		_data = p + (_data - _buf);
		_buf = p;
		_buf_size = n;
		return _buf_size;
	}

}; // end namespace
