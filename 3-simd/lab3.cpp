#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Pierre Gerard, Julian Schrembri, Francois Schiltz



// Struct that contains the data
struct RAW {
    unsigned char * content; // unisgned char -> a raw byte
    size_t size;
};


int loadFile(const char* fileName, RAW& rawData){

    FILE * fd = fopen(fileName, "rb"); // reading in binary mode

    if (fd == 0){
        perror("Error : reading rawData");
        return -1;
    }
    // seek the size
    fseek(fd, 0, SEEK_END);
    rawData.size = ftell(fd);

    // replace at begining and read content after malloc
    fseek(fd, 0, SEEK_SET);
    rawData.content = (unsigned char*)malloc(rawData.size);
    if (rawData.content == 0){
        perror("Error : malloc failure");
        return -1;
    }
    fread(rawData.content, sizeof(unsigned char), rawData.size, fd);

    return 1;
}

int writeFile(const char * path, const RAW rawData){

    FILE * fd = fopen(path, "wb"); // File descriptor in write binary mode

    if (fd == 0){ // if opening failed for x reason
        perror("Error : writing file");
        return -1;
    }

    // writing
    fwrite(rawData.content, sizeof(unsigned char), rawData.size, fd);

    // we free memory back
    free(rawData.content);

    return 1;
}




void applyTransfoInC(const RAW data, const unsigned char threshold){


}

void applyTransfoInSIM(const RAW file, const unsigned char threshold){

}




int main() {


    // time var
    time_t start_time, end_time ;
    float dt ;

    // file path
    const char inpath[] = "test.raw";
    const char outpathC[] = "test_out_c.raw";
    const char outpathASM[] = "test_out_simd.raw";

    // image threshold for black OR white
    const unsigned char threshold = 54;


    int errorCode; // error

    // ---- C ----

    // data struct, we assign memory
    RAW rawDataC;

    // IN
    errorCode = loadFile(inpath, rawDataC);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyTransfoInC(rawDataC,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathC, rawDataC);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    // ---- ASM ----


    RAW rawDataASM;
    // IN
    errorCode = loadFile(inpath, rawDataASM);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyTransfoInC(rawDataASM,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathASM, rawDataASM);
    if (errorCode != 1) // failure
        return 1; // leave on failure



    return 0;
}