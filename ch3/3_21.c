#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>

int main(int argc, char **argv)
{
  if(argc>2){
    fprintf(stderr,"Args passed invaild!\n");
    return 1;
  }
  pid_t pid;
  pid =fork();
  if(pid<0){
    fprintf(stderr,"Fork Failed\n");
    return 1;
  }
  else if(pid == 0){
    /*child process*/
    int num = atoi(argv[1]);
    printf("%d,",num);
    while(num!=1){
      if(num%2==0){
        num=num/2;
        if(num==1)printf("%d\n",num);
	else printf("%d,",num);
      }
      else{
	num=3*num+1;
        printf("%d,",num);
      }
    }
  }
  else{
    /*Parent process*/
    wait(NULL);
    printf("Child Complete\n");
  }
  return 0;
}
