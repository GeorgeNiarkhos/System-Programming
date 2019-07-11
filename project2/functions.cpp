#include "functions.hpp"

using namespace std;

int deleteDir(char * path){
    DIR *d = opendir(path);
    size_t pathLen = strlen(path);
    int fl = -1;

    if (d){
        struct dirent *dir;
        fl = 0;
        
        while ( (fl == 0) && (dir=readdir(d))){
            int fl2 = -1;
            char *str;
            size_t len;

            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")){   //skip . ..
                continue;
            }

            len = pathLen + strlen(dir->d_name) + 2; 
            str = new char[len];
            sprintf(str, "%s/%s", path, dir->d_name);
            struct stat statbuf;
            if (!stat(str, &statbuf)){
                if (S_ISDIR(statbuf.st_mode)){  //another directory
                    fl2 = deleteDir(str);
                }
                else{
                    fl2 = unlink(str);  //delete file
                }
            }
            delete[] str;
            fl = fl2;   //if error is encoutered break and return
        }
        closedir(d);
    }

    if (fl == 0){   //empty folder
        fl = rmdir(path);
    }
    return fl;
}

int numlen(int num){
    int cnt = 1;
    while(num=num/10){
        cnt++;
    }
    return cnt;
}

int writeFunc(int writefd, char *input_dir, char *path, char* pathS, char* logF, int bsize){
    //GO THROUGH INPUT FOLDER
    int mode = 0;
    if (path == NULL){
        path = new char[strlen(input_dir)+1];
        strcpy(path, input_dir);
        mode = 1;
    }
    int pathLen = strlen(path);
    DIR *directory = opendir(path);
    struct dirent *direntp;
    int cnt = 0;
    ofstream fout;
    while ( ( direntp=readdir(directory) ) != NULL ){                            
        if ((strcmp(direntp->d_name, ".") != 0) && (strcmp(direntp->d_name, "..") != 0) ){
            cnt++;
            
            short len = pathLen + strlen(direntp->d_name) + 2; 
            short lenS; 
            char * str = new char[len];
            char * strS;
            sprintf(str, "%s/%s", path, direntp->d_name);   //path to open
            if(mode == 1){
                //AT INPUT DIR
                lenS = strlen(direntp->d_name) + 1;
                strS = new char[lenS];
                sprintf(strS, "%s", direntp->d_name);   //path to send
            }
            else{
                lenS = strlen(pathS) + strlen(direntp->d_name) + 2;
                strS = new char[lenS];
                sprintf(strS, "%s/%s", pathS, direntp->d_name);
            }
            
            
            struct stat statbuf;
            if (!stat(str, &statbuf)){
                if (S_ISDIR(statbuf.st_mode)){
                    //IF DIR
                    if( writeFunc(writefd, input_dir, str, strS, logF, bsize) == -1){
                        //ERROR
                        closedir(directory);
                        delete[] str;
                        delete[] strS;
                        if(mode){
                            delete[] path;
                        }
                        cerr << "ERROR 1" << endl;
                        return -1;
                    }   
                }
                else{
                    //IF FILE
                    char * buff;
                    int n;
                    ifstream f(str);
                    //SEND PATH SIZE
                    n = 2;
                    if (write(writefd, &lenS, n) != n) {
                        //ERROR
                        closedir(directory);
                        delete[] str;
                        delete[] strS;
                        if(mode){
                            delete[] path;
                        }
                        f.close();
                        cerr << "ERROR 2" << endl;
                        return -1;
                    }
                    //SEND PATH
                    if (write(writefd, strS, lenS) != lenS) {
                        //ERROR
                        closedir(directory);
                        delete[] str;
                        delete[] strS;
                        if(mode){
                            delete[] path;
                        }
                        f.close();
                        cerr << "ERROR 5" << endl;
                        return -1;
                    }
                    //SEND FILE SIZE
                    f.seekg (0,f.end);
                    uint size = f.tellg();
                    f.seekg (0);
                    n = 4;
                    if (write(writefd, &size, n) != n) {
                        //ERROR
                        closedir(directory);
                        delete[] str;
                        delete[] strS;
                        if(mode){
                            delete[] path;
                        }
                        f.close();
                        cerr << "ERROR 6" << endl;
                        return -1;
                    }
                    //SEND FILE
                    int bytecnt = 0;
                    
                    if (size > bsize){
                        //NOT ONE WRITE
                        uint tsize = size;
                        while(tsize > bsize){
                            buff = new char[bsize];
                            f.read(buff,bsize);
                            if (write(writefd, buff, bsize) != bsize) {
                                //ERROR
                                //WRITE TO LOG
                                fout.open(logF, ofstream::out | ofstream::app);
                                char *buff1 = new char[numlen(bytecnt) + 8];
                                sprintf(buff1, "WROTE %d\n", bytecnt);
                                fout.write(buff1, strlen(buff1));
                                fout.close();
                                delete[] buff1;
                                //CLEAR DATA
                                closedir(directory);
                                delete[] str;
                                delete[] strS;
                                delete[] buff;
                                if(mode){
                                    delete[] path;
                                }
                                f.close();
                                cerr << "ERROR 7" << endl;
                                return -1;
                            }
                            bytecnt+=bsize;
                            tsize-=bsize;
                            delete[] buff;
                        }
                        //LAST WRITE
                        buff = new char[tsize];
                        f.read(buff,tsize);
                        if (write(writefd, buff, tsize) != tsize) {
                            //ERROR
                            //WRITE TO LOG
                            fout.open(logF, ofstream::out | ofstream::app);
                            char *buff1 = new char[numlen(bytecnt) + 8];
                            sprintf(buff1, "WROTE %d\n", bytecnt);
                            fout.write(buff1, strlen(buff1));
                            fout.close();
                            delete[] buff1;
                            closedir(directory);
                            delete[] str;
                            delete[] strS;
                            delete[] buff;
                            if(mode){
                                delete[] path;
                            }
                            f.close();
                            cerr << "ERROR 8" << endl;
                            return -1;
                        }
                        bytecnt+=tsize;
                        delete[] buff;
                    }
                    else{
                        //ONE WRITE
                        buff = new char[size];
                        f.read(buff, size);
                        if (write(writefd, buff, size) != size) {
                            //ERROR
                            //WRITE TO LOG
                            fout.open(logF, ofstream::out | ofstream::app);
                            char *buff1 = new char[numlen(bytecnt) + 8];
                            sprintf(buff1, "WROTE %d\n", bytecnt);
                            fout.write(buff1, strlen(buff1));
                            fout.close();
                            delete[] buff1;
                            closedir(directory);
                            delete[] str;
                            delete[] strS;
                            delete[] buff;
                            if(mode){
                                delete[] path;
                            }
                            f.close();
                            cerr << "ERROR 9" << endl;
                            return -1;
                        }
                        bytecnt += size;
                        delete[] buff;
                    }
                    f.close();
                    //WRITE TO LOG
                    fout.open(logF, ofstream::out | ofstream::app);
                    buff = new char[numlen(bytecnt) + 8];
                    sprintf(buff, "WROTE %d\n", bytecnt);
                    fout.write(buff, strlen(buff));
                    fout.close();
                    //WRITE TO LOG
                    fout.open(logF, ofstream::out | ofstream::app);
                    char *buff1 = new char[14];
                    sprintf(buff1, "FILE WRITTEN\n");
                    fout.write(buff1, strlen(buff1));
                    fout.close();
                    delete[] buff1;
                    delete[] buff;
                }
            }
            else{
                //ERROR
                closedir(directory);
                delete[] str;
                delete[] strS;
                if(mode){
                    delete[] path;
                }
                cerr << "ERROR 10" << endl;
                return -1;
            }
            delete[] str;
            delete[] strS;
            
            
            
        }
    }
    //IF EMPTY FOLDER
    if (!cnt){
        //SEND ONLY DIR NAME
        short len = strlen(pathS) + 2;
        char * str = new char[len];
        sprintf(str, "%s/", pathS);
        //WRITE PATH LEN
        int n = 2;
        if (write(writefd, &len, n) != n) {
            //ERROR
            closedir(directory);
            delete[] str;
            if(mode){
                delete[] path;
            }
            cerr << "ERROR 90" << endl;
            return -1;
        }
        //WRITE PATH
        if (write(writefd, str, len) != len) {
            //ERROR
            closedir(directory);
            delete[] str;
            if(mode){
                delete[] path;
            }
            cerr << "ERROR 90" << endl;
            return -1;
        }
        delete[] str;
    }
    closedir(directory);
    if(mode){
        delete[] path;
    }
    return 0;
}

int readFunc(int readfd, char *mirror_dir, char *logF, int bsize){
    int mode=0, rc, n;
    char * path = NULL;
    short pathlen;
    uint filelen, remaining;
    ofstream fout, flog;
    struct pollfd fdarray[1];
    int bytecnt = 0;
    while(1){
        fdarray[0].fd = readfd;
        fdarray[0].events = POLLIN;
        rc = poll(fdarray, 1, 30000);
        if (rc == 0){
            //POLL TIMED OUT
            cerr << "Timeout in reader of " << getppid() << endl;
            return -1;
        }
        else if (rc == 1){
            if (mode == 0){ //GET PATH LEN
                if ((n = read(readfd, &pathlen, 2)) != 2){
                    //ERROR
                    cerr << "Error in read in reader of " << getppid() << endl;
                    return -1;
                }
                if (pathlen == 0){
                    //FINISHED
                    return 0;
                }
                mode = 1;    
            }
            else if(mode == 1){ //GET PATH
                path = new char[pathlen];
                if ((n = read(readfd, path, pathlen)) != pathlen){
                    //ERROR
                    if(path != NULL){
                        delete[] path;
                    }
                    cerr << "Error in read in reader of " << getppid() << endl;
                    return -1;
                }
                
                //DETERMINE DIRS
                char * temp = new char[strlen(path)+1];
                strcpy(temp, path);
                char * token1, *token2;
                //IF LAST CHAR IS /
                int caseD = 0;
                if(path[strlen(path)-1] == '/'){
                    //EMPTY DIR MODE
                    caseD = 1;
                }
                else{
                    caseD = 0;
                }
                token1 = strtok(temp, "/");
                token2 = strtok(NULL, "/");
                if(token2 == NULL){
                    if(caseD == 0){
                        //JUST FILE
                        //OPEN-CREATE FILE STREAM
                        char * inpath = new char[strlen(mirror_dir) + strlen(path) + 2];
                        sprintf(inpath, "%s/%s", mirror_dir, path);
                        fout.open(inpath, ofstream::out);
                        delete[] inpath;
                        mode = 2;
                    }
                    else{
                        //JUST DIR
                        char * inpath = new char[strlen(mirror_dir) + strlen(path) + 2];
                        sprintf(inpath, "%s/%s", mirror_dir, path);
                        mkdir(inpath, S_IRWXU | S_IRWXG | S_IROTH);
                        delete[] inpath;
                        //NO NEED TO READ FOR FILE
                        if(path != NULL){
                            delete[] path;
                        }
                        path = NULL;
                        mode = 0;
                    }
                }
                else{
                    //AT LEAST ONE DIR
                    char * inpath1, * inpath2;
                    //MAKE FIRST DIR
                    inpath1 = new char[strlen(mirror_dir) + strlen(token1) + 2];
                    sprintf(inpath1, "%s/%s", mirror_dir, token1);
                    mkdir(inpath1, S_IRWXU | S_IRWXG | S_IROTH);
                    token1 = token2;
                    token2 = strtok(NULL, "/");
                    while(token2 != NULL){
                        //MORE DIRECTORIES                       
                        inpath2 = new char[strlen(inpath1) + strlen(token1) + 2];
                        sprintf(inpath2, "%s/%s", inpath1, token1);
                        mkdir(inpath2, S_IRWXU | S_IRWXG | S_IROTH);
                        delete[] inpath1;
                        inpath1 = new char[strlen(inpath2) + 1];
                        strcpy(inpath1, inpath2);
                        delete[] inpath2;
                        token1 = token2;
                        token2 = strtok(NULL, "/");
                    }
                    if(caseD == 0){
                        //OPEN FILE
                        inpath2 = new char[strlen(inpath1) + strlen(token1) + 2];
                        sprintf(inpath2, "%s/%s", inpath1, token1);
                        fout.open(inpath2, ofstream::out);
                        delete[] inpath1;
                        delete[] inpath2;
                        mode = 2;
                    }
                    else{
                        //EMPTY DIR CASE
                        inpath2 = new char[strlen(inpath1) + strlen(token1) + 3];
                        sprintf(inpath2, "%s/%s", inpath1, token1);
                        mkdir(inpath2, S_IRWXU | S_IRWXG | S_IROTH);
                        delete[] inpath1;
                        delete[] inpath2;
                        //GO TO START
                        mode = 0;
                        //FREE DATA
                        if(path != NULL){
                            delete[] path;
                            path=NULL;
                        }
                    }
                }
                delete[] temp;
            }
            else if(mode == 2){ //GET FILE LEN
                //PATH READ
                //FILE ALREADY OPENED
                char * buff = new char[4];
                if ((n = read(readfd, &filelen, 4)) != 4){
                    //ERROR
                    delete[] buff;
                    fout.close();
                    if(path != NULL){
                        delete[] path;
                    }
                    cerr << "Error in read in reader of " << getppid() << endl;
                    return -1;
                }

                remaining=filelen;
                bytecnt=0;
                delete[] buff;
                mode = 3;
            }
            else{   //GET FILE
                //PATH READ
                //FILE ALREADY OPENED
                //FILE LEN AQUIRED AND TEMPCNT

                
                if(remaining <= bsize){   //IF REMAINING FIT TO BUFF 
                    char * buff = new char[remaining];
                    if ((n = read(readfd, buff, remaining)) != remaining){
                        //ERROR
                        //WRITE TO LOG
                        flog.open(logF, ofstream::out | ofstream::app);
                        char *buff1 = new char[numlen(bytecnt) + 7];
                        sprintf(buff1, "READ %d\n", bytecnt);
                        flog.write(buff1, strlen(buff1));
                        flog.close();
                        delete[] buff1;
                        delete[] buff;
                        if(path != NULL){
                            delete[] path;
                        }
                        fout.close();
                        cerr << "Error in read in reader of " << getppid() << endl;
                        return -1;
                    }
                    bytecnt+=remaining;
                    //WRITE TO FILE
                    fout.write(buff, remaining);
                    //WRITE TO LOG
                    flog.open(logF, ofstream::out | ofstream::app);
                    char *buff1 = new char[numlen(bytecnt) + 7];
                    sprintf(buff1, "READ %d\n", bytecnt);
                    flog.write(buff1, strlen(buff1));
                    flog.close();
                    delete[] buff1;
                    //WRITE TO LOG
                    flog.open(logF, ofstream::out | ofstream::app);
                    buff1 = new char[11];
                    sprintf(buff1, "FILE READ\n");
                    flog.write(buff1, strlen(buff1));
                    flog.close();
                    delete[] buff1;
                    //FREE DATA FOR NEXT FILE
                    if(path != NULL){
                        delete[] path;
                        path = NULL;
                    }
                    fout.close();
                    delete[] buff;
                    mode = 0;
                }
                else{
                    char * buff = new char[bsize];
                    if ((n = read(readfd, buff, bsize)) != bsize){
                        //ERROR
                        //WRITE TO LOG
                        flog.open(logF, ofstream::out | ofstream::app);
                        char *buff1 = new char[numlen(bytecnt) + 7];
                        sprintf(buff1, "READ %d\n", bytecnt);
                        flog.write(buff1, strlen(buff1));
                        flog.close();
                        delete[] buff1;
                        delete[] buff;
                        if(path != NULL){
                            delete[] path;
                        }
                        fout.close();
                        cerr << "Error in read in reader of " << getppid() << endl;
                        return -1;
                    }
                    bytecnt+=bsize;
                    //WRITE TO FILE
                    fout.write(buff, bsize);
                    //UPDATE REMAINING BYTES TO WRITE
                    remaining-=bsize;
                    delete[] buff;
                    mode = 3;
                }
                
            }
        }    
    }

}