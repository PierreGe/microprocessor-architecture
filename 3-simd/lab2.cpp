#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

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




inline void applyMinTransfoInC(const RAW data, RAW res, int width){
    unsigned int m = 255;
    for (size_t w = 0; w < data.width ; ++w) {
        for (size_t h = 0; h < (data.size / data.width); ++h) {
            int t = -(width/2);
            for(int i = 0; i < width; ++i){
                for(int j = 0; j < width; ++j)
                {
                    if((w + t + i)*data.width + (h + t + j) >= 0 && (w + t + i)*data.width + (h + t + j) < data.size)
                        m = min(m,data.content[(w + t + i)*data.width + (h + t + j)]);  
                } 
            }
            // set the value
            res.content[w * data.width + h] = m;
            m = 255;
        }
    }
}

inline void applyMaxTransfoInC(const RAW data, RAW res, int width){
    unsigned int m = 0;
    for (size_t w = 0; w < data.width ; ++w) {
        for (size_t h = 0; h < (data.size / data.width); ++h) {
            int t = -(width/2);
            for(int i = 0; i < width; ++i){
                for(int j = 0; j < width; ++j)
                {
                    if((w + t + i)*data.width + (h + t + j) >= 0 && (w + t + i)*data.width + (h + t + j) < data.size)
                        m = max(m,data.content[(w + t + i)*data.width + (h + t + j)]);  
                } 
            }
        }
    }
}

inline void applyMinTransfoInSIM(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 14;

    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l2:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"

            "pminub %%xmm1,%%xmm0\n\t;"
            "pminub %%xmm2,%%xmm0\n\t;"
 
            "vpsrldq $1,%%xmm0,%%xmm6\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm7\n\t;"
            "pminub %%xmm7,%%xmm6\n\t;"
            "pminub %%xmm0,%%xmm6\n\t;"

            "movdqu %%xmm6,(%%edi)\n\t;"
            "add $14,%%esi\n\t;"
            "add $14,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l2\n\t;"
    "emms\n\t;" //clean
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2", "%xmm6", "%xmm7"/* clobbered operands */
    );

}

inline void applyMinTransfoInSIMby5(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 12;
    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l3:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"
            "movdqu 3072(%%esi),%%xmm3\n\t;"
            "movdqu 4096(%%esi),%%xmm4\n\t;"
            

            "pminub %%xmm1,%%xmm0\n\t;"
            "pminub %%xmm2,%%xmm0\n\t;"
            "pminub %%xmm3,%%xmm0\n\t;"
            "pminub %%xmm4,%%xmm0\n\t;"
            
            "vpsrldq $1,%%xmm0,%%xmm4\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm5\n\t;"
            "vpsrldq $3,%%xmm0,%%xmm6\n\t;"
            "vpsrldq $4,%%xmm0,%%xmm7\n\t;"
            
            "pminub %%xmm5,%%xmm4\n\t;"
            "pminub %%xmm6,%%xmm4\n\t;"
            "pminub %%xmm7,%%xmm4\n\t;"
            "pminub %%xmm0,%%xmm4\n\t;"

            "movdqu %%xmm4,(%%edi)\n\t;"
            "add $12,%%esi\n\t;"
            "add $12,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l3\n\t;"
        "emms\n\t;" //clean
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2","%xmm3","%xmm4","%xmm5", "%xmm6", "%xmm7"/* clobbered operands */
    );
}

inline void applyMinTransfoInSIMby7(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 10;
    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l4:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"
            "movdqu 3072(%%esi),%%xmm3\n\t;"
            "movdqu 4096(%%esi),%%xmm4\n\t;"
            "movdqu 5120(%%esi),%%xmm5\n\t;"
            "movdqu 6144(%%esi),%%xmm6\n\t;"
            

            "pminub %%xmm1,%%xmm0\n\t;"
            "pminub %%xmm2,%%xmm0\n\t;"
            "pminub %%xmm3,%%xmm0\n\t;"
            "pminub %%xmm4,%%xmm0\n\t;"
            
            "vpsrldq $1,%%xmm0,%%xmm1\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm2\n\t;"
            "vpsrldq $3,%%xmm0,%%xmm3\n\t;"
            "vpsrldq $4,%%xmm0,%%xmm4\n\t;"
            "vpsrldq $5,%%xmm0,%%xmm5\n\t;"
            "vpsrldq $6,%%xmm0,%%xmm6\n\t;"
            
            
            "pminub %%xmm0,%%xmm6\n\t;"
            "pminub %%xmm1,%%xmm6\n\t;"
            "pminub %%xmm2,%%xmm6\n\t;"
            "pminub %%xmm3,%%xmm6\n\t;"
            "pminub %%xmm4,%%xmm6\n\t;"
            "pminub %%xmm5,%%xmm6\n\t;"

            "movdqu %%xmm6,(%%edi)\n\t;"
            "add $10,%%esi\n\t;"
            "add $10,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l4\n\t;"
        "emms\n\t;" //clean
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2","%xmm3","%xmm4","%xmm5", "%xmm6", "%xmm7"/* clobbered operands */
    );
}

inline void applyMaxTransfoInSIM(const RAW data, RAW res){
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

            "pmaxub %%xmm1,%%xmm0\n\t;"
            "pmaxub %%xmm2,%%xmm0\n\t;"

            "vpsrldq $1,%%xmm0,%%xmm3\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm4\n\t;"
            
            "pmaxub %%xmm4,%%xmm3\n\t;"
            "pmaxub %%xmm0,%%xmm3\n\t;"

            "movdqu %%xmm3,(%%edi)\n\t;"
            "add $14,%%esi\n\t;"
            "add $14,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l1\n\t;"
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4"/* clobbered operands */
    );

}

inline void applyMaxTransfoInSIMby5(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 12;
    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l5:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"
            "movdqu 3072(%%esi),%%xmm3\n\t;"
            "movdqu 4096(%%esi),%%xmm4\n\t;"
            

            "pmaxub %%xmm1,%%xmm0\n\t;"
            "pmaxub %%xmm2,%%xmm0\n\t;"
            "pmaxub %%xmm3,%%xmm0\n\t;"
            "pmaxub %%xmm4,%%xmm0\n\t;"
            
            "vpsrldq $1,%%xmm0,%%xmm1\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm2\n\t;"
            "vpsrldq $3,%%xmm0,%%xmm3\n\t;"
            "vpsrldq $4,%%xmm0,%%xmm4\n\t;"
            
            "pmaxub %%xmm1,%%xmm4\n\t;"
            "pmaxub %%xmm2,%%xmm4\n\t;"
            "pmaxub %%xmm3,%%xmm4\n\t;"
            "pmaxub %%xmm0,%%xmm4\n\t;"

            "movdqu %%xmm4,(%%edi)\n\t;"
            "add $12,%%esi\n\t;"
            "add $12,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l5\n\t;"
        "emms\n\t;" //clean
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2","%xmm3","%xmm4"/* clobbered operands */
    );
}

inline void applyMaxTransfoInSIMby7(const RAW data, RAW res){
    unsigned char * dataEntryPoint = &data.content[0];
    unsigned char * outputEntryPoint = &res.content[0];
    size_t nbr16Bblocks = data.size / 10;
    __asm__ (
        "movl %2,%%esi\n\t;"
        "movl %1,%%ecx\n\t;"
        "movl %0,%%edi\n\t;"
        "l6:"
            "movdqu (%%esi),%%xmm0\n\t;"
            "movdqu 1024(%%esi),%%xmm1\n\t;"
            "movdqu 2048(%%esi),%%xmm2\n\t;"
            "movdqu 3072(%%esi),%%xmm3\n\t;"
            "movdqu 4096(%%esi),%%xmm4\n\t;"
            "movdqu 5120(%%esi),%%xmm5\n\t;"
            "movdqu 6144(%%esi),%%xmm6\n\t;"
            
            "pmaxub %%xmm1,%%xmm0\n\t;"
            "pmaxub %%xmm2,%%xmm0\n\t;"
            "pmaxub %%xmm3,%%xmm0\n\t;"
            "pmaxub %%xmm4,%%xmm0\n\t;"
            
            "vpsrldq $1,%%xmm0,%%xmm1\n\t;"
            "vpsrldq $2,%%xmm0,%%xmm2\n\t;"
            "vpsrldq $3,%%xmm0,%%xmm3\n\t;"
            "vpsrldq $4,%%xmm0,%%xmm4\n\t;"
            "vpsrldq $5,%%xmm0,%%xmm5\n\t;"
            "vpsrldq $6,%%xmm0,%%xmm6\n\t;"
            
            
            "pmaxub %%xmm0,%%xmm6\n\t;"
            "pmaxub %%xmm1,%%xmm6\n\t;"
            "pmaxub %%xmm2,%%xmm6\n\t;"
            "pmaxub %%xmm3,%%xmm6\n\t;"
            "pmaxub %%xmm4,%%xmm6\n\t;"
            "pmaxub %%xmm5,%%xmm6\n\t;"

            "movdqu %%xmm6,(%%edi)\n\t;"
            "add $10,%%esi\n\t;"
            "add $10,%%edi\n\t;"
            "sub $1,%%ecx\n\t;"
            "jnz l6\n\t;"
        "emms\n\t;" //clean
    : "=m" (outputEntryPoint)/* output operands */
    : "g" (nbr16Bblocks), "m" (dataEntryPoint) /* input operands */
    : "%esi",  "%ecx", "%edi", "%xmm0", "%xmm1", "%xmm2","%xmm3","%xmm4","%xmm5", "%xmm6"/* clobbered operands */
    );
}


int main() {

    // time var
    time_t start_time, end_time ;
    float dt ;
    unsigned int box = 7;
    // file path
    const char inpath[] = "test.raw";
    const char outpathMinC[] = "test_min_c.raw";
    const char outpathMaxC[] = "test_max_c.raw";
    const char outpathMinASM[] = "test_min_simd.raw";
    const char outpathMaxASM[] = "test_max_simd.raw";

    int errorCode; // error

    RAW dataOutC;

    // ---- C  min ----

    // data struct, we assign memory
    RAW rawMinDataC;


    // IN
    errorCode = loadFile(inpath, rawMinDataC, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    dataOutC.content = (unsigned char*)malloc(rawMinDataC.size);
    dataOutC.size = rawMinDataC.size;

    //*
    dataOutC.width = rawMinDataC.width;
    RAW temp;
    temp.content = (unsigned char*)malloc(rawMinDataC.size);
    temp.size = rawMinDataC.size;
    //*/
    
    start_time = clock ();
    applyMinTransfoInC(rawMinDataC,dataOutC,box);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    
    /*
     for(int i = 0; i < 5; i++){
        applyMinTransfoInC(dataOutC,temp,box);
        for(int j = 0; j < temp.size; j++){
           dataOutC.content[j] = temp.content[j]; 
        }
    }
    //*/

    printf("Time needed in C (min) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMinC, dataOutC);
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
    applyMaxTransfoInC(rawDataMaxC,dataOutC,box);
    end_time = clock ();
    dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;

    printf("Time needed in C (max) : %.6f \n", dt);

    // OUT
    errorCode = writeFile(outpathMaxC, dataOutC);
    if (errorCode != 1) // failure
        return 1; // leave on failure


    free(rawDataMaxC.content);
    free(dataOutC.content);
    // ---- ASM min ----

    RAW dataOutSIMD;
    dataOutSIMD.content = (unsigned char*)malloc(rawMinDataC.size);
    dataOutSIMD.size = rawMinDataC.size;
    
    //*
    dataOutSIMD.width = rawMinDataC.width;

    temp.content = (unsigned char*)malloc(rawMinDataC.size);
    temp.size = rawMinDataC.size;
    //*/

    RAW rawMinDataASM;
    // IN
    errorCode = loadFile(inpath, rawMinDataASM, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    if(box == 5){
        start_time = clock ();
        applyMinTransfoInSIMby5(rawMinDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    }
    else if(box == 7){
        start_time = clock ();
        applyMinTransfoInSIMby7(rawMinDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    }
    else{
        start_time = clock ();
        applyMinTransfoInSIM(rawMinDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
        
        /*
        for(int i = 0; i < 5; i++){
            applyMinTransfoInSIM(dataOutSIMD,temp);
            for(int j = 0; j < temp.size; j++){
                dataOutSIMD.content[j] = temp.content[j]; 
            }
        }
        //*/
    }

    printf("Time needed in SIMD (min) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMinASM, dataOutSIMD);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    free(rawMinDataASM.content);
    // ---- ASM max ----


    RAW rawMaxDataASM;
    // IN
    errorCode = loadFile(inpath, rawMaxDataASM, 1024);
    if (errorCode != 1)  // failure
        return 1; // leave on failure

    if(box == 5){
        start_time = clock ();
        applyMaxTransfoInSIMby5(rawMinDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    }
    else if(box == 7){
        start_time = clock ();
        applyMaxTransfoInSIMby7(rawMinDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    }
    else{
        start_time = clock ();
        applyMaxTransfoInSIM(rawMaxDataASM, dataOutSIMD);
        end_time = clock ();
        dt = (end_time-start_time)/(float)(CLOCKS_PER_SEC) ;
    }

    printf("Time needed in SIMD (max) : %.6f \n", dt);


    // OUT
    errorCode = writeFile(outpathMaxASM, dataOutSIMD);
    if (errorCode != 1) // failure
        return 1; // leave on failure

    free(rawMaxDataASM.content);
    free(dataOutSIMD.content);

    return 0;
}