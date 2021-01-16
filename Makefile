CC     = gcc
CFLAGS = -Wall -g -std=c99
CCLINK =$(CC)
OBJS   = grades.o
LIB   = libgrades.so
RM     = rm -rf 

$(LIB): grades.o
	$(CCLINK) -shared grades.o -o libgrades.so -llinked-list -L.

grades.o: grades.c grades.h linked-list.h
	$(CC) $(CFLAGS) -c -fPIC grades.c



clean:
	$(RM) $(LIB) $(OBJS)