/*
Student No.: 0616308
Student Name: Ho Chia Shin
Email: annie520143@gmail.com
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page.
*/

#include<iostream>
#include<stdio.h>
#include<fstream>
#include<map>
#include<list>
#include<set>
#include<utility>
#include<iomanip>
#include<sys/time.h>
#include<unistd.h>

using namespace std;

#define ref_n first
#define page_n second

int frame[4] = {64, 128, 256, 512};

/***************************************
*	I didn't use linear search         *
*	but it is still slowly!!                *
***************************************/

int main(int argc, char* argv[])
{

	if(argc != 2) return 0;//perror("ERROR: ");

	float time;
	struct timeval st, en;
	//LFU
	printf("LFU policy:\n");
	printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");
	
	for(int i=0;i<4;i++){

		int frame_n = frame[i];
		unsigned int x;
		int hit=0, miss=0;
		float pf_ratio;
		set< pair<int,int> > S;
		set< pair<int,int> >::iterator it;
		map<int, int> M;

		gettimeofday(&st, NULL);
		FILE* fin = fopen(argv[1], "r");

		while(fscanf(fin, "%u", &x) != EOF){

			//x not in the frame : miss
			if( M.find(x) == M.end()){

				//load x into map
				M[x] = 1;
				miss++;

				//list full -> remove least used
				if( S.size() == frame_n){

					it = S.begin();
					S.erase( make_pair(it->ref_n, it->page_n));
					S.insert( make_pair(1,x) );
					M.erase(it->page_n);

				}

				else S.insert( make_pair(1,x) );
			} 

			//x in the frame :hit -> renew value
			else{
				it = S.find( make_pair(M[x], x) );
				S.erase( make_pair(it->ref_n, it->page_n));
				S.insert( make_pair(++M[x],x) );
				hit++;
			}
		}
		
		int total = hit+miss;
	 	pf_ratio = (float)miss / (float)total ;
		printf("%d\t%d\t\t%d\t\t", frame_n, hit, miss);
		cout<<fixed<<setprecision(10)<<pf_ratio<<"\n";

		gettimeofday(&en, NULL);
		time += (float)(en.tv_sec - st.tv_sec) + (float)(en.tv_usec - st.tv_usec)/1000000;


	}

	printf("Total elapsed time %.4f sec\n",time );

	//LRU
	time = 0;
	printf("LRU policy:\n");
	printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");

	for(int i=0;i<4;i++){

		int frame_n = frame[i];
		unsigned int x;
		int hit=0, miss=0;
		float pf_ratio;
		list<int> L;
		map<int, int> M;

		gettimeofday(&st, NULL);
		FILE* fin = fopen(argv[1], "r");

		while(fscanf(fin, "%u", &x) != EOF){

			//x not in the frame : miss
			if( M.find(x) == M.end()){

				//load x into map
				M[x] = 1;
				miss++;

				//list full -> remove least used
				if( L.size() == frame_n){
					int remove_page = L.back();
					L.pop_back();
					L.push_front(x);
					M.erase(remove_page);
				}

				else L.push_front(x);
			} 

			//x in the frame :hit -> renew value
			else{
				L.remove(x);
				L.push_front(x);
				hit++;
			}
		}
		
		int total = hit+miss;
	 	pf_ratio = (float)miss / (float)total ;
		//cout<<total;
		printf("%d\t%d\t\t%d\t\t", frame_n, hit, miss);
		//printf("%llf\n", pf_ratio);
		cout<<fixed<<setprecision(10)<<pf_ratio<<"\n";

		gettimeofday(&en, NULL);
		time += (float)(en.tv_sec - st.tv_sec) + (float)(en.tv_usec - st.tv_usec)/1000000;


	}

	printf("Total elapsed time %.4f sec\n",time );



	return 0;
}

