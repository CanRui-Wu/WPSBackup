#include "doc_vec.h"
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
	//dict.erase(dict.begin(),dict.begin()+5); //Delete the special string
}

//Build the word_map,mapping from string to <int,double>,the first value represent word_id,the second value represent word idf value 
void buildWordMap(vector<vector<string> > &all_text,vector<string> &dict,map<string,pair<int,double> > &word_map) {
	for(int i = 0;i < dict.size();i++) {
		word_map[dict[i]] = make_pair(i+1,0.0);
	}
	int *temp = new int[dict.size()+1];
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

//Write the train or test tf-idf doc-vector to file
void outputResult(vector<vector<pair<int,double> > > &tf_idf_result,vector<string> &label,string filename) {
	ofstream out(filename.c_str(),ios::out);
	if(!out.is_open()) {
		cout << "Error: Cannot Open " << filename << endl;
		exit(1);
	}
	for(int i = 0;i < label.size();i++) {
		out << label[i] << " ";
		for(int j = 0;j < tf_idf_result[i].size();j++) {
			out << tf_idf_result[i][j].first << ":" << tf_idf_result[i][j].second << " ";
		}
		out << endl;
	}
	out.close();
}