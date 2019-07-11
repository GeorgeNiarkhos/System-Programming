#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <signal.h>
#include <fstream>
#include <errno.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>

using namespace std;

int getFileList(char * path);

int numlen(int num);

int writeFunc(int writefd, char *input_dir, char *path, char* pathS, char* logF, int bsize);

int readFunc(int readfd, char *mirror_dir, char *logF, int bsize);

int deleteDir(char * path);