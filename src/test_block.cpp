#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/log.h"
#include "block_reader.h"
#include "block_writer.h"

using namespace sdb;

int main(int argc, char **argv){
	std::string filename = "a.db";
	
	BlockWriter *writer = BlockWriter::open(filename);
	if(!writer){
		log_error("error %s", strerror(errno));
		exit(0);
	}
	
	int count = 100000;
	
	log_debug("");
	srand(time(NULL));
	for(int i=0; i<count; i++){
		int num = rand();
		char buf[128];
		snprintf(buf, sizeof(buf), "%d", num);
		int ret = writer->add(buf, strlen(buf));
		if(ret == -1){
			log_error("error %s", strerror(errno));
			exit(0);
		}
		//printf("Press Enter: ");
		//getchar();
	}
	log_debug("");
	delete writer;
	
	
	BlockReader *reader = BlockReader::open(filename);
	if(!reader){
		log_error("error %s", strerror(errno));
		exit(0);
	}
	char *data;
	int size;
	int n = 0;
	log_debug("");
	while(reader->get(&data, &size)){
		n ++;
		//std::string s(data, size);
		//log_debug("%s", s.c_str());
	}
	log_debug("%d item(s)", n);
	
	delete reader;
	return 0;
}
