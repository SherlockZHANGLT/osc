#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <semaphore.h>

#define gettid() syscall(SYS_gettid)//获得线程号
#define NUM_THREADS 3//线程数
#define BUFFER_SIZE 20//缓冲区slot数

//定义缓冲区
typedef struct buffer_struct{
    int rear;//尾指针，用于写入
    int front;//头指针，用于读出
    int buffer[BUFFER_SIZE];//长度20的int数组
}buffer_struct;

//定义锁
sem_t *full;
sem_t *empty;
sem_t *mutex;
int pipef;//定义管道
void *ptr;//创建空指针，用于访问共享内存

double sleep_time(double lambda_c);//返回一个符合负指数分布的随机变量
void *client(void *temp); //生产者线程
void *pipe_func(void *temp);//管道线程

int main(int argc, char *argv[])
{
    //声明缓冲区对象并清零
    buffer_struct bf;
    memset(&bf, 0, sizeof(buffer_struct));

    
    if (argc != 2){
        printf("The number of supplied arguments are false.\n");
        return -1;
    }
    if (atof(argv[1]) < 0){
        printf("The lambda entered should be greater than 0.\n");
        return -1;
    }
    double lambda_c = atof(argv[1]);//读取lambda p转化为数字
    
    //打开具名信号量并初始化 
    full = sem_open("cfull", O_CREAT, 0666, 0);
    empty = sem_open("cempty", O_CREAT, 0666, 0);
    mutex = sem_open("cmutex", O_CREAT, 0666, 0);
    sem_init(full, 1, 0);//初始时有0个满的slot
    sem_init(empty, 1, BUFFER_SIZE);//20个空位对应20个slot
    sem_init(mutex, 1, 1);//读写锁，一次只能有一个线程读写

    //创建共享内存
    int shm_fd = shm_open("client_buf", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(buffer_struct));
    ptr = mmap(0, sizeof(buffer_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //创建3个服务器和一个管道
    pthread_t tid[NUM_THREADS];
    pthread_t tid0;
    pthread_attr_t attr[NUM_THREADS];
    pthread_attr_t attr0;
    for (int i = 0; i < NUM_THREADS; i++){
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], client, &lambda_c);//生产者线程
    }
    pthread_attr_init(&attr0);
    pthread_create(&tid0, &attr0, pipe_func, &lambda_c);//管道线程
    for (int j = 0; j < NUM_THREADS; j++){
        pthread_join(tid[j], NULL);
    }
    pthread_join(tid0, NULL);

    return 0;
}

//符合负指数分布的时间函数
double sleep_time(double lambda_c){
    double r;
    r = ((double)rand() / RAND_MAX);
    while(r == 0 || r == 1){
        r = ((double)rand() / RAND_MAX);
    }
    r = (-1 / lambda_c) * log(1-r);
    return r;
}

//生产者函数
void *client(void *temp){
    double lambda_c = *(double *)temp;
    do{
        double interval_time = lambda_c;
        unsigned int sleepTime = (unsigned int)sleep_time(interval_time);
        sleep(sleepTime);//生产数据的间隔
        int item = rand()%999+1;//生产一个1-999之间的数
        buffer_struct *shm_ptr = ((buffer_struct *)ptr);//指向共享的缓冲区struct
        sem_wait(empty);//empty--
        sem_wait(mutex);//mutex--
        printf("Sleep Time: %d s | Producing the data %d to buffer[%d] by thread %ld in process %d.\n", sleepTime, item,shm_ptr->rear,gettid(), getpid());
        shm_ptr->buffer[shm_ptr->rear] = item;//向缓冲区输入item
        shm_ptr->rear = (shm_ptr->rear+1) % BUFFER_SIZE;//尾指针后移
        sem_post(mutex);//mutex++
        sem_post(full);//full++
    }while(1);
    pthread_exit(0);
}

void *pipe_func(void *temp)
{
    int *rec = (int *)malloc(sizeof(int));
    pipef = open("./pipe_func", O_WRONLY);//以只读的方式打开管道文件
    if(pipef < 0)
	{
		printf("open pipef error is %s\n", strerror(errno));
		*rec = -1;
		pthread_exit((void *)rec);
		return NULL;
	}
    do{
        sem_wait(full);//full--
        sem_wait(mutex);//mutex--
        buffer_struct *shm_ptr = ((buffer_struct *)ptr);//指向共享的缓冲区struct
        int item  = shm_ptr->buffer[shm_ptr->front];//从头指针读出
        char str[5];
        sprintf(str+strlen(str), "%d", item);//将item转化为string形式
        write(pipef, str, sizeof(str));//向管道写入item的string
        shm_ptr->front = (shm_ptr->front+1) % BUFFER_SIZE;//头指针后移
        memset(str, 0, sizeof(str));//string空间清零
        sem_post(mutex);//mutex++
        sem_post(empty);//empty++
    }while(1);
    pthread_exit(0);
}
