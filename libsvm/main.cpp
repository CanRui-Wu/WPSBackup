#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
using namespace std;

int main() {
	char train_file_name[1024] = "wps_train.txt";
	char test_file_name[1024] = "wps_test.txt";
	char output_file_name[1024] = "wps_output";
	char *error;
	void *handle = dlopen("./libtradition.so",RTLD_NOW);
	int (*traditionTrain)(char *);
	int (*traditionPredict)(char *,char *);
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&traditionPredict) = dlsym(handle,"traditionPredict");
	
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&traditionTrain) = dlsym(handle,"traditionTrain");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	int result;
	result = traditionTrain(train_file_name);
	if(result == 0) {
		exit(1);
	}
	result = traditionPredict(test_file_name,output_file_name);
	if(result == 0) {
		exit(1);
	}
	dlclose(handle);
}