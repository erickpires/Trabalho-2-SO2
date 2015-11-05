#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>

typedef enum {
	regular,
	directory,
	link_file,
	block,
	character
} file_type;

file_type types = directory;
unsigned int count = 0;

int walk_dir (const char *path, void (*func) (const char *)) {
	DIR *dirp;
	struct dirent *dp;
	char *p, *full_path;
	int len;
	/* abre o diretório */
	if ((dirp = opendir (path)) == NULL)
		return (-1);
	len = strlen (path);
	/* aloca uma área na qual, garantidamente, o caminho caberá */
	if ((full_path = malloc (len + NAME_MAX + 2)) == NULL) {
		closedir (dirp);
		return (-1);
	}
	/* copia o prefixo e acrescenta a ‘/’ ao final */
	memcpy (full_path, path, len);
	p = full_path + len; *p++ = '/';     /* deixa “p” no lugar certo! */
	while ((dp = readdir (dirp)) != NULL) {
		/* ignora as entradas “.” e “..” */
		if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
			continue;
		strcpy (p, dp->d_name);
		/* “full_path” armazena o caminho */
		(*func) (full_path);
	}
	free (full_path);
	closedir (dirp);
	return (0);
}    /* end walk_dir */

void count_files(const char* path) {
	struct stat buffer = {0};
	lstat(path, &buffer);

	switch (types) {
	case regular:
		if (S_ISREG(buffer.st_mode)) count++;
		break;
	case directory:
		if (S_ISDIR(buffer.st_mode)) count++;
		break;
	case link_file:
		if (S_ISLNK(buffer.st_mode)) count++;
		break;
	case block:
		if (S_ISBLK(buffer.st_mode)) count++;
		break;
	case character:
		if (S_ISCHR(buffer.st_mode)) count++;
		break;
	default:
		break;
	}
}

int main(int argc, char** argv) {
	walk_dir(".", count_files);

	int opt;

	while((opt = getopt(argc, argv, "rdlbca")) != -1) {
		switch (opt) {
		case 'r' :
			puts("regular");
			break;
		case 'd' :
			puts("dir");
			break;
		case 'l' :
			puts("link");
			break;
		case 'b' :
			puts("block");
			break;
		case 'c' :
			puts("char");
			break;
		case 'a' :
			puts("all");
			break;
		}
	}

	if (optind < argc)
{
    do
    {
        char *file = argv[optind];
        printf("%s\n", file);
        // do something with file
    }
    while ( ++optind < argc);
}

	printf("%d\n", count);

	return 0;
}
