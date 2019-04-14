#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "oocmerge.h"

static int fCount = 0; //Used to create distinct file names
static char fname[15];

int verifySort(FILE* input, FILE* output){
	float cur, prev = -100.1; //Assign two comparators less than current float
	
	while((fread(&cur, sizeof(float), 1, input) == 1) && cur>=prev){ 
	//While ascending and not EOF
		fwrite(&cur, sizeof(float), 1, output);
		prev = cur;
	}
	//See what made while loop stop
	if(cur<prev){ //Out of order
		return 0;
	}
	fseek(input, 0, SEEK_CUR);
	long exp = ftell(input);
	fseek(input, 0, SEEK_END);
	long real = ftell(input); //See if really at the end by finding eof and current
	if(exp == real){ //EOF
		return 1;
	}
}

FILE* merge(FILE* arr1, FILE* arr2){
	float f1, f2;
	long size1, size2;
	//Get sizes of files
	fseek(arr1, 0, SEEK_END);
	size1 = ftell(arr1);
	fseek(arr1, 0, SEEK_SET);
	fseek(arr2, 0, SEEK_END);
	size2 = ftell(arr2);
	fseek(arr2, 0, SEEK_SET);
	//Files are reset.
	//Open new file for merge
	sprintf(fname, "arr%i.bin", ++fCount);
	FILE* mergedF = fopen(fname, "wb");
	chmod(fname, 0700);
	while(ftell(arr1) < size1 && ftell(arr2) < size2){ //While both files !eof
		fread(&f1, sizeof(float), 1, arr1);
		fread(&f2, sizeof(float), 1, arr2);
		if(f1<f2){ //Reset f1, write f2
			fseek(arr2, -sizeof(float), SEEK_CUR);
			fwrite(&f1, sizeof(float), 1, mergedF);
		}
		else{ //Reset f2, write f1
			fseek(arr1, -sizeof(float), SEEK_CUR);
			fwrite(&f2, sizeof(float), 1, mergedF);
		}
	}
	while(ftell(arr1) < size1){
		fread(&f1, sizeof(float), 1, arr1);
		fwrite(&f1, sizeof(float), 1, mergedF);
	}
	while(ftell(arr2) < size2){
		fread(&f2, sizeof(float), 1, arr2);
		fwrite(&f2, sizeof(float), 1, mergedF);
	}
	//I think merge should be done now
	//AFTER MERGE:
	//close file, then open it as readable
	fclose(mergedF);
	mergedF = fopen(fname, "r");
	if( mergedF == NULL ) {
		perror("Error opening file mergedF");
	}
	return mergedF;
}



FILE* mSplit(FILE* list, int first, int last){
	int mid = (first + last)/2 - ((first + last)/2)%4; //Trying to find mid float
	int numToRead1, numToRead2;
	//Make sure to not split middle of float
	if (mid>3){
		//More than 1 float in file, so keep getting smaller
		numToRead1 = mid/4; //For readability/countability
		numToRead2 = (last-mid)/4;
		sprintf(fname, "arr%i.bin", ++fCount);
		FILE* lowlist = fopen(fname, "wb");
		chmod(fname, 0700);
		sprintf(fname, "arr%i.bin", ++fCount);
		FILE* highlist = fopen(fname, "wb");
		chmod(fname, 0700);
		float inter;
		while(numToRead1--){
			fread(&inter, sizeof(float), 1, list);
			fwrite(&inter, sizeof(float), 1, lowlist);
		}
		while(numToRead2--){
			fread(&inter, sizeof(float), 1, list);
			fwrite(&inter, sizeof(float), 1, highlist);
		}
		//Close and switch to read
		fclose(lowlist);
		fclose(highlist);
		sprintf(fname, "arr%i.bin", --fCount);
		lowlist = fopen(fname, "r");
		if( lowlist == NULL ) {
			perror ("Error opening file lowlist");
		}
		sprintf(fname, "arr%i.bin", ++fCount);
		highlist = fopen(fname, "r");
		if( highlist == NULL ) {
			perror ("Error opening file highlist");
		}
		//Do merge stuff
		FILE* mFile1 = mSplit(lowlist, first, mid);
		FILE* mFile2 = mSplit(highlist, first, (last-mid));
		FILE* toreturn = merge(mFile1, mFile2);
		//Close lowlist and highlist
		fclose(lowlist);
		fclose(highlist);

		return toreturn;
	}
	else{
		return list;
	}
}

int main(int argc, char const *argv[]){
	//oocmerge <N> <output filename>
	if (argc != 3){
		fprintf(stderr, "Wrong number of command-line arguments\n");
		printf("usage: \n\t<N> (number of floats)\n\t<output filename>\n");
		return -1;
	}
	int N = atoi(argv[1]);
	srand((unsigned int)time(NULL));
	
	mkdir("temp", 0700); //0700 = user only
	chdir("temp");
	//UTL inside temp
	FILE* randNums = fopen("arr0.bin", "wb");
	chmod("arr0.bin", 0700);
	for (int i = 0; i < N; ++i){
		//Generate random numbers [0, 200] then subtract by 100.
		float randNum = (float)((((double)rand()/(double)(RAND_MAX)) * 200.0) - 100.0);
		fwrite(&randNum, 1, sizeof(float), randNums);
	}
	//All nums are stored in initial file at this point.
	//Find size of file:
	fseek(randNums, 0, SEEK_END);
	long flength = ftell(randNums);
	fclose(randNums);
	randNums = fopen("arr0.bin", "r"); //Opening randNums for 2nd time to read
	if( randNums == NULL ) {
		perror ("Error opening file");
		return(-1);
	}
	FILE* fSorted = mSplit(randNums, 0, flength); //HERE is where mSort begins
	fclose(randNums); //Close randNums #2
	char output[100]; //Right now max file name length is assumed to be < 100
	sprintf(output, "../%s", argv[2]);
	FILE* vSorted = fopen(output, "wb"); //Copy to this file for output and verification
	chmod(output, 0700);
	//Copy fSorted to vSorted, verifying along the way
	if (verifySort(fSorted, vSorted))
		printf("Verified Sorted\n");
	else
		printf("File Not In Ascending Order\n");
	fclose(fSorted);
	fclose(vSorted);
	remove("arr0.bin"); //Remove file with original randnums
	for (int i = 1; i <= fCount; i++) //Try to remove all the files
	{
		sprintf(fname, "arr%i.bin", i);
		remove(fname);
	}
	chdir(".."); //Get out of temp/try to delete
	/* CODE FOR DELETING TEMP IF IT IS EMPTY
	int check = rmdir("temp");
	if (!check)
		printf("Directory deleted\n");
	else{   
		printf("Unable to remove directory\n");
		exit(1);
	} 
	*/
	return 0;
}