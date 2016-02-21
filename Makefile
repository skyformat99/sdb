$(shell sh build.sh 1>&2)
include config.mk

all:
	cd src; ${MAKE}

.PHONY: ios
	
clean:
	cd src; ${MAKE} clean

clean_all: clean
	
