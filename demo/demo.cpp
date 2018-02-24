#include "demo.h"
using namespace std;

//This function is to encrypt the model file.
//Encrypt is simply xor each char with the key,and the mode represent the model is SVM or fasttext.
//In the SVM, we must combine the idf result with the SVM model file and encrypt together.
void encrypt(const char * input_file_name,int mode,const char* model_file_name,const char* word_map_file) {
	char key[] = "ohmygod";
	FILE *fp = fopen(input_file_name,"r");
	if(fp == NULL) {
		printf("Cannot open %s\n",input_file_name);
		exit(1);
	}
	vector<char> myvector;
	char b;
	myvector.push_back(mode^key[3]);
	int index = 0;
	if(mode == 0) {
		FILE *w_fp = fopen(word_map_file,"r");
		if(w_fp == NULL) {
			printf("Unknown Error 0");
			exit(1);
		}
		while(true) {
			b = getc(w_fp);
			if(feof(w_fp))
				break;
			myvector.push_back(b^key[index%7]);
			index++;
		}
		fclose(w_fp);
	}
	while(true) {
		b = getc(fp);
		if(feof(fp))
			break;
		myvector.push_back(b^key[index%7]);
		index++;
	}
	fclose(fp);
	FILE *fp2 = fopen(model_file_name,"w");
	if(fp2 == NULL) {
		printf("Cannot open %s\n",model_file_name);
		exit(1);
	}
	for(auto &c:myvector) {
		fprintf(fp2, "%c", c);
	}
	fclose(fp2);
}


//This function is to cut the text into words by using jieba
void cutDocs(vector<string> & text,vector<vector<string> > &docs_words) {
	vector<string> words;
	char bar[100] = "Processing[-------------------------]";
	printf("%s\r",bar);
	fflush(stdout);
	double current_process;
	int bar_index = 11; 
	for(int i = 0;i < text.size();i++) {
		current_process = double(i)/(text.size()-1);
		while(current_process >= (bar_index-10)*0.04) {
			bar[bar_index] = '>';
			bar_index++;
		}
		if(i == text.size()-1) {
			printf("%s\n",bar);
		} else {
			printf("%s\r",bar);
		}
		fflush(stdout);
		jieba.CutAll(text[i],words);
		docs_words.push_back(words);
	}
}

void buildData(vector<string> &text,vector<string> &labels,vector<string> &titles,char *txtPath) {
	char bar[100] = "Processing[-------------------------]";
	printf("%s\r",bar);
	fflush(stdout);
	double current_process;
	int bar_index = 11;
    ofstream ofs(txtPath,ios::out);
    vector<string> words;
    for(int i = 0; i < text.size(); ++i) {
    	current_process = double(i)/(text.size()-1);
		while(current_process >= (bar_index-10)*0.04) {
			bar[bar_index] = '>';
			bar_index++;
		}
		if(i == text.size()-1) {
			printf("%s\n",bar);
		} else {
			printf("%s\r",bar);
		}
		fflush(stdout);
        string category = labels[i];
        string title = titles[i];
        string content = text[i];
        string titleContent = title + ":::" + content;
        for(int i = 0; i < titleContent.length(); ++i) {
            if(titleContent[i] == '\n')
                titleContent = titleContent.replace(i, 1, " ");
        }
        jieba.Cut(titleContent, words, true);
    	string cutResult = limonp::Join(words.begin(), words.end(), " ");
        string label = "    __label__" + category;
        string result = cutResult + label + "\n";
        ofs << result;       
    }
    ofs.close();
}

void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
         
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}


void output(char* output_file_name,vector<string>& predictions)
{
	int num = predictions.size();
	vector<string> hasSplited;
	for(int i = 0; i < num; ++i) {
		hasSplited.clear();
		SplitString(predictions[i],hasSplited,"__label__");
		predictions[i] = hasSplited[1];
	}
	//outfile all
	ofstream ofs_out(output_file_name,ios::out);
	for(int i = 0; i < predictions.size(); ++i) {
		ofs_out << predictions[i] << std::endl;   
	}
	ofs_out.close();
}

void saveWordMap(map<string,pair<int,double> > &word_map,char *tempfile) {
	map<string,pair<int,double> >::iterator it = word_map.begin();
	ofstream out(tempfile,ios::out);
	if(!out.is_open()) {
		cout << "Error: Cannot Open " << tempfile << endl;
		exit(1);
	}
	while(it != word_map.end()) {
		out << (it->first).c_str() << endl;
		out << (it->second).first  << endl;
		out << (float)(it->second).second << endl;
		it++;
	}
	out << "我是分割标识符" << endl;
	out.close();
}


void splitMapModel(char *tempfile,map<string,pair<int,double> > &word_map) {
	FILE *fp = fopen(tempfile,"r");
	if(fp == NULL) {
		fprintf(stderr, "Error: Cannot Open %s\n", tempfile);
		exit(1);
	}
	char word[10000];
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
	}
	vector<char> myvector;
	char b;
	while(true) {
		b = getc(fp);
		if(feof(fp))
			break;
		myvector.push_back(b);
	}
	fclose(fp);
	FILE *fp2 = fopen(tempfile,"w");
	for(auto &c:myvector) {
		fprintf(fp2, "%c", c);
	}
	fclose(fp2);
}

//This is the api to call SVM train part.
void train1(vector<string> &train_text,vector<string> & train_labels,char *model_file_name) {
	char *error;
	void *handle = dlopen("./libs/libdocvec.so",RTLD_NOW);
	void (*buildDict)(vector<vector<string> > &,vector<string> &,unordered_set<string> &);
	void (*buildWordMap)(vector<vector<string> > &,vector<string> &,map<string,pair<int,double> > &);
	void (*buildTfidf)(vector<vector<string> > &,map<string,pair<int,double> > &,vector<vector<pair<int,double> > > &);
	void (*buildStopWord)(unordered_set<string> &,string);
	void (*outputResult)(vector<vector<pair<int,double> > > &,vector<string> &,string);
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&buildDict) = dlsym(handle,"buildDict");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&buildWordMap) = dlsym(handle,"buildWordMap");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&buildTfidf) = dlsym(handle,"buildTfidf");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&buildStopWord) = dlsym(handle,"buildStopWord");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&outputResult) = dlsym(handle,"outputResult");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	vector<vector<string> > train_docs_words;
	vector<vector<pair<int,double> > > tf_idf_result;
	map<string,pair<int,double> > word_map;
	vector<string> dict;
	unordered_set<string> stop_words;
	cout << "Handling Train File" << endl;
	cutDocs(train_text,train_docs_words);
	cout << "Building Stop Word from stop.txt" << endl;
	buildStopWord(stop_words,"./data/stop.txt");
	cout << "Building dictionary" << endl; 
	buildDict(train_docs_words,dict,stop_words);
	cout << "Building Word Map" << endl;
	buildWordMap(train_docs_words,dict,word_map);
	
	cout << "Building Final Reuslt" << endl;
	buildTfidf(train_docs_words,word_map,tf_idf_result);
	cout << "Collecting result for train" << endl;
	char buf1[L_tmpnam];
	char buf2[L_tmpnam];
	char buf3[L_tmpnam];
	char * p_tf_1 = tmpnam(buf1); //For train doc-vec file
	char * p_tf_2 = tmpnam(buf2); //For unencrypt model-file
	char * p_tf_3 = tmpnam(buf3); //For unencrypt word_map_file
	outputResult(tf_idf_result,train_labels,p_tf_1);
	saveWordMap(word_map,p_tf_3);
	dlclose(handle);
	void *handle2 = dlopen("./libs/libtradition.so",RTLD_NOW);
	int (*traditionTrain)(char *,char *);
	if(!handle2) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&traditionTrain) = dlsym(handle2,"traditionTrain");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	cout << "Starting Training" << endl;
	int result = traditionTrain(p_tf_1,p_tf_2);
	if(result == 0) {
		fprintf(stderr, "%s\n", "Unknown error");
		exit(1);
	}
	encrypt(p_tf_2,0,model_file_name,p_tf_3);
	cout << "Finish train,please use the model file to predict" << endl;
}

//This is the api to call SVM predict part.
void predict1(vector<string> & test_text,vector<string> & test_labels,char *model_file_name,char *output_file_name,map<string,pair<int,double> > &word_map) {
	char *error;
	void *handle = dlopen("./libs/libdocvec.so",RTLD_NOW);
	void (*buildTfidf)(vector<vector<string> > &,map<string,pair<int,double> > &,vector<vector<pair<int,double> > > &);
	void (*outputResult)(vector<vector<pair<int,double> > > &,vector<string> &,string);
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&buildTfidf) = dlsym(handle,"buildTfidf");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&outputResult) = dlsym(handle,"outputResult");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	
	void *handle2 = dlopen("./libs/libtradition.so",RTLD_NOW);
	int (*traditionPredict)(char *,char *,char *);
	if(!handle2) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&traditionPredict) = dlsym(handle2,"traditionPredict");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	vector<vector<string> > test_docs_words;
	vector<vector<pair<int,double> > > tf_idf_result;
	char buf1[L_tmpnam];
	char * p_tf_1 = tmpnam(buf1); //For test doc-vec file
	cout << "Handling Test File" << endl;
	cutDocs(test_text,test_docs_words);
	cout << "Building Final Reuslt" << endl;
	buildTfidf(test_docs_words,word_map,tf_idf_result);
	cout << "Collecting result for train" << endl;
	outputResult(tf_idf_result,test_labels,p_tf_1);
	cout << "Starting Predicting" << endl;
	int result = traditionPredict(p_tf_1,model_file_name,output_file_name);
	if(result == 0) {
		fprintf(stderr, "%s\n", "Unknown error");
		exit(1);
	}
	dlclose(handle);
	dlclose(handle2);
	cout << "All Task finish,pleash check the result" << endl;
}

//This is the api to call fasttext predict part.
void predict2(vector<string> &test_text,vector<string> &test_labels,vector<string> &test_titles,string model,char *output_file_name) {
	vector<string> args;
	char *error;
	void *handle = dlopen("./libs/libneural.so",RTLD_NOW);
	void (*neuralPredict)(std::vector<std::string> &,std::vector<std::string> &,std::string,std::string);
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&neuralPredict) = dlsym(handle,"neuralPredict");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	char buf2[L_tmpnam];
	char * p_tf_2 = tmpnam(buf2); //For fasttext test file
	string test_file(p_tf_2);
	cout << "Handling Test File" << endl;
	buildData(test_text,test_labels,test_titles,p_tf_2);
	vector<string> result;
	cout << "Starting Predicting" << endl;
	neuralPredict(args,result,test_file,model);
	output(output_file_name,result);
	dlclose(handle);
	cout << "All Task finish,please check the result" << endl;
}

void predict(vector<string> & test_text,vector<string> & test_labels,vector<string> & test_titles,char *model_file_name,char* output_file_name) {
	char key[] = "ohmygod";
	FILE *fp = fopen(model_file_name,"r");
	if(fp == NULL) {
		fprintf(stderr, "Error: Cannot Open %s\n", model_file_name);
		exit(1);
	}
	vector<char> myvector;
	int mode = getc(fp);
	mode ^= key[3];
	char b;
	int index = 0;
	while(true) {
		b = getc(fp);
		if(feof(fp))
			break;
		myvector.push_back(b^key[index%7]);
		index++;
	}
	fclose(fp);
	char buf[L_tmpnam];
	char *p_tf = tmpnam(buf);
	FILE *fp2 = fopen(p_tf,"w");
	if(fp2 == NULL) {
		fprintf(stderr, "Unknown Error\n");
		exit(1);
	}
	for(auto &c:myvector) {
		fprintf(fp2, "%c", c);
	}
	fclose(fp2);
	if(mode == 0) {
		cout << "Using Tradition Mode to predict" << endl;
		map<string,pair<int,double> > word_map;
		splitMapModel(p_tf,word_map);
		predict1(test_text,test_labels,p_tf,output_file_name,word_map);
	} else {
		cout << "Using Neural Mode to predict" << endl;
		string s(p_tf);
		predict2(test_text,test_labels,test_titles,s,output_file_name);
	}
}

//This is the api to call fasttext train part.
void train2(vector<string> & train_text,vector<string> &train_labels,vector<string> &train_titles,char *model_file_name) {
	char *error;
	void *handle = dlopen("./libs/libneural.so",RTLD_NOW);
	void (*neuralTrain)(std::vector<std::string> &,std::string,std::string);
	void (*neuralPredict)(std::vector<std::string> &,std::vector<std::string> &,std::string,std::string);
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	*(void **)(&neuralTrain) = dlsym(handle,"neuralTrain");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	*(void **)(&neuralPredict) = dlsym(handle,"neuralPredict");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "%s\n", error);
		exit(1);
	}
	char buf1[L_tmpnam];
	char buf2[L_tmpnam];
	char * p_tf_1 = tmpnam(buf1); //For fasttext train file
	char * p_tf_2 = tmpnam(buf2); //For fasttext unencrypt model file
	string train_file(p_tf_1);
	string model(p_tf_2);
	vector<string> args;
	cout << "Handling Train File" << endl;
	buildData(train_text,train_labels,train_titles,p_tf_1);
	cout << "Starting Training" << endl;
	neuralTrain(args,train_file,model);
	encrypt(model.c_str(),1,model_file_name,NULL);
	cout << "Finish train,please use the model file to predict" << endl;
}