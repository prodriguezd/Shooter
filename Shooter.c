#include "Info.h"
#include "Find.h"
#include "Delete.h"

#define ERR_VOL         "El sistema de archivos no es ni EXT2 ni FAT16\n"
#define ERR_ARGS 		"Error. Numero incorrecto de argumentos.\n"
#define ERR_OP  		"Operacion incorrecta\n"
#define ERR_FILE        "Error. Volumen no encontrado\n"

int getOption(char* option) {
	if (strcmp(option, "/info") == 0) return 1;
	if (strcmp(option, "/find") == 0) return 2;
	if (strcmp(option, "/delete") == 0) return 3;
	return 0;
}

int main(int argc, char** argv) {
	
	
	int option = getOption(argv[1]);

	if ((option == 1 && argc != 3) || (option == 2 && argc == 3)) write (1, ERR_ARGS, strlen(ERR_ARGS));
	
	
	else {
		int fd;
		if (argc < 3) {
			write(1, ERR_OP, strlen(ERR_OP));
			exit(0);
		}


		if ((fd = open(argv[2], O_RDWR)) < 0) write(1, ERR_FILE, strlen(ERR_FILE));
		
		else {
			int typeVolume = INFO_getType(fd);
			fs_FAT fat;
			fs_ext2 ext;

			switch (typeVolume) {

				case 1: 
					fat = INFO_getInfoFat(fd);

					if (option == 1) showinfoFAT(fat);

					else if (option == 2) FIND_findFileFat(fd, fat, argv[3]);

					else if (option == 3) DELETE_fileFAT(fd, fat, argv[3]);

					else write(1, ERR_OP, strlen(ERR_OP));

					write(1, "\n", 1);

				break;

				case 2: 
					ext = INFO_getInfoExt(fd);

					if (option == 1) showinfoEXT(ext);

					else if (option == 2) FIND_fileExt2(fd, ext, argv[3]);

					else if (option == 3) DELETE_fileExt2(fd, ext, argv[3]);
					
					else write(1, ERR_OP, strlen(ERR_OP));

					write(1, "\n", 1);

				break;

				default:
					write(1, ERR_VOL, strlen(ERR_VOL));
				break;
			}
			close(fd);
		}
		
	}
}
