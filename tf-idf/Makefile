CXX = g++
CFLAGS = -fPIC -shared -std=c++11

all:libdocvec.so

libdocvec.so:doc_vec.cpp doc_vec.h
	$(CXX) doc_vec.cpp $(CFLAGS) -o libdocvec.so

clean:
	rm libdocvec.so
