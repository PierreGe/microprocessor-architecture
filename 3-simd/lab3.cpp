#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Pierre Gerard, Julian Schrembri, Francois Schiltz


#define NUMBER_OF_THREAD 2


// Struct that contains the data
struct RAW {
    unsigned char * content; // unisgned char -> a raw byte
    size_t size;
};

struct ThresoldThreadData {
    RAW * content; // unisgned char -> a raw byte
    size_t begin;
    size_t end;
    unsigned char thresold;
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

void *threadFuncThresC(void *arg){
    ThresoldThreadData *dt;
    dt=(ThresoldThreadData*)arg;
    for (size_t i = 0; i < dt->content->size; ++i){
        if (dt->content->content[i] < dt->thresold){
            dt->content->content[i] = 0;
        }
        else{
            dt->content->content[i] = 255;
        }
    }
}


void applyTransfoInCMulti(RAW data, const unsigned char threshold){
    for (int i = 0; i < NUMBER_OF_THREAD; ++i) {
        ThresoldThreadData *threadData = NULL;
        threadData = (struct ThresoldThreadData*)malloc(sizeof(struct ThresoldThreadData));
        (*threadData).content = &data;
        (*threadData).thresold = threshold;
        (*threadData).begin = i * (data.size/NUMBER_OF_THREAD);
        (*threadData).end = (i+1) * (data.size/NUMBER_OF_THREAD);
        pthread_t pth;
        /* Create worker thread */
        pthread_create(&pth,NULL,threadFuncThresC,(void*)threadData);
        /* wait for our thread to finish before continuing */
        pthread_join(pth, NULL);
    }




}


void applyTransfoInSIM(const RAW data, const unsigned char threshold){
    // preprocess for ASM
    unsigned char compare[16];
    for (size_t i = 0; i < 16; ++i){
        compare[i] = threshold+127;
    }
    size_t nbr16Bblocks = data.size / 16;
    uint64_t add128[2];
    add128[0] = 0x8080808080808080;
    add128[1] = 0x8080808080808080;
    // define entry point to pass to ASM
    uint64_t* add128entrypoint = &add128[0];
    unsigned char* compareEntryPoint = &compare[0];
    unsigned char * dataEntryPoint = &data.content[0];
    // ASM : ATT syntax
    // GCC compile in 32 bits
    __asm__ (
    "movl %1, %%esi\n\t;" //comp
            "movdqu (%%esi),%%xmm1\n\t;"
            "movl %2, %%esi\n\t;" // read sign
            "movdqu (%%esi), %%xmm2\n\t;"
            "movl %3, %%ecx\n\t;" //l
            "mov %4, %%esi\n\t;" // ptr
            "label1:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "paddb %%xmm2,%%xmm0\n\t;"
            "pcmpgtb %%xmm1,%%xmm0\n\t;"
            "movdqu %%xmm0,(%%esi)\n\t;"
            "addl $16, %%esi\n\t;"
            "subl $1, %%ecx\n\t;"
            "jnz label1\n\t;"
            "emms\n\t;" //clean
    : "=m" (dataEntryPoint)/* output operands */
    : "m" (compareEntryPoint), "r" (add128entrypoint), "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%xmm1", "%xmm2", "%ecx", "%xmm0"/* clobbered operands */
    );
}

void applyTransfoInSIMMulti(const RAW data, const unsigned char threshold){

}


int mainThresold() {
    // time var
    time_t start_time, end_time ;
    float dt ;
    // file path
    const char inpath[] = "test.raw";
    const char outpathMonoC[] = "test_mono_threshold_c.raw";
    const char outpathMultiC[] = "test_multi_threshold_c.raw";
    const char outpathMonoASM[] = "test_mono_threshold_simd.raw";
    const char outpathMultiASM[] = "test_multi_threshold_simd.raw";
    // image threshold for black OR white
    const unsigned char threshold = 54;
    int errorCode; // error

    // ---- C mono----
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
    printf("Time needed in C (mono-thread): %.6f \n", dt);
    errorCode = writeFile(outpathMonoC, rawDataC); // OUT
    if (errorCode != 1) // failure
        return 1; // leave on failure

    // ---- C multi----
    // data struct, we assign memory
    RAW rawDataCMulti;
    // IN
    errorCode = loadFile(inpath, rawDataCMulti);
    if (errorCode != 1)  // failure
        return 1; // leave on failure
    start_time = clock ();
    applyTransfoInCMulti(rawDataCMulti,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    printf("Time needed in C (multi-thread): %.6f \n", dt);
    errorCode = writeFile(outpathMultiC, rawDataCMulti); // OUT
    if (errorCode != 1) // failure
        return 1; // leave on failure

    // ---- ASM mono ----
    RAW rawDataASM;
    // IN
    errorCode = loadFile(inpath, rawDataASM);
    if (errorCode != 1)  // failure
        return 1; // leave on failure
    start_time = clock ();
    applyTransfoInSIM(rawDataASM,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    printf("Time needed in SIMD (mono-thread): %.6f \n", dt);
    // OUT
    errorCode = writeFile(outpathMonoASM, rawDataASM);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    // ---- ASM multi ----
    RAW rawDataASMMulti;
    // IN
    errorCode = loadFile(inpath, rawDataASMMulti);
    if (errorCode != 1)  // failure
        return 1; // leave on failure
    start_time = clock ();
    applyTransfoInSIMMulti(rawDataASMMulti,threshold);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    printf("Time needed in SIMD (mono-thread): %.6f \n", dt);
    // OUT
    errorCode = writeFile(outpathMultiASM, rawDataASMMulti);
    if (errorCode != 1) // failure
        return 1; // leave on failure
    return 0;
}

int mainMinMax() {

    return 0;
}


int main() {
    short e;
    e = mainThresold();
    if (e != 0){
        return 1; // leave on failure
    }
    e = mainMinMax();
    return e;
}

