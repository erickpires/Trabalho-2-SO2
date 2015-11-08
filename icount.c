#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/stat.h>

#define FALSE 0
#define TRUE  1

typedef unsigned int uint;

typedef enum {
	// Iniciando enum com 1 para podermos testar se a variável já foi "setada"
	regular = 1,
	directory,
	link_file,
	block,
	character
} file_type;

file_type types = 0;
uint count = 0;
uint use_all_files = FALSE;
uint verbose_mode = FALSE;

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

	if(strstr(path, "/.") && !use_all_files) // O arquivo é oculto e não será considerado
		return;

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

	if(verbose_mode)
		printf("%d : %s\n", count, path);
}

void more_than_one_type_error() {
	fprintf(stderr, "Specify only one type of file.\n");
	exit(1);
}

int main(int argc, char** argv) {
	int opt;
	char* pathname = NULL;
	while((opt = getopt(argc, argv, "rdlbcav")) != -1) {
		switch (opt) {
		case 'r' :
			if(!types) types = regular;
			else more_than_one_type_error();
			break;
		case 'd' :
			if(!types) types = directory;
			else more_than_one_type_error();
			break;
		case 'l' :
			if(!types) types = link_file;
			else more_than_one_type_error();
			break;
		case 'b' :
			if(!types) types = block;
			else more_than_one_type_error();
			break;
		case 'c' :
			if(!types) types = character;
			else more_than_one_type_error();
			break;
		case 'a' :
			use_all_files = TRUE;
			break;
		case 'v' :
			verbose_mode = TRUE;
			break;
		default :
			break;
		}
	}

	// Se nenhum tipo for informado, usar "regular" como padrão
	if(!types) types = regular;

	if (optind < argc) {
        pathname = argv[optind];
    }
    else {
    	fprintf(stderr, "You must specify a path.\n");
    	exit(2);
    }


	walk_dir(pathname, count_files);
	printf("%d\n", count);

	return 0;
}
