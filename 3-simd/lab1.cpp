
#include <fcntl.h>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>



struct RAW {
    unsigned char * content;
    size_t size;
};


int writeFile(const char * path, const RAW file_raw){

    FILE * fd = fopen(path, "wb"); // File descriptor in write binary mode

    if (fd == 0){ // if opening failed for x reason
        perror("Error : writing file");
        return -1;
    }

    // writing
    fwrite(file_raw.content, sizeof(unsigned char), file_raw.size, fd);

    return 0;
}

int loadFile(const char* path, RAW& file){

    FILE * fd = fopen(path, "rb"); // reading in binary mode

    if (fd == 0){
        perror("Error : reading file");
        return -1;
    }
    // seek the size
    fseek(fd, 0, SEEK_END);
    file.size = ftell(fd);

    // replace at begining and read content after malloc
    fseek(fd, 0, SEEK_SET);
    file.content = (unsigned char*)malloc(file.size); // TODO should check if ok
    if (file.content == 0){
        perror("Error : malloc failure");
        return -1;
    }
    fread(file.content, sizeof(unsigned char), file.size, fd);

    return 0;
}





int main(const int argc, const char* const argv[]) {

    int errorCode; // error

    // file path
    char inpath[] = "test.raw";
    char outpath[] = "test_out.raw";


    // image threshold for black OR white
    unsigned char threshold = 64;

    // data struct, we assign memory
    RAW file;

    // IN
    errorCode = loadFile(inpath, file);
    if (errorCode != 0)  // failure
        return 1; // leave on failure

    // OUT
    errorCode = writeFile(outpath, file);
    if (errorCode != 0) // failure
        return 1; // leave on failure
    free(file.content); // we free memory



    return 0;
}