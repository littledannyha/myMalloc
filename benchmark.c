#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <alloca.h>

int main(int argc, char* argv[]){
	
	int i; 
	int size,calls;
	for(i = 0; i < 100; i++){
		printf("\n");
	}
	if(argc == 3){
		size = atoi(argv[1]);
		calls = atoi(argv[2]);
	}
	else{
		printf("usage: size numcalls\n");	
		exit(0);
	}


	void** buff = alloca(sizeof(void*) * calls);
	
	clock_t begin,end;
	double time_spent;
	
	begin = clock();

	for(i = 0;i < calls; i++){
		buff[i] = malloc(size);
	}
	
	end = clock();
	printf("mallocing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin) /CLOCKS_PER_SEC); 



	begin = clock();


	for(i = 0;i < calls; i++){
		free(buff[i]);
	}



	end = clock();
	printf("mallocing %d blocks of size %d took %lfs\n",calls,size,(double)(end - begin) /CLOCKS_PER_SEC); 
	






	printf("\n\nSUCCESS\n");
}
