#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int binConvert(char* textLine, FILE* binFile){
	char* parts;
	parts = strtok(textLine, "\t");
	short uid = (short)atoi(parts);
	fwrite(&uid, sizeof(short), 1, binFile);
	parts = strtok(NULL, "\t");
	short iid = (short)atoi(parts);
	fwrite(&iid, sizeof(short), 1, binFile);
	parts = strtok(NULL, "\t");
	char rat = (char)atoi(parts);
	fwrite(&rat, 1, 1, binFile);
	parts = strtok(NULL, "\t");
	long tsp = (long)atoi(parts);
	fwrite(&tsp, sizeof(long), 1, binFile);
}

int main(int argc, char **argv) { // argv:  text2bin <input filename> <output filename>
	if (argc != 3) { 
		fprintf(stderr, 
			"Wrong number of command-line arguments\n");
		return -1;
	}
	FILE *textFile, *binFile;
	textFile = fopen(argv[1], "r");
	if( textFile == NULL ) {
		perror ("Error opening file");
		return(-1);
	}
	binFile = fopen(argv[2], "wb");
	char* line = (char*)malloc(sizeof(char) *100);
	while(fgets(line, 100, textFile) != NULL){
		binConvert(line, binFile);
	}
	free(line);
	fclose(textFile);
	fclose(binFile);
	return(0);
}