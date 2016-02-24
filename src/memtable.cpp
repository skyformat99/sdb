#include "memtable.h"
#include "util/log.h"
#include "aof_reader.h"

MemTable::MemTable(){
}

MemTable::~MemTable(){
}

void MemTable::var_dump(){
	log_trace("=== memtable ===");
	log_trace("  size: %d byte(s)", _size);
	for(std::map<std::string, Record>::iterator it=_records.begin(); it!=_records.end(); it++){
		Record &rec = it->second;
		log_trace("  %s %s", (rec.is_set()? "set":"del"), rec.key.c_str());
	}
	log_trace("=== total %d ===", _records.size());
}

int MemTable::size() const{
	return _size;
}

void MemTable::set(const std::string &key, const std::string &val){
	this->set(RECORD_SET, key, val);
}

void MemTable::del(const std::string &key){
	this->set(RECORD_DEL, key, "");
}

void MemTable::set(const Record &rec){
	int bytes = 4 + rec.key.size() + rec.val.size();
	int incr_bytes = 0;

	std::map<std::string, Record>::iterator it;
	it = _records.find(rec.key);
	if(it != _records.end()){
		Record old = it->second;
		int old_bytes = 4 + old.key.size() + old.val.size();
		incr_bytes = bytes - old_bytes;
	}
	
	_records[rec.key] = rec;
	_size += incr_bytes;
}

void MemTable::set(RecordType type, const std::string &key, const std::string &val){
	Record rec;
	rec.type = type;
	rec.key = key;
	rec.val = val;
	this->set(rec);
}

void MemTable::load(const std::string &filename){
	AofReader *reader = AofReader::open(filename);
	Record rec;
	while(reader->next(&rec)){
		this->set(rec);
	}
	delete reader;
}

