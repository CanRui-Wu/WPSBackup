#include "cppjieba/Jieba.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <cmath>
#include "json/json.h"
using namespace std;

//Build the dictionary by giving text_list except the stop_word 
void buildDict(vector<vector<string> > &all_text,vector<string> &dict,unordered_set<string> &stop_words) {
	char bar[100] = "Processing[-------------------------]";
	printf("%s\r",bar);
	fflush(stdout);
	double current_process;
	int bar_index = 11; 
	for(int i = 0;i < all_text.size();i++) {
		current_process = double(i)/(all_text.size()-1);
		while(current_process >= (bar_index-10)*0.04) {
			bar[bar_index] = '>';
			bar_index++;
		}
		if(i == all_text.size()-1) {
			printf("%s\n",bar);
		} else {
			printf("%s\r",bar);
		}
		fflush(stdout);
		for(int j = 0;j < all_text[i].size();j++) {
			if(stop_words.find(all_text[i][j]) == stop_words.end()) {
				if(all_text[i][j].find('\n') == string::npos)
					dict.push_back(all_text[i][j]);
			}
		}
	}
	sort(dict.begin(),dict.end());
	dict.erase(unique(dict.begin(),dict.end()),dict.end());
	//dict.erase(dict.begin(),dict.begin()+10); //Delete the special string
}

//Build the word_map,mapping from string to <int,double>,the first value represent word_id,the second value represent word idf value 
void buildWordMap(vector<vector<string> > &all_text,vector<string> &dict,map<string,pair<int,double> > &word_map) {
	for(int i = 0;i < dict.size();i++) {
		word_map[dict[i]] = make_pair(i+1,0.0);
	}
	int *temp = new int[dict.size()+1];
	for(int i = 0;i < all_text.size();i++) {
		memset(temp,0,sizeof(int)*dict.size());
		for(int j = 0;j < all_text[i].size();j++) {
			if(word_map.find(all_text[i][j]) == word_map.end())
				continue;
			if(temp[word_map[all_text[i][j]].first] == 1) //Same word in a document should not add twice.
				continue;
			temp[word_map[all_text[i][j]].first] = 1;
			word_map[all_text[i][j]].second += 1;
		}
	}
	for(int i = 0;i < dict.size();i++) {
		word_map[dict[i]].second = log((1+all_text.size())/(1+word_map[dict[i]].second)); //Caculate idf for each word
	}
}

//Build the tf-idf-result by related text_list and pre-computing word_map
void buildTfidf(vector<vector<string> > &all_text,map<string,pair<int,double> > &word_map,vector<vector<pair<int,double> > > &tf_idf_result) {
	for(int i = 0;i < all_text.size();i++) {
		vector<string> temp;
		vector<pair<int,double> > temp2;
		for(int j = 0;j < all_text[i].size();j++) {
			if(word_map.find(all_text[i][j]) != word_map.end()) {
				temp.push_back(all_text[i][j]);
			}
		}
		sort(temp.begin(),temp.end());
		double count = 0.0;
		double tf_idf_value;
		for(int j = 0;j < temp.size();j++) {
			if(j == 0 || !temp[j].compare(temp[j-1])) {
				count++;
			} else {
				tf_idf_value = word_map[temp[j-1]].second*count/temp.size();
				temp2.push_back(make_pair(word_map[temp[j-1]].first,tf_idf_value));
				count = 1;
			}
			if(j == temp.size()-1) {
				tf_idf_value = word_map[temp[j]].second*count/temp.size();
				temp2.push_back(make_pair(word_map[temp[j]].first,tf_idf_value));
			}
		}
		// for(int i = 0;i < temp2.size();i++) {
		// 	cout << temp2[i].second << endl;
		// }
		tf_idf_result.push_back(temp2);
	}

}

//Build the stop_words vector by giving stop word file
void buildStopWord(unordered_set<string> &stop_words,string filename) {
	ifstream input(filename.c_str(),ios::in);
	if(!input.is_open()) {
		cout << "Error: Cannot Open " << filename << endl;
		exit(1);
	}
	string str;
	while(getline(input,str)) {
		stop_words.insert(str);
	}
	input.close();
}

//Write the final tf-idf doc-vector and split into train_part and test_part to two file
void outputResult(vector<vector<pair<int,double> > > &tf_idf_result,vector<string> &label,int split_index,string train_filename,string test_filename) {
	ofstream train_out(train_filename.c_str(),ios::out);
	if(!train_out.is_open()) {
		cout << "Error: Cannot Open " << train_filename << endl;
		exit(1);
	}
	ofstream test_out(test_filename.c_str(),ios::out);
	if(!test_out.is_open()) {
		cout << "Error: Cannot Open " << test_filename << endl;
		exit(1);
	}
	for(int i = 0;i < split_index;i++) {
		train_out << label[i] << " ";
		for(int j = 0;j < tf_idf_result[i].size();j++) {
			train_out << tf_idf_result[i][j].first << ":" << tf_idf_result[i][j].second << " ";
		}
		train_out << endl;
	}
	for(int i = split_index;i < tf_idf_result.size();i++) {
		test_out << label[i] << " ";
		for(int j = 0;j < tf_idf_result[i].size();j++) {
			test_out << tf_idf_result[i][j].first << ":" << tf_idf_result[i][j].second << " ";
		}
		test_out << endl;
	}
	test_out.close();
	train_out.close();
}

const char* const DICT_PATH = "dict/jieba.dict.utf8";
const char* const HMM_PATH = "dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "dict/user.dict.utf8";
const char* const IDF_PATH = "dict/idf.utf8";
const char* const STOP_WORD_PATH = "dict/stop_words.utf8";

int main(int argc, char** argv) {
	vector<vector<string> > train_text,test_text,all_texts;
	vector<string> dict;
	map<string,pair<int,double> > word_map;
	vector<vector<pair<int,double> > > tf_idf_result;
	vector<string> train_label,test_label,all_label;
	unordered_set<string> stop_words;
	double train_test_split = 0.8;
	cppjieba::Jieba jieba(DICT_PATH,
		HMM_PATH,
		USER_DICT_PATH,
		IDF_PATH,
		STOP_WORD_PATH);
	vector<string> words;
	cout << "Building Stop Word from stop.txt" << endl;
	buildStopWord(stop_words,"stop.txt");
	cout << "Parsing data form result.json" << endl;
	ifstream in("../jsoncpp/result.json",ios::in);
	if(!in.is_open()) {
		cout << "Cannot Open result.json" << endl;
		exit(1);
	}
	Json::Reader reader;
	Json::Value root;
	string content;
	string label;
	if(!reader.parse(in,root,false)) {
		cout << "Parse failed" << endl;
		exit(1);
	}
	vector<vector<string> > diff_category_content;
	vector<string> offical_content,article_content,resume_content,contract_content;
	for(int i = 0;i < root.size();i++) {
		content = root[i]["content"].asString();
		label = root[i]["category"].asString();
		if(!label.compare("公文")) {
			offical_content.push_back(content);
		} else if (!label.compare("论文")) {
			article_content.push_back(content);
		} else if (!label.compare("简历")) {
			resume_content.push_back(content);
		} else if (!label.compare("合同")) {
			contract_content.push_back(content);
		} else {
			cout << "Error: In " << i+1 << "term its category do not belong to 4 legal categorys" << endl;
			exit(1);
		}
	}
	diff_category_content.push_back(offical_content);
	diff_category_content.push_back(article_content);
	diff_category_content.push_back(resume_content);
	diff_category_content.push_back(contract_content);
	for(int i = 0;i < diff_category_content.size();i++) {
		int split_index = int(train_test_split*diff_category_content[i].size());
		for(int j = 0;j < split_index;j++) {
			//jieba.CutAll(diff_category_content[i][j],words);
			jieba.Cut(diff_category_content[i][j], words, true);
			train_text.push_back(words);
			train_label.push_back(to_string(i));
		}
		for(int j = split_index;j < diff_category_content[i].size();j++) {
			//jieba.CutAll(diff_category_content[i][j],words);
			jieba.Cut(diff_category_content[i][j], words, true);
			test_text.push_back(words);
			test_label.push_back(to_string(i));
		}
	}
	for(int i = 0;i < train_text.size();i++) {
		all_texts.push_back(train_text[i]);
		all_label.push_back(train_label[i]);
	}
	for(int i = 0;i < test_text.size();i++) {
		all_texts.push_back(test_text[i]);
		all_label.push_back(test_label[i]);
	}
	in.close();
	cout << "Building dictionary" << endl; 
	buildDict(all_texts,dict,stop_words);
	cout << "Building Word Map" << endl;
	buildWordMap(all_texts,dict,word_map);
	cout << "Building Final Reuslt" << endl;
	buildTfidf(all_texts,word_map,tf_idf_result);
	cout << "Writing Result to wps_train.txt and wps_test.txt" << endl;
	cout << "+++++" << dict.size() << endl;
	outputResult(tf_idf_result,all_label,train_text.size(),"wps_train.txt","wps_test.txt");
	return 0;
}

