#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMROWS 2
#define LARGESTSIZE 56
//#define LARGESTSIZE 1073741816
long long* bs; // block size
long long** pt; // pointer array to the starts of blocks

void fr(void* addr);
void init();
void* mc(int size);

void init(){
	long long acc = 32;
	int i;
	bs = malloc(NUMROWS * sizeof(acc));
	for(i = 0; i < NUMROWS; i++){
		bs[i] = acc;
		acc = acc << 1; //multiply by 2
	}
	assert(bs[NUMROWS-1] == LARGESTSIZE + 8);	
	pt = calloc(NUMROWS,sizeof(long long*));
}

int readHeaderIndex(long long* headerAddr){
	return *((int*)headerAddr);
}
int readBodyIndex(long long* bodyAddr){ 
	return readHeaderIndex(bodyAddr - 1);
}
void setHeaderIndex(long long* headerAddr,int index){
	int* target = (int*) headerAddr;
	*target = index;	
}
void setBodyIndex(long long* bodyAddr,int index){
	int* target = (int*) (bodyAddr - 1);
	*target = index;	
}


long long* readHeaderPointer(long long* headerAddr){
	return *headerAddr;
}
long long* readBodyPointer(long long* bodyAddr){
	return *(bodyAddr - 1);
}
void setHeaderPointer(long long* headerAddr, long long* address){
	*headerAddr = address;
}
void setBodyPointer(long long* bodyAddr, long long* address){
	long long* target = bodyAddr - 1;
	*target = address;
}



void* mc(int size){
	long long searching = size + 8;
	// checks if I need to malloc more space
	if(searching > LARGESTSIZE){
		return malloc(2 * bs[NUMROWS - 1]);	
	}
	// finds the row in the ptrs table
	int ptRow = -1;
	int i;
	for(i = 0; i < NUMROWS; i++){
		if (searching <= bs[i]){
			ptRow = i;
			break;	
		}	
	}
	assert(ptRow >= 0);
	// if no more blocks of this size
	if(pt[ptRow] == 0){
		// 8 bytes into block to split
		long long* toSplit = /*(long long*)*/mc(2 * size + 8);
		long long* startOfBlock = toSplit - 1;
		long long* half = (long long*) ((char*) startOfBlock + bs[ptRow]);
		//assign the corresponding index ptr to the 2nd half of black
		*((int*) half) = ptRow; 
		fr(half + 1); // free 8 bytes after given
		return toSplit;	
	} 


	else{
		long long* out = pt[ptRow];
		//pt[ptRow] = (void*) *((double*)out);//*((void*) out);
		//memcpy(&pt[ptRow], out, sizeof(void*)); // assign the next block
		//pt[i] = *((double*)out); // assign the next block
		*((int*) out) = i; // assign pt index to block
		return out + 1; // return 8 bytes after header
	}

}
// takes in the body of a block(8 bytes after start of header) and frees it
void fr(void* addr){
	long long* startOfBlock = ((long long*) addr) - 1;
	// index of pt row to add block to
	int ptIndex = *((int*)startOfBlock);	

	// update the new Blocks next pointer	
	//memcpy(startOfBlock,pt+ptIndex,sizeof(long long));
	*startOfBlock = (long long)pt[ptIndex];
	pt[ptIndex] = startOfBlock;
	
}


void fixedTest(int size){
	printf("testing with size %d\n",size);
	int i,j;
	int calls = 3;
	int testSize = size;

	long long** buffs = alloca(sizeof(long long*) * calls);
	for(i = 0; i < calls; i++){
		long long* al = mc(testSize);
		memset(al, i + 'a', testSize);
		buffs[i] = al;
	}
	// BUG at this point, buffs[1] and buffs[2] are identical 
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
//	fixedTest(20);
	long long* header = alloca(24);
	long long* body = header +1;

}

