include ../config.mk

OBJS = log.o \
	table_writer.o table_reader.o file_reader.o

all: $(OBJS)
	$(CXX) -o test.out test.cpp $(OBJS)

log.o: util/log.h util/log.cpp
	$(CXX) $(CFLAGS) -c util/log.cpp
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.out

