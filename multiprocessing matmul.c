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
#include<sys/wait.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/time.h>
#include<stdbool.h>

//matrix size
unsigned int checksum, n;
float time;
bool last;

void matrixMul(int index, int x, unsigned int* in_matrix, unsigned int* out_matrix)
{

	if(!last){
		for(int i= index*n*n/x ; i<(index+1)*n*n/x; i++){
			int row = i/n;
			int column = i%n;
			unsigned int tmp = 0;
			for(int j=0;j<n;j++){
				tmp += in_matrix[n*row+j]*in_matrix[column+j*n];
			}

			out_matrix[i] = tmp;
		}
	}

	else{
		for(int i=index*n*n/x ; i<n*n; i++){
			int row = i/n;
			int column = i%n;
			unsigned int tmp = 0;
			for(int j=0;j<n;j++){
				tmp += in_matrix[n*row+j]*in_matrix[column+j*n];
			}

			out_matrix[i] = tmp;
		}
	}



}

void print(int i)
{
	printf("Multiplying matrices using %d process\n", i);
	printf("Elasped time : ");
	printf("%f usec", time);
	printf(", Checksum : ");
	printf("%u", checksum);
	printf("\n");
}

void createProcess(int id_in, int id_out, int x, unsigned int* in_matrix, unsigned int* out_matrix)
{


	//create n child process
	for(int i=0;i<x;i++){

		if(fork() == 0){

			shmat(id_in, NULL, 0);
			shmat(id_out, NULL, 0);

			//last chlid process
			if(i == x-1) last = true;
			else last = false;

			matrixMul(i,x,in_matrix,out_matrix);

			shmdt(in_matrix);
			shmdt(out_matrix);
			exit(0);
		}
	}

	for(int i=0;i<n;i++) wait(NULL);
}



int main()
{
	scanf("%d", &n);

	//shared mem : A, B
	unsigned int *in_matrix;
	int shmid_in = shmget(0, n*n*sizeof(unsigned int), IPC_CREAT|0666);
	in_matrix = (unsigned int*) shmat(shmid_in, NULL, 0);

	for(int i=0;i<n*n;i++) in_matrix[i] = i;

	//shared mem : C
	unsigned int *out_matrix;
	int shmid_out = shmget(0, n*n*sizeof(unsigned int), IPC_CREAT| 0666);

	out_matrix = (unsigned int*) shmat(shmid_out, NULL, 0);


	for(int j=1;j<=16;j++){

		struct timeval start, end;

		gettimeofday(&start, 0);
		createProcess(shmid_in, shmid_out, j, in_matrix, out_matrix);
		gettimeofday(&end, 0);

		//get time in ms
		int sec = end.tv_sec - start.tv_sec;
		int usec = end.tv_usec - start.tv_usec;
		time =  sec*1000000 + usec;

		checksum = 0;

		for(int i=0;i<n*n;i++) {
			checksum += out_matrix[i];
		}

		print(j);
	}

	/*for(int i =0;i<n*n;i++){ printf("%u", out_matrix[i]);
	printf(" ");
	}*/

	shmdt(in_matrix);
	shmdt(out_matrix);

	shmctl(shmid_in, IPC_RMID, NULL);
	shmctl(shmid_out, IPC_RMID, NULL);
	return 0;
}

