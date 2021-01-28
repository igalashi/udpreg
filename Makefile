#
#
#

CC = gcc
CFLAGS = -Wall -g -O
INCLUDES =
LIBS =

CXX = g++
CXXFLAGS = -Wall -g -O --std=c++0x

all: udpreg copperlite rbcp rbcpload

rbcpwin: rbcpWin.c rbcpWin.h rbcpWin_send.c myAtoi.c
	$(CC) $(CFLAGS) -o $@ $(INCDLUES) \
		rbcpWin.c \
		$(LIBS)

udpreg: udpreg.c
	$(CC) $(CFLAGS) -o $@ $(INCLUDES) \
		udpreg.c \
		$(LIBS)
rbcp: rbcp.c
	$(CC) $(CFLAGS) -o $@ $(INCLUDES) \
		-DTEST_MAIN -DDEBUG rbcp.c \
		$(LIBS)

copperlite: copperlite.c
	$(CC) $(CFLAGS) -o $@ $(INCLUDES) \
		copperlite.c \
		$(LIBS)

rbcpload: rbcpload.cxx rbcp.o rbcp.h
	$(CXX) $(CXXFLAGS) -o $@ $(INCLUDES) \
		rbcpload.cxx rbcp.o
		$(LIBS)

clean:
	rm -f rbcpwin udpreg copperlite rbcp rbcpload
	rm -f *.o

.c.o:
	$(CC) $(CFLAGS) -c $<
