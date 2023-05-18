#CFLAGS = -O2
#XINCLUDEDIR = /usr/X11R6/include
#XLIBDIR = /usr/X11R6/lib

all:
#	$(CC) $(CFLAGS) -c main.c -I$(XINCLUDEDIR)
	$(CC) $(CFLAGS) -c *.c
#	$(CC) $(CFLAGS) -L$(XLIBDIR) -lm -o vcgtload vcgtload.o -lX11 -lXxf86vm -lXext -lm
	$(CC) $(CFLAGS) -lm -o main *.o
clean:
	rm -f main
	rm -f *.o
