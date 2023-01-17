#ifndef DEV_DEB_H
#define DEV_DEB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DEBUG 1


struct deb_file_entry{
    char* filename;
    long timestamp;
    int owner_id;
    int group_id;
    int f_mode;
    long f_index;
    long f_size;
};

struct deb_file{
    char* filename;
    int entries_nb;
    struct deb_file_entry* entries;
};

void cleanweirddebformat(char* data, size_t size);

struct deb_file parsedebfile(char* filename);

void extractdebfile(struct deb_file file, char* outputfolder);

#endif
