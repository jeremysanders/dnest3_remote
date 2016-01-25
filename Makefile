CFLAGS = -O2 -Wall -I/home/jsanders/soft/include/dnest3
LIBS =  -ldnest3 -lgsl -lgslcblas -lboost_system -lboost_thread

LDLIBS = -L/home/jsanders/soft/lib -ldnest3 -lgsl -lgslcblas -lboost_system -lboost_thread -lm -lpthread
LDFLAGS =
OBJS = dnest3_remote.o RemoteModel.o
TARGETS = dnest3_remote

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $<

all: $(TARGETS)

dnest3_remote: $(OBJS)
	$(CXX) $(LDFLAGS) -o dnest3_remote $(OBJS) $(LDLIBS)

RemoteModel.o: RemoteModel.hpp

dnest3_remote.o: RemoteModel.hpp

clean:
	rm -f $(OBJS)

dist-clean: clean
	rm -f $(TARGETS)


default:
	g++ $(CFLAGS) -c *.cpp
	g++ -o dnest3_remote *.o $(LIBS)
	rm -f *.o
