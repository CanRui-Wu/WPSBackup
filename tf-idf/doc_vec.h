#ifndef DOC_VEC
#define DOC_VEC
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <cmath>
#include <set>
#include <unordered_set>
using namespace std;
extern "C" void buildDict(vector<vector<string> > &,vector<string> &,unordered_set<string> &);
extern "C" void buildWordMap(vector<vector<string> > &,vector<string> &,map<string,pair<int,double> > &);
extern "C" void buildTfidf(vector<vector<string> > &,map<string,pair<int,double> > &,vector<vector<pair<int,double> > > &);
extern "C" void buildStopWord(unordered_set<string> &,string);
extern "C" void outputResult(vector<vector<pair<int,double> > > &,vector<string> &,string);
#endif