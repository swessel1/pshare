COMPILER = g++
CFLAGS = -std=c++11 -Wall -I include

OBJ = TcpListener.o \
	  TcpListenerEvent.o \
	  Event.o \
	  EventRegistrar.o \
	  Node.o \
	  NetworkStructure.o

pshare: $(OBJ)
	$(COMPILER) $(CFLAGS) -o $@ $^ main.cpp -pthread

%.o: src/%.cpp
	$(COMPILER) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o pshare
