#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "cppjieba/Jieba.hpp"
using namespace std;

int main() {
	cppjieba::Jieba jieba(DICT_PATH,
	HMM_PATH,
	USER_DICT_PATH,
	IDF_PATH,
	STOP_WORD_PATH);
	vector<string> words;
	string s = "哈哈哈";
	jieba.CutAll(s,words);
}