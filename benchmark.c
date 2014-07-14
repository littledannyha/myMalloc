#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alloca.h>

double totalTime,mallocTime,freeTime;
double diff;

void fixedTest(int size, int calls,int reps){
	int currRep, i;
	for(currRep = 0; currRep < reps; currRep++){

		void** buff = malloc(sizeof(void*) * calls);
		
		clock_t begin,end;
		double time_spent;
		
		begin = clock();

		for(i = 0;i < calls; i++){
			buff[i] = malloc(size);
		}
		
		end = clock();
		diff = ((double)(end - begin) /CLOCKS_PER_SEC); 
		mallocTime += diff;
		totalTime += diff;

		printf("mallocing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin) /CLOCKS_PER_SEC); 


		begin = clock();


		for(i = 0;i < calls; i++){
			free(buff[i]);
		}
		end = clock();


		diff = ((double)(end - begin) /CLOCKS_PER_SEC); 
		freeTime += diff;
		totalTime += diff;


		printf("freeing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin) /CLOCKS_PER_SEC); 
		
		free(buff);
	}

}


void cycleTest(int size, int calls, int reps){
	int i,j;
	clock_t begin,end;
	begin = clock();
	for(i = 0; i < reps; i++){
		for(j = 0; j < reps; j++){
			void* curr = malloc(size);
			free(curr);
		}
	}
	end = clock();
	mallocTime = -1;
	freeTime = -1;
	totalTime = ((double) end - begin) / CLOCKS_PER_SEC;
}


int main(int argc, char* argv[]){
	
	int i; 
	int size,calls,reps,currRep;
	if(argc == 3){
		size = atoi(argv[1]);
		calls = atoi(argv[2]);
		cycleTest(size,calls,1);
		//fixedTest(size,calls,1);
	}
	else if(argc == 4){
		
		size = atoi(argv[1]);
		calls = atoi(argv[2]);
		reps = atoi(argv[3]);

		cycleTest(size,calls,reps);
		//fixedTest(size,calls,reps);
	}
	else{
		printf("usage: size numcalls\n");	
		exit(0);
	}




	printf("Total time mallocing: %lf\n",mallocTime);
	printf("Total time freeing: %lf\n",freeTime);
	printf("Total time: %lf\n",totalTime);

	printf("\n\nSUCCESS\n");

}

