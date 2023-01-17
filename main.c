#include "deb.h"
#include <unistd.h>
#include <dirent.h>

int main(int argc, char **argv) {
    if(argc < 2){
        fprintf(stderr, "Usage : %s [file] <folder>\n",argv[0]);
        return 1;
    }

    if (access(argv[1], F_OK) != 0) {
        fprintf(stderr, "File does not exists\n");
    }
    struct deb_file t = parsedebfile(argv[1]);
    if (argc == 3){
        DIR* dir = opendir(argv[2]);
        if (dir) {
            closedir(dir);
        }else{
            printf("[WARNING] Creating folder %s",argv[2]);
            mkdir(argv[2],0700);
        }
        extractdebfile(t, argv[2]);
    }else{
        extractdebfile(t, ".");
    }

    return 0;
}
