#include <iostream>
#include <map>
#include <cstdio>
#include <cstdlib>
using namespace std;

void loadWordMap(map<string,pair<int,double> > &word_map,char *tempfile) {
	FILE *fp = fopen(tempfile,"r");
	if(fp == NULL) {
		fprintf(stderr, "Error: Cannot Open %s\n", tempfile);
		exit(1);
	}
	char word[1000];
	int index;
	float idf_value;
	while(true) {
		fscanf(fp,"%s\n",word);
		string s(word);
		if(s == "我是分割标识符") {
			break;
		}
		fscanf(fp,"%d\n",&index);
		fscanf(fp,"%f\n",&idf_value);
		word_map[s] = make_pair(index,(double)idf_value);
		cout << index << endl;
	}
	fclose(fp);
}

int main() {
	map<string,pair<int,double> > word_map;
	char haha[30] = "haha.txt";
	loadWordMap(word_map,haha);
}