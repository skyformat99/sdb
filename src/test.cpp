#include "table_reader.h"
#include "table_writer.h"
#include "util/log.h"

using namespace sdb;

int main(int argc, char **argv){
	TableWriter *writer = TableWriter::open("a.db");
	if(!writer){
		log_error("error %s", strerror(errno));
		exit(0);
	}
	log_debug("open");
	return 0;
}
