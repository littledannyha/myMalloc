#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMROWS 26
#define LARGESTSIZE 1073741816
int* bs; // block size
void** pt; // pointer array

void fr(void* addr);
void init();
void* mc(int size);

void init(){
	int acc = 32;
	int i;
	bs = malloc(NUMROWS * sizeof(int));
	for(i = 0; i < NUMROWS; i++){
		bs[i] = acc;
		acc = acc << 1; //multiply by 2
	}
	assert(bs[NUMROWS-1] == LARGESTSIZE + 8);	
	pt = (void**)calloc(NUMROWS,sizeof(void*));
}

void* mc(int size){
	int searching = size + 8;
	// checks if I need to malloc more space
	if(searching > LARGESTSIZE){
		return malloc(bs[NUMROWS - 1]);	
	}
	// finds the row in the ptrs table
	int ptRow = -1;
	int i;
	for(i = 0; i < NUMROWS; i++){
		if (searching < bs[i]){
			ptRow = i;
			break;	
		}	
	}
	assert(ptRow >= 0);
	// if there are no more blocks of this size,get a larger one and split
	if(pt[ptRow] == 0){
		void* toSplit = mc(2 * size + 8);
		void* half = (char*) toSplit + bs[ptRow];
		*((int*) half) = ptRow; //assign the corresponding index ptr to the 2nd half of black
		fr((char*)half + 8);
		return (char*)toSplit + 8;	
	} 
	else{
		void* out = pt[ptRow];
		//pt[ptRow] = (void*) *((double*)out);//*((void*) out);
		memcpy(&pt[ptRow], out, sizeof(void*)); // assign the next block
		//pt[i] = *((double*)out); // assign the next block
		*(int*) out = i; // assign pt index to block
		return (char*) out + 8; 
	}

}

void fr(void* addr){
	void* startOfBlock = (char*) addr - 8;
	int ptIndex = *((int*)startOfBlock);	
	
	memcpy(startOfBlock, &pt[ptIndex], sizeof(void*));//block knows where next free block is 
	pt[ptIndex] = startOfBlock;
	
}


void fixedTest(int size){
	printf("testing with size %d\n",size);
	int i,j;
	int calls = 3;
	int testSize = size;

	void** buffs = alloca(sizeof(double*) * calls);
	for(i = 0; i < calls; i++){
		void* al = mc(testSize);
		memset(al, i + 'a', testSize);
		buffs[i] = al;
	}

	for(i = 0; i < calls; i++){
		char* curr = (char*) buffs[i];
		for(j = 0; j < testSize; j++){
			//assert(curr[j] == i + 'a');	
		}
		printf("%s\n",curr);
		
	}
	
	for(i = 0; i < calls;i++){
		fr(buffs[i]);	
		
	}
	printf("\n\n");
}

void printRows(){
	int i; 
	for(i = 0; i < NUMROWS; i++){
		void* row = pt[i];
		int acc = 0;
		void* curr = row;

		while(curr != 0){
			acc++;
		//TODO fix this	

		}
		printf("row %d: %d items",i,acc);
		
	}

}
int main(int argc, char* argv[]){
	int i; 
	init();
	/*for(i = 8; i < 55; i++){
		fixedTest(i);
	}
	for(i = 100; i < 102; i++){
		fixedTest(i);
	}*/
	for(i = 20; i <= 20; i++){

		fixedTest(i);
	}
	//fixedTest(20);
}

