#include "functions.hpp"

int getFileList(char * path, FileNameList& list, char * inputDir, char * filePath){
    DIR *d = opendir(path);
    size_t pathLen = strlen(path);
    int fl = -2;
    char * path_to_add = NULL;
    if (d != NULL){
        struct dirent *dir;
        fl = 0;
        
        while ( (fl == 0) && (dir=readdir(d))){
            int fl2 = -1;
            char *str;
            size_t len;

            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")){   //skip . ..
                continue;
            }
            if(!strcmp(path, inputDir)){
                //if path is starting folder dont put in path
                path_to_add = new char[strlen(dir->d_name) + 1];
                sprintf(path_to_add, "%s", dir->d_name);
            }
            else{
                path_to_add = new char[strlen(filePath) + strlen(dir->d_name) + 2];
                sprintf(path_to_add, "%s/%s",filePath, dir->d_name);
            }
            len = pathLen + strlen(dir->d_name) + 2; 
            str = new char[len];
            sprintf(str, "%s/%s", path, dir->d_name);
            struct stat statbuf;
            if (!stat(str, &statbuf)){
                if (S_ISDIR(statbuf.st_mode)){  //another directory
                    fl2 = getFileList(str, list, inputDir, path_to_add);
                }
                else{
                    
                    fl2 = list.addFileNameNode(path_to_add, statbuf.st_ctime);  //add file
                }
            }
            delete[] str;
            delete[] path_to_add;
            fl = fl2;   //if error is encoutered break and return
        }
        closedir(d);
    }
    return fl;
}

int writeFunc(char *path, int sock, int bsize){
    char * buff;
    int n;
    
    FILE * ifile;
    ifile = fopen(path, "rb");
    if(ifile == NULL){
        cout << "error in open " << path << endl;
    }
    //SEND FILE SIZE
    long size;
    fseek (ifile, 0, SEEK_END);   // non-portable
    size=ftell (ifile);
    fclose (ifile);
    if(size == -1){
        cerr << "telg Error " << path << endl;
    }
    ifstream f(path, ifstream::binary);
    n = 4;
    if (write(sock, &size, sizeof(size)) <= 0) {
        //ERROR
        f.close();
        return -1;
    }
    //SEND FILE
    long bytecnt = 0;
    if(size == 0){
        //do nothing
        return 0;
    }
    if (size > bsize){
        //NOT ONE WRITE
        long tsize = size;
        while(tsize > bsize){
            buff = new char[bsize];
            f.read(buff,bsize);
            if (write(sock, buff, bsize) != bsize) {
                //ERROR
                delete[] buff;
                f.close();
                return -1;
            }
            bytecnt+=bsize;
            tsize-=bsize;
            delete[] buff;
        }
        //LAST WRITE
        buff = new char[bsize];
        f.read(buff,tsize);
        if (write(sock, buff, bsize) != bsize) {
            //ERROR
            delete[] buff;
            f.close();
            return -1;
        }
        bytecnt+=tsize;
        delete[] buff;
    }
    else{
        //ONE WRITE
        buff = new char[bsize];
        f.read(buff, size);
        if (write(sock, buff, bsize) != bsize) {
            //ERROR
            delete[] buff;
            f.close();
            return -1;
        }
        bytecnt += size;
        delete[] buff;
    }
    f.close();
                    
    return 0;
}