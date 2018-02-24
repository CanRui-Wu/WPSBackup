CXX = g++
CFLAGS = -I ../jieba/include/ -I ../jieba/deps/ -ldl -fPIC -shared -std=c++11

all:libdemo.so

libdemo.so:demo.cpp
	$(CXX) demo.cpp $(CFLAGS) -o libdemo.so

clean:
	rm libdemo.so
