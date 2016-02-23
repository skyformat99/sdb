#include "memtable.h"

Memtable::Memtable(){
}

Memtable::~Memtable(){
}

int Memtable::size() const{
	return _size;
}

void Memtable::set(const std::string &key, const std::string &val){
	this->set(RECORD_SET, key, val);
}

void Memtable::del(const std::string &key){
	this->set(RECORD_DEL, key, "");
}

void Memtable::set(RecordType type, const std::string &key, const std::string &val){
	Record rec;
	rec.type = type;
	rec.key = key;
	rec.val = val;
	int bytes = 4 + key.size() + val.size();
	int incr_bytes = 0;

	std::map<std::string, Record>::iterator it;
	it = _records.find(key);
	if(it != _records.end()){
		Record old = it->second;
		int old_bytes = 4 + old.key.size() + old.val.size();
		incr_bytes = bytes - old_bytes;
	}
	
	_records[key] = rec;
	_size += incr_bytes;
}

