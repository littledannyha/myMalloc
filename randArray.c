#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

int main(int argc, char* argv[]){
	int i,j;
	if(argc != 3){
		printf("usage: blockSize numBlocks\n");
		exit(0);
	}

	srand(time(NULL));
	int numRand = atoi(argv[1]);
	char* fileName = argv[2];
	FILE* currFile = fopen(fileName,"w");


	
	fputc('{',currFile);	
	for(i = 0; i < numRand; i++){
		int r = rand()%26;
		fprintf(currFile,"%d",r);
		if(i != numRand -1){
			fputc(',',currFile);		
		}
	}
	fputc('}',currFile);	
	fputc('\n',currFile);	




	fclose(currFile);
}
