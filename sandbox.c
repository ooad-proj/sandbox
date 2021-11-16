#include <seccomp.h>
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
int result=0;
void play(char*filename,char**argv,char**env,FILE *config_file,char*line,int java){
struct rlimit max_memory,max_cpu_time;
   rewind(config_file);
   fgets(line,1000,config_file);
   long timelimit=atoi(fgets(line,1000,config_file));
   long spacelimit=-1;
   if(!java)
   spacelimit=atoi(fgets(line,1000,config_file))*(long)1024;;
   max_memory.rlim_cur = max_memory.rlim_max = (rlim_t) (spacelimit) * 2;
   max_cpu_time.rlim_cur = max_cpu_time.rlim_max = (rlim_t) ((timelimit + 1000) / 1000);
//  printf("\n%ld\n",max_cpu_time.rlim_cur);
 //  printf("%ld\n",max_memory.rlim_cur);
    if (setrlimit(RLIMIT_CPU, &max_cpu_time) != 0) {
          // printf("timeout");
   }
   if (setrlimit(RLIMIT_AS, &max_memory) != 0) {
       //   printf("spaceout");
    }
result=execve(filename, argv, env);
printf("error");
}

int main() {
   char line[100];
   FILE *input_file = NULL, *output_file = NULL,   
       *error_file=NULL,*config_file=NULL,*result_file=NULL;
   char *filename;
   char *argv[100];
   char *env[100];
   int result;
   config_file=fopen("details.txt", "r");
   input_file=fopen("in.txt","r");
   if(fgets(line, 10,input_file)!=NULL){
   input_file=freopen("in.txt", "r",stdin);
  }else fclose(input_file); 
   output_file=freopen("out.txt", "w",stdout);
   error_file=freopen("out.txt","w",stderr);
   result_file=fopen("results.txt","w");
int edit=0;
int java=0;
fgets(line,10,config_file);
long timelimit=atoi(fgets(line,100,config_file));
//printf("%s",line);
long memorylimit=atoi(fgets(line,100,config_file))*(long)1024;
//printf("memorylimit:%ld",memorylimit);
while(1){
   rewind(config_file);
   fgets(line,10,config_file);
   if(line[0]=='j'&&line[1]=='a'&&line[2]=='v'&&line[3]=='a'){
    if(!edit){
    char *filename1="/usr/bin/javac";
    char *argv1[] = {"javac","Main.java",NULL};
    char *env1[] = {NULL};
    java=1;
     filename=filename1;
    for (int i = 0; i < 3; i++){
    argv[i]=(char*)argv1[i];
    }
    for (int i = 0; i < 3; i++){
    env[i]=(char*)env1[i];
    }
}else{
    char *filename1="/usr/bin/java";
    atoi(fgets(line,1000,config_file));
    long space=atoi(fgets(line,1000,config_file));
    char buf2[30];
    sprintf(buf2,"-XX:MaxRAM=%ldk",space);
    char *argv1[] = {"java","Main",buf2,NULL};
    char *env1[] = {NULL};
    filename=filename1;
    for (int i = 0; i < 4; i++){
    argv[i]=(char*)argv1[i];
}
    for (int i = 0; i < 3; i++){
    env[i]=(char*)env1[i];
}
}
}else if(line[0]=='p'&&line[1]=='y'&&line[2]=='t'&&line[3]=='h'&&line[4]=='o'&&line[5]=='n'){
    if(!edit){
    char *filename1="/usr/bin/python2.7";
    char *argv1[] = {"python","-m","py_compile","Main.py",NULL};
    char *env1[] = {NULL};
     filename=filename1;
    for (int i = 0; i < 5; i++){
    argv[i]=(char*)argv1[i];
    }
    for (int i = 0; i < 3; i++){
    env[i]=(char*)env1[i];
    }
    }else{
    char *filename1="/usr/bin/python2.7";
    char *argv1[] = {"python","Main.pyc",NULL};
    char *env1[] = {NULL};
    filename=filename1;
    for (int i = 0; i < 3; i++){
    argv[i]=(char*)argv1[i];
    }
    for (int i = 0; i < 3; i++){
    env[i]=(char*)env1[i];
    }
}
}
   struct timeval start, end;
   gettimeofday(&start, NULL);
   pid_t child_pid=fork();
   //printf("%d",child_pid);
   if(child_pid==0){
    play(filename,argv,env,config_file,line,java);    
} 
    int status; 
    int memory;
    char lan[30];
    if(java){
    strcpy(lan,"Main.class");
}   else strcpy(lan,"Main.pyc");
    struct rusage resource_usage; 
     if (wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1){  
     kill(child_pid,SIGKILL);
}   
    memory = resource_usage.ru_maxrss * 1024; 
    gettimeofday(&end, NULL);
    int real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);
     if (WIFSIGNALED(status) != 0) {
            int signal = WTERMSIG(status);
            if(signal==SIGUSR1){
             result=-1;
            }
            if(signal!=0){
            result=-1;
            }
            if(signal==SIGSEGV){
            result=-1;
            }
            if(result==-1){
             fputs("-3",result_file); 
             break;  
            }
      }
   if(edit){
   remove(lan);
   error_file=fopen("out.txt","r");
    fputs("0\n",result_file);
   char buf3[30];
   sprintf(buf3,"%d\n",real_time);
   fputs(buf3,result_file);
   sprintf(buf3,"%d\n",memory);
   fputs(buf3,result_file);
    break;
   }
   if(memory>memorylimit){
      fputs("-2",result_file);
   }
   if(real_time>timelimit){
      fputs("-1",result_file);
   }
   edit=1;
   if(java&&access(lan,F_OK)==-1){
        fputs("-4",result_file);
        break;
   }
   }
   return 0;
}
