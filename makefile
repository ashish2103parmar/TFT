SRCS  := $(wildcard *.c)
TARGET := $(SRCS:.c=.out)
LFLAG := -L lib -ltftp
CFLAG := -I include

all : ${TARGET} 

%.out:%.c
	gcc  $^ ${LFLAG} ${CFLAG} -o $@

clean:
		rm ${TARGET}
