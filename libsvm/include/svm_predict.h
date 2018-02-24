#ifndef SVM_PREDICT
#define SVM_PREDICT
static int print_null(const char *s,...) {return 0;}

static int (*info)(const char *fmt,...) = &printf;

static char* readline(FILE *);
static void exit_input_error(int);
static void predict(FILE *, FILE *);
static void exit_with_help();
static void predict(FILE *, FILE *);
static struct svm_node *x;
static int max_nr_attr = 64;
static struct svm_model* model;
static int predict_probability=0;
static char *line = NULL;
static int max_line_len;
extern "C" int traditionPredict(char *,char *,char *);
#endif 