#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>

#define NUM 5//哲学家数量
enum {THINKING,HUNGRY,EATING}state[NUM];//每个哲学家的三种状态

pthread_cond_t self[NUM];// 条件变量使哲学家等待
pthread_mutex_t mutex[NUM]; // 互斥锁

void *philo(void *temp);//哲学家函数，以循环模拟哲学家在状态间切换
void pickup_forks(int id);//尝试拿起筷子
void return_forks(int id);//放下筷子并通知两边的哲学家可以拿起筷子
void test(int id);//查看是否能转换成EATING状态

int main(){
    //初始化
    for(int i = 0; i < NUM; i++){
        pthread_cond_init(&self[i], NULL);
        pthread_mutex_init(&mutex[i], NULL);
    }

    int id[NUM];// 给哲学家编号
    pthread_t tid[NUM];// 用于线程初始化
    pthread_attr_t attr[NUM];
    //线程初始化
    for(int j = 0; j < NUM; j++){
        id[j] = j;
        pthread_attr_init(&attr[j]);
        pthread_create(&tid[j], &attr[j], philo, &id[j]);
    }
    for(int k = 0; k < NUM; k++){
        pthread_join(tid[k], NULL);
    }
    return 0;
}


void *philo(void *temp){
    do{
    int id = *( (int *)temp);
    //尝试拿起筷子
    pickup_forks(id);
    //吃一段时间
    srand((unsigned)time(NULL));
    int sec = rand()%3+1;// 吃1-3秒
    sleep(sec);
    printf("The philosopher %d is eating for %d seconds.\n",id,sec);
    //放回筷子
    return_forks(id);
    //思考一段时间
    srand((unsigned)time(NULL));
    sec = rand()%3 +1;// 思考1-3秒
    sleep(sec);
    printf("The philosopher %d is thinking for %d seconds.\n",id,sec);
    }while(1);
    pthread_exit(NULL);
}

void pickup_forks(int i){
    state[i] = HUNGRY;// 切换到饥饿状态
    test(i);// 查看是否可以吃
    pthread_mutex_lock(&mutex[i]);
    //当没有进入EATING状态说明相邻哲学家在吃，开始等待
    while (state[i] != EATING){
        pthread_cond_wait(&self[i],&mutex[i]);
    }
    pthread_mutex_unlock(&mutex[i]);
}

void return_forks(int i){
    state[i] = THINKING;
    //告知相邻的哲学家我已经吃完了
    test((i+4)%5);
    test((i+1)%5);
}

void test(int i){
    // 哲学家只有在相邻哲学家都不在EATING状态才能转换成EATING
    if((state[(i+4)%5]!= EATING)&&(state[i]== HUNGRY) &&(state[(i+1)%5]!= EATING))
    {
        pthread_mutex_lock(&mutex[i]);
        state[i] = EATING;
        pthread_cond_signal(&self[i]);
        pthread_mutex_unlock(&mutex[i]);
    }
}