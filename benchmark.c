#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alloca.h>
#include <string.h>
#define RANDCASES 4000000

double totalTime,mallocTime,freeTime;
double diff;




int verifyBlock(long startValue,int size, long long* block){
	int i;
	int lim = size/8; // dealing with number of doubles
	for(i = 0; i < lim; i++){
		assert(block[i] == startValue + i);
	}
}


void fillBlock(long long startValue,int size,long long* block){
	int i;
	int lim = size/8;
	for(i = 0; i < lim; i++){
		block[i] = startValue + i;
	}
}

int* genTest(char* filePath){
	int i,j;
	char* firstSpace;	
	char* firstNewLine;
	char* currLine = calloc(100, sizeof(char));
	FILE* f = fopen(filePath,"r");
	int* out;
	out = calloc(RANDCASES * 3 , sizeof(out));
	for(i = 0; i < RANDCASES; i++){
		fgets(currLine,100,f);
		firstSpace = strchr(currLine,'\t');
		firstNewLine = strchr(currLine,'\n');
		// two numbers
		if((firstSpace != NULL) && (firstSpace < firstNewLine)){
			out[i * 3] = 2;
			out[i * 3 + 1] = atoi(currLine);
			out[i * 3 + 2] = atoi(firstSpace);	
		}
		// one number
		else{
			out[i * 3] = 1;
			out[i * 3 + 1] = atoi(currLine);
			out[i * 3 + 2] = -1;
		}
	}
	free(currLine);
	return out;
} 


void randoTest(char* filePath){
	int i;
	clock_t begin,end;
	int morf,index,size;
	int* args = genTest(filePath);
	printf("file read\n");
	long long** malloced;
	malloced = calloc(sizeof(malloced), 2000);
	int* sizes = calloc(sizeof(int),2000);
	begin = clock();
	for(i = 0; i < 4000000; i++){
		int morf = args[3 * i];
		int index = args[3 * i + 1];
		if(morf == 1){
			//begin = clock();
			verifyBlock(index,sizes[index],malloced[index]);
			free(malloced[index]);
			//end = clock();
			//freeTime += ((double) (end - begin))/CLOCKS_PER_SEC; 
			//totalTime += ((double) (end - begin))/CLOCKS_PER_SEC; 
			malloced[index] = 0;
		}
		// malloc
		else{
			size = args[3 * i + 2];
			sizes[index] = size;
			//assert(malloced[index] == 0);	
			//begin = clock();
			malloced[index] = malloc(size);
			fillBlock(index,sizes[index],malloced[index]);
			//end = clock();
			//mallocTime += ((double) (end - begin))/CLOCKS_PER_SEC; 
			//totalTime += ((double) (end - begin))/CLOCKS_PER_SEC; 
		}
	}
	end = clock();
	totalTime += ((double)(end - begin)/CLOCKS_PER_SEC);
	free(malloced);	
}


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
		/*
		printf("usage: size numcalls\n");	
		exit(0);
		*/
		printf("verifying elements\n");
		randoTest("mf.txt");

	}




	printf("Total time mallocing: %lf\n",mallocTime);
	printf("Total time freeing: %lf\n",freeTime);
	printf("Total time: %lf\n",totalTime);

	printf("\n\nSUCCESS\n");

}

