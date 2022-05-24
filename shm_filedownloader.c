#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>

/* set file size 50 MB */
#define FILE_SIZE 52428800

/* struct same as uploader */
struct filesharing_struct {
	char Flag;
	int theSize;
	char theFileName[50]; /* 50 bytes */
	char theFile[FILE_SIZE]; /* 50 MB */
};

int main () {
	/* main method for downloader */
	printf("Starting downloader!\n");

	int shm_drew_fd;/* shared memory */
	const char *shm_name = "dbmart2_filesharing";
	/* name for shared memory is same as uploader */
	shm_drew_fd = shm_open(shm_name, O_RDONLY, 0666);/* open shared memory */
	/* opened for read only */
	/* check return val */
	if (shm_drew_fd == -1) {
		printf("Error, failed to open\n");
		return -1;
	}
	else {
		printf("Shared memory opened\n");
	}

	/* use ftruncate */
	ftruncate(shm_drew_fd, 53477376); /* 51 MB per the assignment instructions */
	
	if (shm_drew_fd == -1) {
		printf("Error, ftruncate() failed\n");
		return -1;
	}

	printf("Working to download shared memory file...\n");

	/* memory map */
	struct filesharing_struct *fptr; /* file ptr */
	fptr = mmap(0, FILE_SIZE, PROT_READ, MAP_SHARED, shm_drew_fd, 0);
	/* check mmap worked */
	if (fptr == NULL) {
		printf("Error, mmap failed\n");
		return -1;
	}
	else {
		printf("Memory map success\n");
	}

	/* file pointer */
	FILE *fp;

	/*access shared mem to find out if file is downloaded */
	/* if flag */
	if (fptr->Flag) {
		fp = fopen("newDownload", "w"); /* open file for the download */
		/* check if it is there */
		if (fp == NULL) {
			printf("File not opened\n");
			return -1;
		}
		else {
			printf("Download file is opened for writing!\n");
		}

		printf("File is in shared memory\n");
		printf("File name will be: newDownload\n");
		/* tell user what downloaded file is named */

		printf("File size: %d\n", fptr->theSize);
		/* printed out info */
		
		/* similar to uploader, only do 1024 at a time */
		/* char buffer[1024]; */
		/* int count = read(shm_drew_fd, buffer, sizeof(buffer)); */
		
		/* write contents */
		fwrite(fptr->theFile, fptr->theSize, 1, fp);

		/* close */
		fclose(fp);
	}

	/* if not, then something went wrong */
	else {
		printf("File is not available in shared memory.\n");
	}
	shm_unlink(shm_name); /* unlink shared mem */
	
	printf("Successfully unlinked, download finished!\n");
	printf("Congratulations, you have downloaded the file\n");

	/* end */
	return 0;
}

