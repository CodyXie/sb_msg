

all : alinkd

OBJS:=$(patsubst %.c, %.o, $(wildcard *.c))

%.o : %.c
	$(CC) -fPIC -g -c $^ -o $@

alinkd : $(OBJS)
	$(CC) $^ -o $@ -lpthread

.PHONY : clean all
clean :
	rm -rf *.o
	rm -rf alinkd
