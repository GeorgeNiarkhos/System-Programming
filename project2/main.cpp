#include "functions.hpp"
#include "structs.hpp"

#define ERROR 10
#define EXIT 0
#define PERMS 0666

using namespace std;

int flag, var, bsize, alarmflagr/*, alarmflagw*/;


IdList ids;

void f1(int signum){
    flag = 0;
}

void f2(int sig, siginfo_t* info, void* vp){
    var = 0;
}

void ingore(int signum){
    //ignore   
}

void alarmfr(int signum){
    alarmflagr = 1;
}

int main(int argc, char *argv[]){
    //DEFINE ARGUMENTS
    if (argc%2 == 0){
        cerr << "Wrong argument input..." << endl;
        return ERROR;
    }
    char *n=NULL, *c=NULL, *in=NULL, *m=NULL, *l=NULL, *b=NULL;
    for (int i=1; i<argc; i++){
        if (!strcmp("-n", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (n != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;   
            }
            n=argv[i+1];
            i++;
        }
        else if (!strcmp("-c", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (c != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;   
            }
            c=argv[i+1];
            i++;
        }
        else if (!strcmp("-i", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (in != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
             in=argv[i+1];
             i++;
        }
        else if (!strcmp("-m", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (m != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            m=argv[i+1];
            i++;
        }
        else if (!strcmp("-l", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (l != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            l=argv[i+1];
            i++;
        }
        else if (!strcmp("-b", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (b != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            b=argv[i+1];
            i++;
        }
        else {
            cerr << "Wrong argument input..." << endl;
            return ERROR;
        }
    }
    if (n == NULL || c == NULL || in == NULL || m == NULL || l == NULL || b == NULL){
        cerr << "Wrong argument input..." << endl;
        return ERROR;
    }  
    //ASSIGN ARGS VALS
    int id = atoi(n);
    bsize = atoi(b);
    alarmflagr = 0;
    struct dirent *direntp;
    //CHECK DIRS
    DIR * input_dir = opendir(in);    
    if (input_dir == NULL){
        cerr << "Cannot open " << in << " directory" << endl;
        return ERROR;
    }
    closedir(input_dir);
    DIR * mirror_dir = opendir(m);
    if (mirror_dir != NULL){
        cerr << "Cannot continue, directory " << m << " already used" << endl;
        return ERROR;
    }
    else{
        mkdir(m, S_IRWXU | S_IRWXG | S_IROTH);
        mirror_dir = opendir(m);
    }
    closedir(mirror_dir);
    DIR * common_dir = opendir(c);
    if (common_dir == NULL){
        mkdir(c, S_IRWXU | S_IRWXG | S_IROTH);
        common_dir = opendir(c);
    }
    closedir(common_dir);
    //CREAT SELF ID FILE
    char * fname;
    fname = new char[strlen(c) + strlen(n) + 5];
    strcpy(fname, c);
    strcat(fname, "/");
    strcat(fname, n);
    strcat(fname, ".id");
    struct stat st;
    if(stat(fname,&st) == 0){
        delete[] fname;
        cerr << "File " << fname << "already exists!" << endl;
        return ERROR;
    }
    
    cout << "Hi " << id << endl;
    fstream fs;
    fs.open(fname, fstream::out | fstream::trunc);
    fs << getpid() ;
    fs.close();
    fs.open(l, fstream::out | fstream::trunc);
    fs << "START " << id << endl;
    fs.close();
    ids.addId(id);
    //SIG HANDLERS
    flag = 1;
    static struct sigaction act;
    act.sa_handler = f1;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    //
    static struct sigaction act1;
    act1.sa_handler = f1;
    sigemptyset(&act1.sa_mask);
    sigaction(SIGQUIT, &act1, NULL);
    //
    static struct sigaction act2;
    act2.sa_sigaction = f2;
    sigemptyset(&act2.sa_mask);
    sigaction(SIGUSR1, &act2, NULL);
    //WHILE CHECK COMMON DIR
    while(flag){
        int cnt = 0;
        ids.resetFlags();
        common_dir = opendir(c);
        while ( ( direntp=readdir(common_dir) ) != NULL ){            
            if ((strcmp(direntp->d_name, ".") != 0) && (strcmp(direntp->d_name, "..") != 0) ){
                char * token1, * token2;
                char delims[] = ".";
                char * temp = new char[strlen(direntp->d_name)+1];
                strcpy(temp, direntp->d_name);
                token1 = strtok(temp, delims);
                token2 = strtok(NULL, delims);
                if (!strcmp(token2, "id")){
                    //id file
                    ids.addId(atoi(token1));
                    cnt++;
                }
                delete[] temp;
            }
        }
        closedir(common_dir);
        int newId;
        while((newId = ids.findNewId()) != -1){
            cout << getpid() << " new id: " << newId << endl;
            char * str = new char[strlen(m) + numlen(newId) + 2];
            sprintf(str, "%s/%d", m, newId);
            mkdir(str, S_IRWXU | S_IRWXG | S_IROTH);
            delete[] str;
            for(int k=0; k<3; k++){
                pid_t pids[2];
                int j;
                pid_t pid;
                var = 1;
                for(int i=0; i<2; i++){
                    pid = fork();
                    j = i;
                    if (pid == 0) break;
                    pids[i] = pid;
                }
                    
                if(pid == 0){
                    if (j == 0){
                        //READER
                        int readfd, n;
                        char *buff = new char[bsize];
                        char *path = new char[strlen(c) + 15 + numlen(newId) + numlen(id)];
                        sprintf(path, "%s/id%d_to_id%d.fifo", c, newId, id);
                        //alarm
                        static struct sigaction alarmr;
                        alarmr.sa_handler = alarmfr;
                        sigemptyset(&alarmr.sa_mask);
                        sigaction(SIGALRM, &alarmr, NULL);
                        //open fifo
                        alarm(30);
                        while( (readfd = open(path, O_RDONLY)) < 0){
                            if(alarmflagr){
                                //cerr  CANT OPEN
                                unlink(path);
                                delete[] path;
                                kill(getppid(), SIGUSR1);
                                alarmflagr = 0;
                                exit(ERROR);
                            }
                        }
                        char * str1 = new char[strlen(m) + numlen(newId) + 2];
                        sprintf(str1, "%s/%d", m, newId);
                        if(readFunc(readfd, str1, l, bsize) == -1){
                            close(readfd);
                            unlink(path);
                            delete[] path;
                            delete[] str1;
                            alarmflagr = 0;
                            kill(getppid(), SIGUSR1);
                            exit(ERROR);
                        }
                        close(readfd);
                        unlink(path);
                        delete[] path;
                        delete[] str1;
                        alarmflagr = 0;
                        exit(EXIT);
                    }
                    else{
                        //WRITER
                        int writefd;
                        static struct sigaction ignoreSig;
                        ignoreSig.sa_handler = ingore;
                        sigaction(SIGPIPE, &ignoreSig, NULL);
                        //make fifo
                        char *path = new char[strlen(c) + 15 + numlen(newId) + numlen(id)];
                        sprintf(path, "%s/id%d_to_id%d.fifo", c, id, newId);
                        if( mkfifo(path,PERMS) < 0){
                            //send signal to father
                            //ERROR CANT CREATE
                            delete[] path;
                            kill(getppid(), SIGUSR1);
                            exit(ERROR);
                        }
                        if((writefd = open(path, O_WRONLY)) < 0){
                            //cerr  CANT OPEN
                            unlink(path);
                            delete[] path;
                            kill(getppid(), SIGUSR1);
                            cerr << "Error writer cant open fifo" << endl;
                            exit(ERROR);
                        }
                        if (writeFunc(writefd, in, NULL, NULL, l, bsize) == -1){
                            //ERROR IN WRITING
                            close(writefd);
                            unlink(path);
                            delete[] path;
                            kill(getppid(), SIGUSR1);
                            cerr << "Error writer in writing" << endl;
                            exit(ERROR);
                        }
                        //WRITE 00
                        short t = 0;
                        memcpy(path, &t, 2);
                        if (write(writefd, &t, 2) != 2) {
                            //ERROR
                            close(writefd);
                            unlink(path);
                            delete[] path;
                            kill(getppid(), SIGUSR1);
                            cerr << "Error writer in writing" << endl;
                            exit(ERROR);
                        }
                        //EXIT
                        close(writefd);
                        unlink(path);
                        delete[] path;
                        exit(EXIT);
                    }
                }
                else{
                    int status0, status1;
                    cout << "Parent waiting..." << endl;
                    int n1,n2;
                    
                    while( (n1=waitpid(pids[0], &status0,0)) <= 0){

                    }
                    while( (n2=waitpid(pids[1], &status1,0)) <= 0){

                    }
                    if (var){
                        if (WIFEXITED(status0) && WIFEXITED(status1)){
                            cout << "Done mirroring with id " << newId << endl;
                        }
                        break;
                    } 
                    cout << id << " client retrying..." << endl;
                }
            }
            
            
        }
        if(cnt < ids.size){
            //deleted id file
            int delId;
            while((delId = ids.findFFlag()) != -1){
                cout << "deleting..." << endl;
                char * str = new char[strlen(m) + numlen(delId) + 2];
                sprintf(str, "%s/%d", m, delId);
                pid_t pid = fork();
                if(pid == 0){
                    deleteDir(str);
                    exit(EXIT);
                }
                else{
                    int status;
                    waitpid(pid, &status,0);
                    cout << "Deleted " << delId << endl;
                }
                delete[] str;
            }
        }
        sleep(1);
    }

    //DELETE FILE AND FOLDER
    deleteDir(m);
    unlink(fname);
    cout << endl << "GoodBye" << endl;
    //FREE
    delete[] fname;
    fs.open(l, fstream::out | fstream::app);
    fs << "END" << endl; 
    fs.close();
    return EXIT;
}
