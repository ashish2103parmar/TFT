TARGET := ../lib/libtftp.so
SRC := $(wildcard *.c)
Cflag := -I ../include
Lflag := -L ../lib

all: ${TARGET}

${TARGET} : ${SRC}
		gcc -fPIC -shared $^ ${Lflag} ${Cflag} -o $@

clean:
		rm ${TARGET}
