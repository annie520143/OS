/*
Student No.: 0616308
Student Name: Chia Shin Ho
Email: annie520143@gmail.com
SE tag: xnxcxtxuxoxs
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdbool.h>
#include<semaphore.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>

int n;
int *a, *b;
int st[15], end[15];
sem_t sem_divide[14];
sem_t sem_merge[14];
sem_t sem_main;
sem_t sem;
pthread_t *tid;

void read_input();
void *merge_sort(void* _args);
void *merge_sort_master();
void *bubble_sort(void* _args);
void divide(int i);
void merge(int i);
void merge_sort_ST(int first, int last, int tier);
void bubble_sort_ST(int first, int last);

int main()
{

	read_input();

	//set semaphore's to 0
	for(int i=0;i<14;i++){
		sem_init(&sem_merge[i], 0, 0);
		sem_init(&sem_divide[i], 0, 0);
	}
	sem_init(&sem_main,0,0);
	sem_init(&sem, 0, 0);

	st[0] = 0;
	end[0] = n-1;

	int sec, usec;
	float time;
	struct timeval start_time, end_time;

	//gettimeofday(&start_time, 0);

	//create 15 threads
	tid = malloc(15 * sizeof(pthread_t));
	gettimeofday(&start_time, 0);
	pthread_create(&tid[0], NULL, merge_sort_master, NULL);
	for(int i=1;i<7;i++) pthread_create(&tid[i], NULL, merge_sort, (void*)i);
	for(int i=7;i<15;i++) pthread_create(&tid[i], NULL, bubble_sort, (void*)i);

	sem_post(&sem);
	sem_wait(&sem_main);

	gettimeofday(&end_time, 0);

	//get MT time in ms
	sec = end_time.tv_sec - start_time.tv_sec;
	usec = end_time.tv_usec - start_time.tv_usec;
	time = sec * 1000000 + usec;
	printf("MT sorting time: %f ms", time);
	printf("/n");

	gettimeofday(&start_time, 0);

	merge_sort_ST(0,n-1,1);

	gettimeofday(&end_time, 0);

	//get ST time in ms
	sec = end_time.tv_sec - start_time.tv_sec;
	usec = end_time.tv_usec - start_time.tv_usec;
	time = sec * 1000000 + usec;
	printf("ST sorting time: %f ms\n", time);

	free(tid);
	return 0;

}

void read_input()
{
	char in[1000];
	//scanf("%s", in);
	//char* in = "input.txt";

	fgets(in, 1000, stdin);
	for(int i=0;i<1000;i++){
		if(in[i] == '\n'){
			in[i] = '\0';
			break;
		}
	}

	FILE *fin = fopen(in,"r");
	fscanf(fin, "%d", &n);
	//printf("%d", n);
	a = malloc(n * sizeof(int));
	b = malloc(n * sizeof(int));
	int i = 0, tmp;

	for(int i=0;i<n;i++){
		fscanf(fin, "%d", a+i);
		b[i] = a[i];
		//printf("%d\n", b[i]);
	}

	fclose(fin);

}



void *merge_sort_master()
{
	sem_wait(&sem);

	divide(0);

	sem_post(&sem_divide[0]);
	sem_post(&sem_divide[1]);

	sem_wait(&sem_merge[0]);
	sem_wait(&sem_merge[1]);

	merge(0);

	/*for(int i=0;i<n;i++)
		printf("%d ",a[i]);
	}*/

	//create output file
	FILE *fout =  fopen("output1.txt", "w");
	for(int i=0;i<n;i++) fprintf(fout, "%d ", a[i]);

	fclose(fout);

	sem_post(&sem_main);
	pthread_exit(NULL);
}

void *merge_sort(void *_args)
{

	int i = (int) _args;

	int prev_sem = i-1;
	int next_sem1 = 2*i;
	int next_sem2 = 2*i+1;
	int mid;
	//printf("%d ", i);

	//wait upper level divide
	sem_wait(&sem_divide[prev_sem]);

	divide(i);

	//signal lower level divide
	sem_post(&sem_divide[next_sem1]);
	sem_post(&sem_divide[next_sem2]);

	//wait lower level merge
	sem_wait(&sem_merge[next_sem1]);
	sem_wait(&sem_merge[next_sem2]);

	merge(i);

	//signal upper level merge
	sem_post(&sem_merge[prev_sem]);

	pthread_exit(NULL);

}

void *bubble_sort(void* _args)
{

	int cur_index = (int) _args;

	sem_wait(&sem_divide[cur_index-1]);

	int first = st[cur_index];
	int last = end[cur_index];

	int tmp;
    	for(int i=last;i>first;i--){
        	for(int j=first;j<i;j++){
            	if(a[j] > a[j+1]) {
               		tmp = a[j];
               		a[j] = a[j+1];
               		a[j+1] = tmp;
            		}
    		}
    	}

	sem_post(&sem_merge[cur_index-1]);
	pthread_exit(NULL);
}

void divide(int i)
{
	int first = st[i], last = end[i];
	st[2*i+1] = first, end[2*i+1] = (first+last)/2;
	st[2*i+2] = (first+last)/2 + 1, end[2*i+2] = last;
}

void merge(int i)
{
	int first = st[i], last = end[i];

	int len = last - first + 1;
	int len1 = end[2*i+1] - st[2*i+1] + 1;
	int len2 = end[2*i+2] - st[2*i+2] + 1;
	int cur=0, index1 = st[2*i+1], index2 = st[2*i+2];
	int tmp[len];

	while(cur <= len){

		//arr1 finish merging
		if(index1 == first + len1) tmp[cur++] = a[index2++];

		//arr2 finish merging
		else if(index2 == (first+last)/2+1 + len2)
			tmp[cur++] = a[index1++];
		else{
			if(a[index1] <= a[index2]) tmp[cur++] = a[index1++];
			else tmp[cur++] = a[index2++];
		}
	}
	for(int j=first;j<=last;j++) a[j] = tmp[j-first];

}

void merge_sort_ST(int first, int last, int tier)
{
	//continue divide
	if(tier <= 3){
		merge_sort_ST(first, (first+last)/2, tier+1);
		merge_sort_ST((first+last)/2 +1, last, tier+1);
		int len = last - first +1;
		int len1 = (last+first)/2 - first +1;
		int len2 = last - (last+first)/2;
		int tmp[len];
		int cur=0, index1=first, index2=(first+last)/2 +1;

		//do merge
		while(cur <= len){

			if(index1 == len1 + first) tmp[cur++] = b[index2++];
			else if(index2 == len2 + (first+last)/2 +1)
				tmp[cur++] = b[index1++];
			else{
				if(b[index1] <= b[index2]) tmp[cur++] = b[index1++];
				else tmp[cur++] = b[index2++];
			}
		}
		for(int i=first;i<=last;i++) b[i] = tmp[i-first];

	}

	//do sorting
	else bubble_sort_ST(first, last);

	//write file
	if(tier == 1){

		FILE *fout = fopen("output2.txt", "w");
		for(int i=0;i<n;i++) fprintf(fout, "%d ", b[i]);
	}
}

void bubble_sort_ST(int first, int last)
{
	int i,j,tmp;

    	for(int i=last;i>first;i--){
        	for(int j=first;j<i;j++){
            	if(b[j] > b[j+1]) {
               		tmp = b[j];
               		b[j] = b[j+1];
               		b[j+1] = tmp;
            		}
    		}
    	}
}
