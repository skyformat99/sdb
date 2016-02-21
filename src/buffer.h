#ifndef SDB_BUFFER_H_
#define SDB_BUFFER_H_

#include <string>

namespace sdb
{
	class Buffer{
	private:
		char *buf;
		char *data_;
		int size_;
		int total_;
	public:
		Buffer(int total);
		~Buffer();

		int total() const{ // 缓冲区大小
			return total_;
		}

		bool empty() const{
			return size_ == 0;
		}

		// 指向数据
		char* data() const{
			return data_;
		}

		int size() const{
			return size_;
		}

		void decr(int num){
			size_ -= num;
			data_ += num;
		}
		
		// 扩大缓冲区
		int grow();

		int fill_from_fd(int fd);

	private:
		// 指向空间
		char* slot() const{
			return data_ + size_;
		}

		int space() const{
			return total_ - (int)(data_ - buf) - size_;
		}
		
		void re_arrange();
	};

};

#endif
