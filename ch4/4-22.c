#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
int count;
int main(int argc, char **argv){
	count=argc-1;
	int *arr=(int *)malloc(sizeof(int)*(count));
	if(arr==NULL){
		fprintf(stderr,"Malloc() failed\n");
		exit(1);
	}
	int i=0;
	for(;i<argc-1;i++){
		arr[i]=atoi(argv[i+1]);
	}
	int avg,max,min;
	pthread_t tid1,tid2,tid3;
	pthread_attr_t attr1,attr2,attr3;
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_init(&attr3);
	void *runner1(void *param){
		int *ptr=(int *)param;
		int sum=0;
		int i=0;
		for(;i<count;i++){
			sum+=ptr[i];
		}
		avg=sum/count;
		pthread_exit(0);
	}
	void *runner2(void *param){
		int *ptr=(int *)param;
		int temp,i=0;
		for(;i<count;i++){
			if(i==0) temp=ptr[i];
			else if(ptr[i]<temp) temp=ptr[i];
		}
		min=temp;
		pthread_exit(0);
	}
	void *runner3(void *param){
		int *ptr=(int *)param;
		int temp,i=0;
		for(;i<count;i++){
			if(i==0) temp=ptr[i];
			else if(ptr[i]>temp) temp=ptr[i];
		}
		max=temp;
		pthread_exit(0);
	}
    pthread_create(&tid1,&attr1,runner1,arr);
    pthread_create(&tid2,&attr2,runner2,arr);
    pthread_create(&tid3,&attr3,runner3,arr);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
	free(arr);
	printf("The average value is %d\nThe minimum value is %d\nThe maximum value is %d\n",avg,min,max);
	return 0;
}
