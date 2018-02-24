CXX = g++
CFLAGS = -I include/ -fPIC -shared -std=c++11

all:libtradition.so

libtradition.so:svm.cpp svm-predict.c svm-train.c
	$(CXX) svm.cpp svm-predict.c svm-train.c $(CFLAGS) -o libtradition.so

clean:
	rm libtradition.so
