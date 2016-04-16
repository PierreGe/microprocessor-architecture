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




void applyMinTransfoInC(const RAW data, const unsigned char threshold){


}

void applyMaxTransfoInC(const RAW data, const unsigned char threshold){


}

void applyMinTransfoInSIM(const RAW file, const unsigned char threshold){


}

void applyMaxTransfoInSIM(const RAW file, const unsigned char threshold){


}




int main() {

    // time var
    time_t start_time, end_time ;
    float dt ;

    // file path
    const char inpath[] = "test.raw";
    const char outpathMinC[] = "test_min_c.raw";
    const char outpathMaxC[] = "test_max_c.raw";
    const char outpathMinASM[] = "test_min_simd.raw";
    const char outpathMaxASM[] = "test_max_simd.raw";

    // image threshold for black OR white
    const unsigned char threshold = 54;


    int errorCode; // error

    // ---- C  min ----

    // data struct, we assign memory
    RAW rawMinDataC;

    // IN
    errorCode = loadFile(inpath, rawMinDataC);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMinTransfoInC(rawMinDataC,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C (min) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMinC, rawMinDataC);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    // ---- C  max ----

    // data struct, we assign memory
    RAW rawDataMaxC;

    // IN
    errorCode = loadFile(inpath, rawDataMaxC);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMaxTransfoInC(rawDataMaxC,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C (max) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMaxC, rawDataMaxC);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    // ---- ASM min ----


    RAW rawMinDataASM;
    // IN
    errorCode = loadFile(inpath, rawMinDataASM);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMinTransfoInC(rawMinDataASM,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD (min) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMinASM, rawMinDataASM);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    // ---- ASM max ----


    RAW rawMaxDataASM;
    // IN
    errorCode = loadFile(inpath, rawMaxDataASM);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMaxTransfoInC(rawMaxDataASM,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD (max) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMaxASM, rawMaxDataASM);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    return 0;
}