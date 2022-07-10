#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>

typedef struct arg{
	char *ptr;
	int n;
	char *ptr_re;
}args;


void* thread(void* arg){
	args *s=(args*)arg;
	int i,j,n;
	for(i=0;i<s->n;i++)
		for(j=0;j<s->n-1;j++)
		{
			if(s->ptr[j]>s->ptr[j+1])
			{
				n=s->ptr[j];
				s->ptr[j]=s->ptr[j+1];
				s->ptr[j+1]=n;
			}
		}
	for(i=0;i<s->n;i++)
	s->ptr_re=s->ptr;
	pthread_exit(0);
}

int main()
{
	char b[]={7,12,19,3,18,4,2,6,15,8};
	int i,j;
	pthread_t tid[3];
	pthread_attr_t attr[3];
	args a[3];
	for(i=0;i<3;i++)
	{
		pthread_attr_init(&attr[i]);
	}
	a[0].ptr=b;
	a[1].ptr=b+5;
	a[2].ptr=b;
	a[0].n=5;
	a[1].n=5;
	a[2].n=10;
	a[0].ptr_re=(char *)malloc(sizeof(char)*5);
	a[1].ptr_re=(char *)malloc(sizeof(char)*5);
	a[2].ptr_re=(char *)malloc(sizeof(char)*10);
	pthread_create(tid,attr,thread,(void *)&a[0]);
	pthread_create(&tid[1],&attr[1],thread,(void *)&a[1]);
	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);
	pthread_create(&tid[2],&attr[2],thread,(void *)&a[2]);
	pthread_join(tid[2],NULL);
	for(i=0;i<=2;i++)
	{
		for(j=0;j<a[i].n;j++)
			{
				printf("%d ",a[i].ptr_re[j]);
			}
		printf("\n");
	}
	return 0;
}

