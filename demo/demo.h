#ifndef DEMO
#define DEMO
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <vector>
#include "cppjieba/Jieba.hpp"
#include <map>
using namespace std;
static void cutDocs(vector<string> &,vector<vector<string> > &);
static void buildData(vector<string> &,vector<string> &,vector<string> &,char *);
static void encrypt(const char*,int,const char *,const char *);
static void predict1(vector<string> & ,vector<string> &,char *,map<string,pair<int,double> > &);
static void predict2(vector<string> &,vector<string> &,vector<string> &,string,char *);
static void splitMapModel(char *,map<string,pair<int,double> > &);
static void saveWordMap(map<string,pair<int,double> > &,char *);
extern "C" void predict(vector<string> &,vector<string> &,vector<string> &,char *,char* );
extern "C" void train1(vector<string> &,vector<string> &,char *);
extern "C" void train2(vector<string> &,vector<string> &,vector<string> &,char *);
const char* const DICT_PATH = "dict/jieba.dict.utf8";
const char* const HMM_PATH = "dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "dict/user.dict.utf8";
const char* const IDF_PATH = "dict/idf.utf8";
const char* const STOP_WORD_PATH = "dict/stop_words.utf8";
cppjieba::Jieba jieba(DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH);
#endif
