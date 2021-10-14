/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include<linux/hash.h>
#include<linux/gcd.h>
#include<asm/param.h>
#include<linux/jiffies.h>
#include<linux/sched.h>
#include<linux/init_task.h>

#define MAXN 256
int simple_init(void){
struct stack{
	struct task_struct *data[MAXN];
	int head;
}st;

int st_size(void){return st.head;}
bool st_empty(void){return st.head==0;}
bool st_full(void){return st.head==MAXN;}

struct task_struct *st_pop(void){
	if(st_empty()) return NULL;
	st.head-=1;
	return st.data[st.head];
};
bool st_push(struct task_struct *t){
	if(st_full()) return false;
	st.data[st.head++]=t;
	return true;
}
st.head=0;
st_push(&init_task);
	   struct task_struct *tsk,*p;
	   struct list_head *list,*list2;
	   printk(KERN_INFO "Loading Module\n");
	   while(!st_empty()){
		   tsk=st_pop();
		   if((&tsk->children)->next!=(&tsk->children))
			   printk(KERN_INFO "The child processes of %s (with pid:%d) is:\n",tsk->comm,tsk->pid);
		   list_for_each_prev(list,&tsk->children){
			   p=list_entry(list,struct task_struct,sibling);
			   st_push(p);
			   }
		   list_for_each(list2,&tsk->children){
			   p=list_entry(list2,struct task_struct,sibling);
			   printk(KERN_INFO "name [%s] status [%d] pid [%d]\n",p->comm,p->state,p->pid);
			   }
	   }
       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

