#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

int main(int argc, char* argv[]){
	int i,j;
	if(argc != 2){
		printf("usage: blockSize\n");
		exit(0);
	}


	srand(time(NULL));
	int numRand = atoi(argv[1]);
	char* fileName = argv[2];
	FILE* currFile = fopen("randArr","w");
	fwrite(&numRand,sizeof(int),1,currFile);
	for(i = 0; i < numRand; i++){
		int r = rand()%26;
		fwrite(&r,sizeof(int),1,currFile);
	}
	fclose(currFile);
}
