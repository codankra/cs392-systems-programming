#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getTextLine(FILE* binFile, FILE* textFile){ //Rhymes with getNextLine()
	short s1, s2;
	char c1;
	long l1;
	size_t retSize = fread(&s1, sizeof(short), 1, binFile);
	while(retSize == 1){ //retSize should equal arg3 in fread or else EOF or error.
		fprintf(textFile, "%i\t", s1);
		fread(&s2, sizeof(short), 1, binFile);
		fprintf(textFile, "%i\t", s2);
		fread(&c1, sizeof(char), 1, binFile);
		fprintf(textFile, "%i\t", c1);
		fread(&l1, sizeof(long), 1, binFile);
		fprintf(textFile, "%lu\n", l1);
		retSize = fread(&s1, sizeof(short), 1, binFile);
	}
}

int main(int argc, char **argv) { // argv:  bin2text <input filename> <output filename>
	if (argc != 3) { 
		fprintf(stderr, 
			"Wrong number of command-line arguments\n");
		return -1;
	}
	FILE *textFile, *binFile;
	binFile = fopen(argv[1], "r");
	if( binFile == NULL ) {
		perror ("Error opening file");
		return(-1);
	}
	textFile = fopen(argv[2], "w");
	getTextLine(binFile, textFile); //Convert and then write to the file
	fclose(binFile);
	fclose(textFile);
	return(0);
}