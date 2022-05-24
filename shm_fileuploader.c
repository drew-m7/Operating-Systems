#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

/* maximum size is 50 MB as assignment says */
#define FILE_SIZE 52428800

/* struct for file sharing as defined in assignment sheet */
struct filesharing_struct {
	char Flag;
	int theSize;
	char theFileName[50]; /* 50 bytes for name */
	char theFile[FILE_SIZE]; /* 50 MB for file size */
};

/* main method for file uploader */
int main() {
	printf("Starting uploader\n");

	/* new shm object */
	int shm_drew_fd;
	/* name it */
	const char *shm_name = "dbmart2_filesharing";/* named correctly based on assignment sheet*/

	/* open shared mem for read write */
	shm_drew_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666); /* open shm */
	/* check return */
	if (shm_drew_fd == -1) {
		printf("Error, open failed\n");
		return -1;
	}
	else {
		printf("Shared memory opened\n");
	}


	/* truncate to 51 MB based on assignment requirement */
	ftruncate(shm_drew_fd, 53477376);
	/* check if failed */
	if (shm_drew_fd == -1) {
		printf("ftruncate() failed\n");
		return -1;
	}
	/* memory map */
	struct filesharing_struct *fptr;/* file ptr*/
	/* memory map into shared memory */
	fptr =  mmap(0, FILE_SIZE, PROT_WRITE, MAP_SHARED, shm_drew_fd, 0);

	/* check return value of mmap() */
	if (fptr == NULL) {
		printf("Error, mmap failed\n");
		return -1;
	}
	else {
		printf("Memory mapped\n");
	}

	/* now ask what name of file to be uploaded is */
	printf("What is the name of file that is to be uploaded: \n");
	/* name of file from user */
	char name[50];
	scanf("%s", name);
	/* tell user the file name */
	printf("The file is %s\n", name);

	/* file pointer call it fp for short */
	FILE *fp;
	fp = fopen(name, "r"); /* open for read */
	/* Check return to see if file actually exists */
	if (fp == NULL) {
		printf("Error, file not found!\n");
		return -1;
	}
	else {
		printf("File has been found\n");
	}


	/* read file for shared memory */
	
	/* utilize fseek() ftell() to get size */
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	/* int size is true size of uploaded file */
	/* make sure to go back to the beginning of file */
	rewind(fp);

	/* flag and size */
	fptr->theSize = sizeof(char) * size;
	fptr->Flag = 1;
	/* read the content with fread() and then close */
	char buffer[1024];/* required only read in 1024 at a time */
	int count = read(shm_drew_fd, buffer, sizeof(buffer));
	while (count > 0) {
		fread(fptr->theFile, sizeof(buffer), count, fp);
		count = read(shm_drew_fd, buffer, sizeof(buffer));
	}
	/* finished reading */
	printf("File size is %d\n", size);
	printf("File uploaded to shared memory\n");
	/* closing it */
	fclose(fp);
	
	printf("Success!\n");

	/* end */
	return 0;
	/* file from user has been uploaded to shared memory for downloader to get */

}

