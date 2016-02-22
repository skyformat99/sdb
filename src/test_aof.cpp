#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/log.h"
#include "aof_reader.h"
#include "aof_writer.h"

using namespace sdb;

int main(int argc, char **argv){
	std::string filename = "a.db";
	
	AofWriter *writer = AofWriter::open(filename);
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
		int ret;
		if(rand() % 2 == 0){
			ret = writer->set(buf, "val");
		}else{
			ret = writer->del(buf);
		}
		if(ret == -1){
			log_error("error %s", strerror(errno));
			exit(0);
		}
		//printf("Press Enter: ");
		//getchar();
	}
	log_debug("");
	delete writer;
	
	
	AofReader *reader = AofReader::open(filename);
	if(!reader){
		log_error("error %s", strerror(errno));
		exit(0);
	}
	
	delete reader;
	return 0;
}
