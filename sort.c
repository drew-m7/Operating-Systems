#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <float.h>
#include <time.h>
#include <sys/mman.h>
#include <bsd/stdlib.h>

/* multithreaded program to find median, mode, avg using heapsort routine */
/* IT 383 program 2 part 2 */
/* Drew Martin */

const int BUFFER_SIZE = 30;

/* to find the median of numbers */
int getMedian(int *theArray, int theSize) {
	if (theSize % 2 == 1) {
		return theArray[theSize / 2];
	}
	else {
		return (theArray[theSize / 2] + theArray[(theSize / 2) - 1]) / 2;
	}
}

/* to find the mode of the numbers */
void getMode(int *theArray, int theSize) {
	int count = 1;
	int current = theArray[0];
	int found = 0;
	int modeSoFar;
	int x = 1;
	/* searching for the most often found number aka mode */
	while (x < theSize) {
		if (current == theArray[x]) {
			count++;
		}
		else {
			count = 1;
		}
		if (count > found) {
			modeSoFar = theArray[x];
			found = count;
		}
		current = theArray[x];
		x++;
	}
	/* just printing result from function would be easiest */
	printf("The mode is %d and it appears %d times!\n", modeSoFar, found);
}

/* to find the average of the numbers */
float getAvg(int *theArray, int theSize) {
	int x;
	float sum = 0;
	float avg = 0.0;
	/* loop to find sum */
	for (x = 0; x < theSize; x++) {
		sum = sum + theArray[x];
	}
	avg = sum / theSize;
	/* return computed avg */
	return avg;
}

/* compare function needed for built in heap sort */
int compare(const void* a, const void* b) {
	return(*(int*)a - *(int*)b);
}



/* struct for passing data to heapsort */
struct heapSortArgs {
	int left;
	int right;
	int *array;
};

/* use built in heap sort function */
void* runHeapSort(void* args) {
	struct heapSortArgs* x = (struct heapSortArgs*) args;
	/* here I apply the built in heap sorting, 
	args are: array, size from right-left, sizeofint, compare */ 
	heapsort(x->array, (x->right - x->left), sizeof(int), compare);
	return NULL;
}

/* main method */
int main(int argc, char const *argv[]) {
	/* need 3 args */
	if (argc < 3) {
		printf("Incorrect args, not enough, expected: ./sort numOfThreads inputFile.txt\n");
	}
	else if (argc > 3) {
		printf("Too many arguments\n");
	}
	else if (argc == 3) {
		/* correct args */
		printf("Multithreaded sort starting\n");
		/* set everything up */
		int fd; /* file descriptor */
		char* buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE); /* allocate */
		int* fArray;
		int* sArray;
		int threadCount = atoi(argv[1]); /* thread count arg */
		FILE *theFile;
		theFile = fopen(argv[2], "r"); /* input file arg */

		fgets(buffer, sizeof(char)*BUFFER_SIZE, theFile);
		int theSize = atoi(strtok(buffer, "% ")); /* gets size from first line */
		printf("The array size is %d\n", theSize);
		if (theSize == 0) {
			printf("Error retrieving input, or couldn't read file\n");
			exit(1); /* quit since didn't read */
		}
		int maxNum = 1000000000; /* largest possible num */
		/* build array */
		int count = 0; /* count til size is reached */
		sArray = (int*)malloc(sizeof(int)*theSize);

		while (fgets(buffer, BUFFER_SIZE, theFile) != NULL) {
			if (count < theSize) {
				sArray[count] = atoi(buffer);
				count++;
			}
			else {
				printf("Error in making the array from the file\n");
			}
		}
		//int *array;
		//for (int i=0; i < theSize; i++) {
		//	sArray[i] = array[i];
		//}

		/* checking whats in array, comment this loop out */
		//for (int t=0;t<theSize;t++){
			//printf("index[%d] = %d\n",t, sArray[t]);
		//}
		
		/* divide array among threads */
		pthread_t threadArray[threadCount]; /* array to hold threads */

		struct heapSortArgs structArray[threadCount]; /* array for structs to pass data */
		/* add data to struct to use in passing to heapsort for each thread */
		int tmp = theSize/threadCount;
		int currIdx = theSize;
		for (int z = threadCount -1; z >= 0; z--) {
			structArray[z].array = sArray;
			structArray[z].right = currIdx;
			currIdx -= tmp;
			if (z == 0) {
				structArray[z].left = 0;
			}
			else {
				structArray[z].left = currIdx + 1;
			}
		}
		//printf("index check: %d\n", sArray[0]);
		/* start clock */
		clock_t time;
		time = clock();
		printf("Clock starting\n");
		/* calling pthread create and then join */
		for (int z = 0; z < threadCount; z++) {
			/* create each thread and send array for heapsorting */
			pthread_create(&threadArray[z], NULL, &runHeapSort, &structArray[z]);
		}
		for (int z = 0; z < threadCount; z++) {
			pthread_join(threadArray[z], NULL);
		}
		printf("There are %d threads\n", threadCount);
		//printf("index check: %d\n", sArray[0]);
		/* trying to get struct data back to complete array */
		//printf("check\n");
		fArray = (int*)malloc(sizeof(int) * theSize);

		
		int x = 0;
		while (x < theSize) {
			int lft;
			int rght;
			//int* finalArray; 
			int num = maxNum;
			int currIdx = 0;

			//loop thru each thread struct  try to add to final array
			for (int z = 0; z < threadCount; z++) {
				lft = structArray[z].left;
				//set left 
				rght = structArray[z].right;
				//set right
				
				if ((sArray[lft] < num) && (lft <= rght)) {
					currIdx = z;
					num = sArray[lft];
					//fArray[z] = sArray[z];
					//print("%d\n", currIdx);
					//print("%d\n", num);
				}
			}
			structArray[currIdx].left = structArray[currIdx].left + 1;
			// set ind of array to it
			fArray[x] = num;
			
			x++;
		}
		//for (int t=0;t<theSize;t++){
		//	printf("index[%d] = %d\n",t, fArray[t]);
		//}
		//printf("index check: %d\n", fArray[0]);
		time = clock() - time;
		printf("Clock finished\n");
		double totalTime = ((double)time)/CLOCKS_PER_SEC;
		printf("Elapsed time with %d thread(s) is %f\n", threadCount, totalTime);
		printf("The median is %d\n", getMedian(fArray, theSize));
		getMode(fArray, theSize); /* printed from within the function */
		printf("The average is %f\n", getAvg(sArray, theSize));
		printf("Multithreaded heapsort finished\n");
	}
	else {
		printf("Error occured\n");
	}
	return 0;
}
