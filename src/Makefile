include ../config.mk

OBJS = log.o \
	file_reader.o block_writer.o block_reader.o aof_writer.o aof_reader.o \
	memtable.o \
	log_db.o

all: $(OBJS)
	#$(CXX) -o test_block.out test_block.cpp $(OBJS)
	#$(CXX) -o test_aof.out test_aof.cpp $(OBJS)
	#$(CXX) -o test_db.out test_db.cpp $(OBJS)
	$(CXX) -o test_log_db.out test_log_db.cpp $(OBJS)

log.o: util/log.h util/log.cpp
	$(CXX) $(CFLAGS) -c util/log.cpp
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.out

