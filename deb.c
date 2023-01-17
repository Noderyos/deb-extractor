#include "deb.h"
#include <lzma.h>



void cleanweirddebformat(char* data, size_t size){
    int index = size - 1;
    while(data[index] == ' '){
        data[index] = '\0';
        index--;
    }
}

int countentries(FILE *ptr){
    int count = 0;

    struct stat st;
    int fd = fileno(ptr);
    fstat(fd, &st);
    off_t size = st.st_size;

    fseek(ptr, 8, SEEK_SET);

    while (ftell(ptr) < size){

        char f_size[11];
        fseek(ptr, 48, SEEK_CUR);
        fread(f_size,10,1,ptr);

        cleanweirddebformat(f_size, 10);
        fseek(ptr, 2, SEEK_CUR);
        fseek(ptr, atol(f_size), SEEK_CUR);
        count++;
    }
    fseek(ptr,8,SEEK_SET);
    return count;
}


struct deb_file parsedebfile(char* filename){

    char *header = "!<arch>\n";
    char fheader[9];
    FILE *ptr;

    ptr = fopen(filename,"rb");

    if(DEBUG)
        printf("[DEBUG] Checking header\n");

    fread(fheader, 8, 1, ptr);
    if(strcmp(fheader, header) != 0)
        fprintf(stderr,"%s doesn't start with DEB magic code\n",filename);

    if(DEBUG)
        printf("[DEBUG] Counting entries\n");

    int entries_nb = countentries(ptr);

    if(DEBUG)
        printf("[DEBUG] %d entries found\n", entries_nb);

    struct deb_file_entry* entries = malloc(sizeof(struct deb_file_entry)*entries_nb);

    int count = 0;

    if(DEBUG)
        printf("[DEBUG] Parsing entries\n");
    
    while (count < entries_nb){

        char *f_name = malloc(17);
        char *timestamp = malloc(13);
        char *owner_id = malloc(7);
        char *group_id = malloc(7);
        char *f_mode = malloc(9);
        char *f_size = malloc(11);
        fread(f_name,16,1,ptr);
        fread(timestamp,12,1,ptr);
        fread(owner_id,6,1,ptr);
        fread(group_id,6,1,ptr);
        fread(f_mode,8,1,ptr);
        fread(f_size,10,1,ptr);


        cleanweirddebformat(f_name, 16);
        cleanweirddebformat(timestamp, 12);
        cleanweirddebformat(owner_id, 6);
        cleanweirddebformat(group_id, 6);
        cleanweirddebformat(f_mode, 8);
        cleanweirddebformat(f_size, 10);

        long size = atol(f_size);

        fseek(ptr, 2, SEEK_CUR);

        long index = ftell(ptr);

        fseek(ptr, size, SEEK_CUR);

        struct deb_file_entry deb = {f_name, atol(timestamp), atoi(owner_id),
                    atoi(group_id), atoi(f_mode), index, atol(f_size)};
        entries[count] = deb;
        count++;

    }
    struct deb_file deb = {filename,entries_nb,entries};
    return deb;
}

void extractdebfile(struct deb_file file, char* outputfolder){
    FILE *ptr = fopen(file.filename,"rb");
    for (int i = 0; i < file.entries_nb; ++i) {
        struct deb_file_entry entry = file.entries[i];
        char *out_file_path = malloc(strlen(outputfolder) + strlen(entry.filename) + 1);
        sprintf(out_file_path,"%s/%s",outputfolder, entry.filename);
        FILE *out = fopen(out_file_path,"wb");
        if(DEBUG)
            printf("[DEBUG] Extracting %s\n",entry.filename);
        fseek(ptr,entry.f_index,SEEK_SET);
        unsigned char *data = malloc(entry.f_size);
        fread(data, entry.f_size, 1, ptr);
        fwrite(data, entry.f_size, 1, out);
        free(out_file_path);
        free(data);
        fclose(out);
    }
    fclose(ptr);
}