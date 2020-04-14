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
#include<queue>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<string.h>
#include<sys/time.h>

using namespace std;

int n, n_elements;
int *a, *b;
int st[15], end[15];
int merge_cnt[7];
sem_t thread_free[8];
sem_t sort_done, job_coming, queue_lock;
pthread_t *tid;
queue<int> job_list;


/*------------------------------------
*	
	job id:
	0 : master
	1-6 : merge
	7-15 : bubble sort

	bind job id to thread id

-------------------------------------*/
	
void init_job()
{
	
	memset(merge_cnt, 0, 7*sizeof(int));

	//restore input array to output array
	for(int i=0;i<n_elements;i++) b[i] = a[i];

	sem_wait(&queue_lock);
		
	//inital job list
	if(!job_list.empty()) job_list.pop();

	for(int i=7;i<15;i++) {
		job_list.push(i);
		sem_post(&job_coming);
	}

	sem_post(&queue_lock);

	//signal n thread can work
	for(int i=1;i<=n;i++)
		sem_post(&thread_free[i-1]);
}

void read_input()
{

	FILE *fin = fopen("input.txt","r");
	fscanf(fin, "%d", &n_elements);
	//printf("%d", n);

	a = (int*)malloc(n_elements * sizeof(int));
	b = (int*)malloc(n_elements * sizeof(int));
	
	int tmp;
	for(int i=0;i<n_elements;i++){
		fscanf(fin, "%d", a+i);
		b[i] = a[i];
	}

	fclose(fin);
}

void bubble_sort(int cur)
{
	int first = st[cur];
	int last = end[cur];
	int tmp;
	//cout<<cur<<" "<<first<<" "<<last<<endl;
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

void divide(int cur)
{
	int first = st[cur], last = end[cur];
	int mid = (first + last)/2;

	//calcuate start and end except the last tier job
	if(cur < 7){
		st[2*cur+1] = first, end[2*cur+1] = mid;
		st[2*cur+2] = mid+1, end[2*cur+2] = last;
		//cout << cur << st[cur] << " "<< end[cur]<<endl;
	
		divide(2*cur+1);
		divide(2*cur+2);
	}
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

		//child 1 finish merging
		if(index1 == first + len1) tmp[cur++] = b[index2++];

		//child 2 finish merging
		else if(index2 == (first+last)/2+1 + len2)
			tmp[cur++] = b[index1++];
		else{
			if(b[index1] <= b[index2]) tmp[cur++] = b[index1++];
			else tmp[cur++] = b[index2++];
		}
	}
	for(int j=first;j<=last;j++) {
		b[j] = tmp[j-first];
	}

}

void* threadpool(void *_args)
{
	int id = (int) _args;

	while(true){

		if(n > 8) break;
		
		sem_wait(&thread_free[id]);
		sem_wait(&job_coming);

		sem_wait(&queue_lock);

		//get the first job in the list
		int job = job_list.front();
		job_list.pop();

		sem_post(&queue_lock);
		
		int parent = (job-1)/2;
		//cout<<job<< " "<<parent<<endl;

		if(job >= 7 && job < 15) bubble_sort(job);
		else merge(job);

		//except for the main job with no parent (job id = 0)
		//count the parnet's merge num
		if(!job) sem_post(&sort_done);
		else{
			merge_cnt[parent]++;

			//two children both finish merging
			//insert parent's job
			if(merge_cnt[parent] == 2){

				sem_wait(&queue_lock);
				job_list.push(parent);
				sem_post(&queue_lock);
				sem_post(&job_coming);
			}
		}

		sem_post(&thread_free[id]);

	}
	
}


int main()
{

	int sec, usec;
	float time;
	struct timeval start_time, end_time;

	//initital semaphort
	sem_init(&sort_done, 0, 0);
	sem_init(&job_coming, 0, 0);
	sem_init(&queue_lock, 0, 1);

	for(int i=0;i<8;i++) sem_init(&thread_free[i], 0, 0);
	
	read_input();
	
	//initial start and end index
	st[0] = 0;
	end[0] = n_elements-1;
	divide(0);
		
	//initial thread
	tid = (pthread_t*)malloc(8*sizeof(pthread_t));
	for(int i=0;i<8;i++) pthread_create(&tid[i], NULL, threadpool, (void*) i);

	for(n=1;n<=8;n++){

		init_job();

		gettimeofday(&start_time, 0);

		sem_wait(&sort_done);

		gettimeofday(&end_time, 0);

		//get time in ms
		sec = end_time.tv_sec - start_time.tv_sec;
		usec = end_time.tv_usec - start_time.tv_usec;
		time = sec * 1000000 + usec;

		//output -> execution time
		printf("sorting with a threadpool of %d threads: %f ms\n", n, time);
		time = 0;

		//output -> sorted array
		char out_name[15];
		sprintf(out_name, "output_%d.txt", n);

		FILE *fout = fopen(out_name, "w");
		for(int i=0;i<n_elements;i++) fprintf(fout, "%d ", b[i]);
		fclose(fout);
	
	}

	free(tid);
	free(a);
	free(b);
	return 0;

}


