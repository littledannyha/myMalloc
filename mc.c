#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SMALLESTSIZE 32

//#define NUMROWS 2
//#define LARGESTSIZE 56
#define NUMROWS 26
#define LARGESTSIZE 1073741816


#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) :(b))
#endif
long long* bs; // block size
long long** pt; // pointer array to the starts of blocks

void fr(void* addr);
void init();
void* mc(int size);
//
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

void terminate(){

	free(bs);
	free(pt);
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


void fixedTest(int size,int c, int verify){
	clock_t begin,end;
	double time_spent;
	int i,j;
	int calls = c;
	int testSize = size;



	printf("\nSTART TEST ON SIZE %d\n\n",size);
	

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
	begin = clock();
	for(i = 0; i < calls; i++){
		long long* al = mc(testSize);
		buffs[i] = al;
//		printf("malloc'd at index %d: 0x%x\n",i,al);

		if(verify){memset(al, (i*i + 'a')%126, testSize);}
//		buffs[i] = mc(testSize);
	}
	end = clock();



	printf("mallocing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin)/CLOCKS_PER_SEC);


	if(verify){
		printf("\n\nverifying contents\n\n");
		for(i = 0; i < calls; i++){
			char* curr = (char*) buffs[i];
//			printf("%s\n",curr);
			for(j = 0; j < testSize; j++){
				//assert(curr[j] == i + 'a');	

				if(curr[j] != (i*i + 'a')%126){
					printf("curr[j] != i + 'a' failed with\t i: %d\t j: %d\t char:%s\n",i,j);	
					exit(1);
				};	
			}
		}
	}




	printf("\n\nbegin freeing buffs\n\n");
	begin = clock();
	for(i = 0; i < calls;i++){
//		printf("freeing item %d at address 0x%x\n",i,buffs[i]);
//		printf("this block's next: 0x%x\n",pt[ptRow]);
		fr(buffs[i]);	
	}
	end = clock();
	printf("freeing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin)/CLOCKS_PER_SEC);
}

void loopTest(int size, int calls,int verify, int reps){
	int i;

	for(i = 0; i < reps; i++){
		fixedTest(size,calls,verify);

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
	int verify = strncmp(argv[argc-1],"-v",2) == 0 ? 1 : 0; // flag used to verify blocks
	if(verify){printf("verifying blocks");}

	for(i = 0; i < 100; i++){
		printf("\n");
	}
	if(argc - verify == 3){
		fixedTest(atoi(argv[1]),atoi(argv[2]),verify);
	}
	if(argc - verify == 4){
		loopTest(atoi(argv[1]),atoi(argv[2]),verify,atoi(argv[3]));
	}
	else{
		fixedTest(1000,1000000,1);
	//	printf("usage: size numcalls [-v]\n");	
		exit(0);
		/*
		int fibSize = 1;
		for(i = 1; fibSize < LARGESTSIZE; i++){
			fixedTest(fibSize, verify);	
			fibSize *= i;
		}
		*/
	}

	printf("\n\nSUCCESS\n");
}

