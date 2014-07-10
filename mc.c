#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SMALLESTSIZE 32

//#define NUMROWS 2
#define NUMROWS 26
//#define LARGESTSIZE 56
#define LARGESTSIZE 1073741816

#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) :(b))
#endif
long long* bs; // block size
long long** pt; // pointer array to the starts of blocks

void fr(void* addr);
void init();
void* mc(int size);

void init(){
	long long acc = SMALLESTSIZE; int i;
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
	setHeaderIndex(bodyAddr - 1,index);
}


long long* readHeaderPointer(long long* headerAddr){
	return (long long*) *headerAddr;
}
long long* readBodyPointer(long long* bodyAddr){
	return readHeaderPointer(bodyAddr - 1);
}
void setHeaderPointer(long long* headerAddr, long long* address){
	*headerAddr = (long long) address;
}
void setBodyPointer(long long* bodyAddr, long long* address){
	setHeaderPointer(bodyAddr - 1, address);
}



void* mc(int size){
	size = max(size,24);	
		
	long long searching = size + 8;
	// checks if I need to malloc more space
	if(searching > LARGESTSIZE){
		return malloc(2 * bs[NUMROWS - 1]);	
	}
	// finds the row in the ptrs table
	int ptRow,i;
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
		long long* toSplit = (long long*)mc(2 * size + 8);
		long long* startOfBlock = toSplit - 1;
		long long* half = (long long*) (((char*) startOfBlock) + bs[ptRow]);
		//assign the corresponding index ptr to the 2nd half of block
		setHeaderIndex(half,ptRow);
		fr(half + 1); // free 8 bytes after given
		// set header of the block to return to the corresponding row
		setHeaderIndex(startOfBlock,ptRow);
		return toSplit;	
	} 


	else{
		long long* out = pt[ptRow];
		long long* nextOutAddr = readHeaderPointer(out);
		pt[ptRow] =  nextOutAddr; // assign the next block
		setHeaderIndex(out,ptRow); // assign pt index to block
		return out + 1; // return 8 bytes after header
	}

}
// takes in the body of a block(8 bytes after start of header) and frees it
void fr(void* addr){
	long long* startOfBlock = ((long long*) addr) - 1;
	// index of pt row to add block to
	int ptIndex = readBodyIndex(addr);//*((int*)startOfBlock);	
	// update the new Blocks next pointer	
	long long* toSet = pt[ptIndex];
	setHeaderPointer(startOfBlock,pt[ptIndex]);
	// update the pt list
	pt[ptIndex] = startOfBlock;
	
}


void fixedTest(int size){
	printf("\nSTART TEST ON SIZE %d\n\n",size);
	int i,j;
	int calls = 3;
	int testSize = size;
	


	// finds the row in the ptrs table
	int ptRow = -1;
	
	for(i = 0; i < NUMROWS; i++){
		if (size <= bs[i]){
			ptRow = i;
			break;	
		}	
	}
	



	long long** buffs = alloca(sizeof(long long*) * calls);
	memset(buffs,0,sizeof(long long*) * calls);
	printf("begin malloc\n\n");
	for(i = 0; i < calls; i++){
		long long* al = mc(testSize);
		printf("malloc'd at index %d: 0x%x\n",i,al);
		memset(al, i + 'a', testSize);
		buffs[i] = al;
		// printf("Current header at index %d: %d\n",i,readBodyIndex(buffs[i]));
	}
	// BUG at this point, buffs[1] and buffs[2] are identical 
	printf("\n\nprinting contents\n\n");
	for(i = 0; i < calls; i++){
		char* curr = (char*) buffs[i];
		for(j = 0; j < testSize; j++){
			assert(curr[j] == i + 'a');	
		}
		printf("%s\n",curr);

	}

	printf("\n\nbegin freeing buffs\n\n");
	for(i = 0; i < calls;i++){
		printf("freeing item %d at address 0x%x\n",i,buffs[i]);
		printf("this block's next: 0x%x\n",pt[ptRow]);
		fr(buffs[i]);	
	}
}



void printRows(){
	int i; 
	for(i = 0; i < NUMROWS; i++){
		void* row = pt[i];
		int acc = 0;
		void* curr = row;

		printf("row %d: \t",i);
		while(curr != 0){
			printf("%x\t",curr);
		//TODO fix this	

		}
		printf("\n");	
	}

}
int main(int argc, char* argv[]){
	int i; 
	int min,max,target;
	init();

	for(i = 0; i < 100; i++){
		printf("\n");
	}
	if(argc == 2){
		fixedTest(atoi(argv[1]));
	}
	else if(argc == 3){
		int max = atoi(argv[2]);
		for(i = atoi(argv[1]); i < max; i++){
			fixedTest(i);	
		}
	}
	else{
		int fibSize = 1;
		for(i = 1; fibSize < LARGESTSIZE; i++){
			fibSize *= i;
			fixedTest(fibSize);	
		}

	}

	/*
	for(i = 9; i < 5700; i++){
		fixedTest(i);
	}
	*/
	printf("\n\nSUCCESS\n");
	/*
	for(i = 100; i < 102; i++){
		fixedTest(i);
	}
*/
	//fixedTest(9);
	/*
	long long* header = alloca(24);
	long long* body = header +1;
	
	setHeaderIndex(header,33);
	assert(readHeaderIndex(header) == readBodyIndex(body));
	assert(readHeaderPointer(header) == readBodyPointer(body));
	*/
}

