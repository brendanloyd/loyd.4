CC	= gcc -g3
CFLAGS  = -g3
TARGET1 = child
TARGET2 = oss 

OBJS1	= child.o
OBJS2	= parent.o
OBJS3   = queue.o

all:	$(TARGET1) $(TARGET2)

$(TARGET1):	$(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1)

$(TARGET2):	$(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)

$(TARGET3):	$(OBJS3)
	$(CC) -o $(TARGET2) $(OBJS3)

child.o:	child.c
	$(CC) $(CFLAGS) -c child.c

parent.o:	parent.c
	$(CC) $(CFLAGS) -c parent.c

queue.o:	queue.c
	$(CC) $(CFLAGS) -c queue.c

clean:
	/bin/rm -f *.o $(TARGET1) $(TARGET2)

