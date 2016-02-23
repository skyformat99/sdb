#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util/log.h"
#include "db.h"

//using namespace sdb;

int main(int argc, char **argv){
	set_log_level("trace");
	std::string path = "./var";
	
	Db *db = Db::open(path);
	if(!db){
		log_error("error %s", strerror(errno));
		exit(0);
	}
	
	int count = 100000;

	log_debug("");
	srand(time(NULL));
	for(int i=0; i<count; i++){
		printf("Press Enter: ");
		getchar();
		int num = rand() % 10;
		char buf[128];
		snprintf(buf, sizeof(buf), "%100d", num);
		int ret;
		if(rand() % 2 == 0){
			ret = db->set(buf, "val");
		}else{
			ret = db->del(buf);
		}
		if(ret == -1){
			log_error("error %s", strerror(errno));
			exit(0);
		}
	}
	log_debug("");

	delete db;
	return 0;
}
