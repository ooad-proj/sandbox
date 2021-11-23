#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
int result=0;

struct retime {
    pid_t tid;
    long time;
};
void *thread_function(void* retime)
{
pid_t id=((struct retime *)retime)->tid;
 if (pthread_detach(pthread_self()) != 0) {
       kill(id,SIGKILL);
        return NULL;
  }
long time1=((struct retime *)retime)->time;
if (sleep((unsigned int)((time1 + 1000) / 1000)) != 0) {
        kill(id,SIGKILL);
        return NULL;
    }
 kill(id,SIGKILL);
  return NULL;
}


void play(char*filename,char**argv,char**env,FILE *config_file,char*line,int java){
struct rlimit max_memory,max_cpu_time,output;
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
output.rlim_cur = output.rlim_max =33554432;
   setrlimit(RLIMIT_FSIZE,&output);
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
   error_file=freopen("error.txt","w",stderr);
   FILE *readError_file=fopen("error.txt","r");
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
    char *filename1="/usr/bin/python3";
    char *argv1[] = {"python3","-m","py_compile","Main.py",NULL};
    char *env1[] = {NULL};
     filename=filename1;
    for (int i = 0; i < 5; i++){
    argv[i]=(char*)argv1[i];
    }
    for (int i = 0; i < 3; i++){
    env[i]=(char*)env1[i];
    }
    }else{
    char *filename1="/usr/bin/python3";
    char *argv1[] = {"python3","__pycache__/Main.cpython-38.pyc",NULL};
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
  // gettimeofday(&start, NULL);
   pid_t child_pid=fork();
   //printf("%d",child_pid);
   if(child_pid==0){
    play(filename,argv,env,config_file,line,java);    
}
struct retime time1;
time1.time=2*timelimit;
time1.tid=child_pid;
    pthread_t id = 0;

    int status_code = pthread_create(&id, NULL, thread_function,(void *)(&time1));
 if(status_code != 0)
 {
 kill(child_pid,SIGKILL);
  perror("pthread_create error");
 }
//kill(child_pid,SIGKILL);
    int status; 
    int memory;
    char lan[50];
    if(java){
    strcpy(lan,"Main.class");
}   else strcpy(lan,"__pycache__/Main.cpython-38.pyc");
    struct rusage resource_usage; 
     if (wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1){  
     kill(child_pid,SIGKILL);
}   
    memory = resource_usage.ru_maxrss * 1024; 
  //  gettimeofday(&end, NULL);
    //int real_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);
   int cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                       resource_usage.ru_utime.tv_usec / 1000);
 if(java&&access(lan,F_OK)==-1){
        fputs("-4",result_file);
        break;
   }
   if(memory>memorylimit){
      fputs("-2",result_file);
      break;
   }
   if(cpu_time>timelimit){
      fputs("-1",result_file);
      break;
   }
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
             fputs("-1",result_file); 
             break;  
            }
      }
char read_ret;
   if((read_ret=fgetc(readError_file))!=EOF){
fclose(output_file);
FILE *addOutput_file=fopen("out.txt", "a");
fputc(read_ret,addOutput_file);
     while(1){
	read_ret = fgetc(readError_file); 
        if(read_ret==EOF) 
        {
            break;
        }
        fputc(read_ret,addOutput_file);
}  
fputs("-3",result_file);
fclose(addOutput_file);
break;
}
   if(edit){
    fputs("0\n",result_file);
   char buf3[30];
   sprintf(buf3,"%d\n",cpu_time);
   fputs(buf3,result_file);
   sprintf(buf3,"%d\n",memory);
   fputs(buf3,result_file);
    break;
   }
   edit=1;
   }
   return 0;
}
