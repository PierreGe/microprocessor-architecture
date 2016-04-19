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
    for (size_t i = 0; i < data.size; ++i){
        if (data.content[i] < threshold){
            data.content[i] = 0;
        }
        else{
            data.content[i] = 255;
        }
    }

}

void applyTransfoInSIM(const RAW file, const unsigned char threshold){
    unsigned char comp[16];
    for (size_t i = 0; i < 16; ++i)
        comp[i] = threshold;
    unsigned char* _comp = &comp[0];

    uint64_t sign[2];
    sign[0] = 0x8080808080808080;
    sign[1] = 0x8080808080808080;
    uint64_t* _sign = &sign[0];

    size_t l = file.size / 16;
    unsigned char * _ptr = &file.content[0];

    __asm__ (
            "movl %1, %%esi\n\t;" //comp
            "movdqu (%%esi),%%xmm1\n\t;"
            "movl %2, %%esi\n\t;" // read sign
            "movdqu (%%esi), %%xmm2\n\t;"
            "paddb %%xmm1,%%xmm2\n\t;"
            "movl %3, %%ecx\n\t;" //l
            "mov %4, %%esi\n\t;" // ptr
             "label1:"
                    "movdqu (%%esi),%%xmm0\n\t;"

                    "paddb %%xmm0,%%xmm2\n\t;"

                    "pcmpgtb %%xmm0,%%xmm1\n\t;"


                    "movdqu %%xmm1,(%%esi)\n\t;"
                    //"paddb %%xmm0,%%xmm2\n\t;" // Add Packed Integers
                    //"pcmpgtb %%xmm0,%%xmm1\n\t;" // Compare Packed Signed Integers for Greater Than

                    "addl $16, %%esi\n\t;"
                    "subl $1, %%ecx\n\t;"
                    "jnz label1\n\t;"
            "emms\n\t;" //clean

            : "=m" (_ptr)/* output operands */
            : "m" (_comp), "r" (_sign), "g" (l), "m" (_ptr) /* input operands */
            : "%esi",  "%xmm1", "%xmm2", "%ecx", "%xmm0"/* clobbered operands */
   );

}




int main() {

    // time var
    time_t start_time, end_time ;
    float dt ;

    // file path
    const char inpath[] = "test.raw";
    const char outpathC[] = "test_threshold_c.raw";
    const char outpathASM[] = "test_threshold_simd.raw";

    // image threshold for black OR white
    const unsigned char threshold = 128;


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
    applyTransfoInSIM(rawDataASM,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathASM, rawDataASM);
    if (errorCode != 1) // failure
        return 1; // leave on failure



    return 0;
}