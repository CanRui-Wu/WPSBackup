#ifndef SVM_TRAIN
#define SVM_TRAIN
#include "svm.h"
#include <iostream>
using namespace std;
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
static void parse_command_line(int argc, char **argv, char *input_file_name, char *model_file_name);
static void read_problem(const char *filename);
static void do_cross_validation();
static void exit_input_error(int line_num);
static void exit_with_help();
static void print_null();
extern "C" int traditionTrain(char *,char *);
static struct svm_parameter param;		// set by parse_command_line
static struct svm_problem prob;		// set by read_problem
static struct svm_model *model;
static struct svm_node *x_space;
static int cross_validation;
static int nr_fold;
static char *line = NULL;
static int max_line_len;
#endif
