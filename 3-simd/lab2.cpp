#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Pierre Gerard, Julian Schrembri, Francois Schiltz


// what? no min max in C ? no problem
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

// Struct that contains the data
struct RAW {
    unsigned char * content; // unisgned char -> a raw byte
    size_t size, width;
};


int loadFile(const char* fileName, RAW& rawData, size_t defwidth){

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

    rawData.width = defwidth;

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

    return 1;
}




void applyMinTransfoInC(const RAW data, RAW res){

    unsigned int m = 255;
    for (size_t w = 0; w < data.width ; ++w) {
        for (size_t h = 0; h < (data.size / data.width); ++h) {
            // center
            m = min(m, data.content[w * data.width + h]);
            // left
            if (w-1 > 0){
                m = min(m, data.content[(w-1) * data.width + (h)]);
            }
            // right
            if (w+1 < data.width){
                m = min(m, data.content[(w+1) * data.width + (h)]);
            }
            // bottom
            if (h-1 > 0){
                m = min(m, data.content[(w) * data.width + (h-1)]);
            }
            // top
            if (h+1 < (data.size / data.width)){
                m = min(m, data.content[(w) * data.width + (h+1)]);
            }
            // bottom left
            if (h-1 > 0 and w-1 > 0){
                m = min(m, data.content[(w-1) * data.width + (h-1)]);
            }
            // bottom right
            if (w+1 < data.width and h-1 > 0){
                m = min(m, data.content[(w+1) * data.width + (h-1)]);
            }
            // top left
            if (h+1 < (data.size / data.width) and w-1 > 0){
                m = min(m, data.content[(w-1) * data.width + (h+1)]);
            }
            // top right
            if (h+1 < (data.size / data.width) and w+1 < data.width){
                m = min(m, data.content[(w+1) * data.width + (h+1)]);
            }
            // set the value
            res.content[w * data.width + h] = m;
            m = 255;

        }
    }
}

void applyMaxTransfoInC(const RAW data, RAW res){
    unsigned int m = 0;
    for (size_t w = 0; w < data.width ; ++w) {
        for (size_t h = 0; h < (data.size / data.width); ++h) {
            // center
            m = max(m, data.content[w * data.width + h]);
            // left
            if (w-1 > 0){
                m = max(m, data.content[(w-1) * data.width + (h)]);
            }
            // right
            if (w+1 < data.width){
                m = max(m, data.content[(w+1) * data.width + (h)]);
            }
            // bottom
            if (h-1 > 0){
                m = max(m, data.content[(w) * data.width + (h-1)]);
            }
            // top
            if (h+1 < (data.size / data.width)){
                m = max(m, data.content[(w) * data.width + (h+1)]);
            }
            // bottom left
            if (h-1 > 0 and w-1 > 0){
                m = max(m, data.content[(w-1) * data.width + (h-1)]);
            }
            // bottom right
            if (w+1 < data.width and h-1 > 0){
                m = max(m, data.content[(w+1) * data.width + (h-1)]);
            }
            // top left
            if (h+1 < (data.size / data.width) and w-1 > 0){
                m = max(m, data.content[(w-1) * data.width + (h+1)]);
            }
            // top right
            if (h+1 < (data.size / data.width) and w+1 < data.width){
                m = max(m, data.content[(w+1) * data.width + (h+1)]);
            }
            // set the value
            res.content[w * data.width + h] = m;
            m = 0;

        }
    }

}

void applyMinTransfoInSIM(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 16;
    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l1:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"


            "movdqu %%xmm0,(%%edi)\n\t;"
            "add $14,%%esi\n\t;"
            "add $14,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l1\n\t;"
        : "=m" (outputEntryPoint)/* output operands */
        : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
        : "%esi",  "%xmm1", "%xmm2", "%ecx", "%edi", "%xmm0"/* clobbered operands */
    );

}

void applyMaxTransfoInSIM(const RAW data, RAW res){


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

    int errorCode; // error

    RAW dataOut;

    // ---- C  min ----

    // data struct, we assign memory
    RAW rawMinDataC;


    // IN
    errorCode = loadFile(inpath, rawMinDataC, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    dataOut.content = (unsigned char*)malloc(rawMinDataC.size);
    dataOut.size = rawMinDataC.size;

    start_time = clock ();
    applyMinTransfoInC(rawMinDataC,dataOut);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C (min) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMinC, dataOut);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    // we free memory back
    free(rawMinDataC.content);


    // ---- C  max ----

    // data struct, we assign memory
    RAW rawDataMaxC;

    // IN
    errorCode = loadFile(inpath, rawDataMaxC, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMaxTransfoInC(rawDataMaxC,dataOut);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C (max) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMaxC, dataOut);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    free(rawDataMaxC.content);
    // ---- ASM min ----


    RAW rawMinDataASM;
    // IN
    errorCode = loadFile(inpath, rawMinDataASM, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMinTransfoInC(rawMinDataASM, dataOut);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD (min) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMinASM, dataOut);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    free(rawMinDataASM.content);
    // ---- ASM max ----


    RAW rawMaxDataASM;
    // IN
    errorCode = loadFile(inpath, rawMaxDataASM, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    start_time = clock ();
    applyMaxTransfoInC(rawMaxDataASM, dataOut);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in SIMD (max) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMaxASM, dataOut);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    free(rawMaxDataASM.content);
    free(dataOut.content);

    return 0;
}