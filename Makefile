CFLAGS = -O2 -Wall -I/usr/local/include/dnest3
LIBS =  -ldnest3 -lgsl -lgslcblas -lboost_system -lboost_thread

LDLIBS = -ldnest3 -lgsl -lgslcblas -lboost_system -lboost_thread -lm -lpthread
LDFLAGS =
OBJS = main.o RemoteModel.o
TARGETS = main

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $<

all: $(TARGETS)

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

RemoteModel.o: RemoteModel.hpp

main.o: RemoteModel.hpp

clean:
	rm -f $(OBJS)

dist-clean: clean
	rm -f $(TARGETS)


default:
	g++ $(CFLAGS) -c *.cpp
	g++ -o main *.o $(LIBS)
	rm -f *.o
