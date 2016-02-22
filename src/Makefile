include ../config.mk

OBJS = log.o \
	file_reader.o block_writer.o block_reader.o

all: $(OBJS)
	$(CXX) -o test_block.out test_block.cpp $(OBJS)

log.o: util/log.h util/log.cpp
	$(CXX) $(CFLAGS) -c util/log.cpp
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.out

