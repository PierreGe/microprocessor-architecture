
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

    // we free memory back
    free(file_raw.content);

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
    file.content = (unsigned char*)malloc(file.size);
    if (file.content == 0){
        perror("Error : malloc failure");
        return -1;
    }
    fread(file.content, sizeof(unsigned char), file.size, fd);

    return 0;
}




void applyTransfoInC(const RAW file, const unsigned char threshold){
    for (size_t i = 0; i < file.size; ++i){
        if (file.content[i] < threshold){
            file.content[i] = 0;
        }
        else{
            file.content[i] = 255;
        }
    }

}

void applyTransfoInSIM(const RAW file, const unsigned char threshold){

    __asm(
            "nop"

    );


}




int main() {

    // time var
    time_t start_time, end_time ;
    float dt ;

    // file path
    const char inpath[] = "test.raw";
    const char outpath[] = "test_out.raw";

    // image threshold for black OR white
    const unsigned char threshold = 70;


    int errorCode; // error

    // data struct, we assign memory
    RAW file;

    // IN
    errorCode = loadFile(inpath, file);
    if (errorCode != 0)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyTransfoInC(file,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in c %.6f \n", dt);



    // OUT
    errorCode = writeFile(outpath, file);
    if (errorCode != 0) // failure
        return 1; // leave on failure



    return 0;
}