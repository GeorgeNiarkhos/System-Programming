#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <dirent.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H
#include "client_list.hpp"
#endif


using namespace std;

int getFileList(char * path, FileNameList& list, char* inputDir, char * path_to_add);

int writeFunc(char *path, int sock, int bsize);