#ifndef SDB_FILE_READER_H_
#define SDB_FILE_READER_H_

#include <string>

class FileReader
{
public:
	static FileReader* open(const std::string &filename);
	~FileReader();
	void reset();
	bool prepare(int count);
	// skip 后, 原来的 data() 指向还是有效的
	void skip(int count);
	// 指向数据
	char *data();
	// data size
	int size();
private:
	char *_buf;
	int _buf_size;
	char *_data;
	int _data_size;
	int _fd;
	int _read_pos;

	FileReader(int fd);
	// 指向空间
	char* slot() const;
	int space() const;
	// 扩大缓冲区
	int grow();
};

#endif
