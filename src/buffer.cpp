#include "buffer.h"

Buffer::Buffer(int total){
	size_ = 0;
	total_ = origin_total = total;
	buf = (char *)malloc(total);
	data_ = buf;
}

Buffer::~Buffer(){
	free(buf);
}

int Buffer::read(int fd){
	int ret = ::read(fd, this->slot(), this->space());
	size_ += ret;
	return ret;
}

void Buffer::re_arrange(){
	memcpy(_buf, this->data(), this->size());
	_data = _buf;
}
