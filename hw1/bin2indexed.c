#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long* findOffsets(FILE* itemFile){
	long* id = (long*)malloc(sizeof(long) *1000);
	int lineNum = 1;
	int maxLines = 1000;
	char line[300];
	while(!feof(itemFile)){
		id[lineNum-1] = ftell(itemFile);
		if (fgets(line, 300, itemFile) != NULL){
			lineNum++;
			if(lineNum == maxLines){
				maxLines*=2;
				id = (long*)realloc(id, sizeof(long)*maxLines);
			}
		}
	}
	return id;
}
int binThrough(FILE* binFile, FILE* indexFile, long* id){
	char* strcp;
	short s1;
	long l1;
	size_t retSize = fread(&strcp, sizeof(char), 2, binFile);
	while(retSize == 2){ //retSize should equal arg3 in fread or else EOF or error.
		fwrite(&strcp, sizeof(char), 2, indexFile);
		fread(&s1, sizeof(short), 1, binFile);
		l1 = id[s1-1];
		fwrite(&l1, sizeof(long), 1, indexFile);
		fread(&strcp, sizeof(char), 9, binFile);
		fwrite(&strcp, sizeof(char), 9, indexFile);
		retSize = fread(&strcp, sizeof(char), 2, binFile);
	}
}

int main(int argc, char **argv) { // argv:  bin2text <input filename> <output filename>
	if (argc != 4) { 
		fprintf(stderr, 
			"Wrong number of command-line arguments\n");
		return -1;
	}
	FILE *itemFile, *binFile, *indexFile;
	binFile = fopen(argv[1], "r");
	if( binFile == NULL ) {
		perror ("Error opening binary file");
		return(-1);
	}
	itemFile = fopen(argv[2], "r");
	if( itemFile == NULL ) {
		perror ("Error opening item file");
		return(-1);
	}
	indexFile = fopen(argv[3], "wb");
	//long* id = (long*)malloc(sizeof(long) *1000);
	long* id = findOffsets(itemFile);
	fclose(itemFile);
	binThrough(binFile, indexFile, id);
	free(id);
	fclose(binFile);
	fclose(indexFile);

	return(0);
}
