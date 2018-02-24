#include <iostream>
#include <vector>
#include <cstdio>
using namespace std;

const char a[] = "ohmygod";

void encrypt(const char * input_file_name) {
	char key[] = "ohmygod";
	FILE *fp = fopen(input_file_name,"r");
	vector<char> myvector;
	char b;
	int index = 0;
	while(true) {
		b = getc(fp);
		if(feof(fp))
			break;
		//fprintf(fp2,"%c",b^a[index%7]);
		myvector.push_back(b^a[index%7]);
		index++;
	}
	fclose(fp);
	FILE *fp2 = fopen(input_file_name,"w");
	for(auto &c:myvector) {
		fprintf(fp2, "%c", c);
	}
	fclose(fp2);
}


int main() {
	encrypt("1");
}