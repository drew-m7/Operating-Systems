all: shm_fileupload shm_filedownload

shm_fileupload: shm_fileuploader.c
	gcc -o shm_fileupload shm_fileuploader.c -lrt

shm_filedownload: shm_filedownloader.c
	gcc -o shm_filedownload shm_filedownloader.c -lrt
