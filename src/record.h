#ifndef SDB_RECORD_H_
#define SDB_RECORD_H_

#include <string>

typedef enum{
	RECORD_SET = 'S',
	RECORD_DEL = 'D',
}RecordType;

class Record
{
public:
	RecordType type;
	std::string key;
	std::string val;
	
	bool is_set() const{
		return type == RECORD_SET;
	}
	
	bool is_del() const{
		return type == RECORD_DEL;
	}
};

#endif
