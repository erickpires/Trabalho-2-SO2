#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>

int isopen (int fd) {
	static struct stat buffer = {0};
	int has_error = fstat(fd, &buffer);

	if(has_error && errno == EIO) {
		fprintf(stderr, "Falha de I/O\n");
		exit(-1);
	}

	return !has_error || (errno == EOVERFLOW);
}


int main (void) {

	int arq1 = open("./test", O_RDONLY);
	int arq2 = open("./tes", O_RDONLY);
	int arq3 = open("./test", O_RDONLY);

	close(arq1);

    int  nopen, fd;
    for (nopen = fd = 0; fd < getdtablesize (); fd++) {
        if (isopen (fd))
			nopen++;
    }
    printf ("Existem %d descritores abertos\n", nopen);
    return (0);
}   /* end main */
