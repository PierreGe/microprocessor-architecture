#include "DataTypes.h"
#include "p33FJ256GP710.h"

void main(void){
    // var defined in the 
    const INT8U N=128;
    volatile INT8U first_sample; // volatile because it is externaly modified and so it tells the compiler not to do optimisation
    INT8U n=first_sample; //define just above
    INT8U k=0; 

    // Under this line we assign some random value to EN just to see if it compile and run
    // normaly it is defined some place else
    volatile INT8S input[] = {0x24, 0x35, 0x13, 0x66, 0x56, 0x5a, 0x6c, 0x3f, 0x14, 0x46, 0x2c, 0x51, 0x7f, 0x39, 0x4e, 0x55, 0x73, 0x24, 0x4b, 0x43, 0x2e, 0x58, 0x64, 0x65, 0x17, 0x7c, 0x74, 0x39, 0x39, 0x03, 0x10, 0x7d, 0x1b, 0x22, 0x23, 0x07, 0x3b, 0x12, 0x24, 0x2f, 0x54, 0x2a, 0x3a, 0x61, 0x66, 0x24, 0x62, 0x25, 0x25, 0x0b, 0x09, 0x31, 0x21, 0x54, 0x36, 0x6d, 0x13, 0x11, 0x79, 0x29, 0x0f, 0x4b, 0x7b, 0x1c, 0x2c, 0x59, 0x63, 0x55, 0x1d, 0x41, 0x31, 0x69, 0x5c, 0x5d, 0x63, 0x49, 0x71, 0x79, 0x6b, 0x78, 0x2f, 0x0b, 0x1d, 0x73, 0x4a, 0x18, 0x4d, 0x23, 0x79, 0x5d, 0x12, 0x45, 0x2f, 0x4d, 0x46, 0x6a, 0x05, 0x05, 0x5d, 0x72, 0x66, 0x7f, 0x44, 0x68, 0x22, 0x0c, 0x14, 0x1c, 0x65, 0x07, 0x4b, 0x4d, 0x6d, 0x43, 0x0b, 0x79, 0x2a, 0x56, 0x1a, 0x6b, 0x7f, 0x0a, 0x5c, 0x00, 0x2e, 0x1b, 0x7b, 0x64 };
    // volatile here for the same reason
    const INT8S en_r[] = {0x32, 0x4a, 0x35, 0x41, 0x52, 0x26, 0x38, 0x44, 0x39, 0x22, 0x53, 0x30, 0x80, 0x6f, 0x29, 0x46, 0x5d, 0x0c, 0x5d, 0x7d, 0x1e, 0x7b, 0x70, 0x45, 0x52, 0x2f, 0x5f, 0x53, 0x48, 0x5c, 0x2e, 0x60, 0x48, 0x0c, 0x1c, 0x07, 0x3d, 0x69, 0x0a, 0x5c, 0x7c, 0x47, 0x6b, 0x13, 0x75, 0x13, 0x42, 0x15, 0x7e, 0x2c, 0x2f, 0x54, 0x6c, 0x05, 0x06, 0x05, 0x4a, 0x30, 0x54, 0x72, 0x7d, 0x20, 0x36, 0x7a, 0x25, 0x3a, 0x48, 0x35, 0x1d, 0x5c, 0x34, 0x20, 0x47, 0x31, 0x02, 0x4f, 0x20, 0x24, 0x70, 0x41, 0x57, 0x80, 0x41, 0x30, 0x0f, 0x53, 0x2d, 0x3e, 0x11, 0x1e, 0x1b, 0x0b, 0x4f, 0x0e, 0x4d, 0x03, 0x16, 0x7a, 0x5e, 0x3a, 0x0b, 0x7f, 0x75, 0x5d, 0x53, 0x24, 0x12, 0x3d, 0x5e, 0x61, 0x48, 0x2e, 0x68, 0x3b, 0x76, 0x79, 0x19, 0x06, 0x76, 0x23, 0x79, 0x08, 0x0b, 0x00, 0x69, 0x72, 0x07, 0x61 };
    const INT8S en_i[] = {0x5c, 0x7d, 0x25, 0x46, 0x66, 0x22, 0x4f, 0x5f, 0x6d, 0x80, 0x7d, 0x69, 0x6a, 0x58, 0x40, 0x48, 0x1b, 0x49, 0x6a, 0x05, 0x37, 0x10, 0x43, 0x65, 0x75, 0x80, 0x16, 0x57, 0x4a, 0x3f, 0x35, 0x41, 0x4a, 0x69, 0x1f, 0x68, 0x26, 0x79, 0x18, 0x36, 0x6f, 0x43, 0x7f, 0x28, 0x33, 0x70, 0x0d, 0x78, 0x56, 0x6c, 0x07, 0x44, 0x0e, 0x1b, 0x11, 0x37, 0x49, 0x39, 0x72, 0x45, 0x7f, 0x16, 0x54, 0x6d, 0x1b, 0x48, 0x0e, 0x1a, 0x80, 0x21, 0x71, 0x45, 0x1a, 0x5c, 0x25, 0x12, 0x2b, 0x16, 0x10, 0x08, 0x10, 0x08, 0x24, 0x37, 0x59, 0x31, 0x49, 0x66, 0x24, 0x1e, 0x19, 0x4c, 0x80, 0x1a, 0x4a, 0x34, 0x42, 0x08, 0x02, 0x79, 0x05, 0x5a, 0x07, 0x0a, 0x66, 0x43, 0x66, 0x2b, 0x63, 0x07, 0x66, 0x6b, 0x6b, 0x7c, 0x3a, 0x69, 0x00, 0x47, 0x3a, 0x52, 0x60, 0x0c, 0x3b, 0x14, 0x5e, 0x52, 0x64, 0x58 };
    
    INT8S output_r[N];
    INT8S output_i[N];

    // temporary var for the computation
    INT8U m;
    INT16U unsigned16temp;
    INT16S signed16temp_r;
    INT16S signed16temp_i;
    INT8S signed8temp;

    // algorithme
    for (k = 0; k < N; k = k+1){
    	// we make sure that the case of the array are define to zero
        output_r[k] = 0;
        output_i[k] = 0;
        for (n = first_sample; n != first_sample-1; n= (n+1) % N){

            // operation pour obtenir l'indice dans l'array EN_r et EN_i
            unsigned16temp = (k * n); // INT16 = INT8 * INT8 TODO cast avant ?
            m = (INT8U) (unsigned16temp / (INT16U) N); // m is rounded to the nearest integer here, if inputs are correct it should be castable to int8

            // computation for real part
            signed16temp_r = (INT16S)input[n] * (INT16S) en_r[m];
            signed8temp = signed16temp_r >>8; // we shift the value and loose some accuracy
            // we shift because we want to keep the most significant bits of the fixed point
            ;
            signed16temp_r = (INT16S) signed8temp + (INT16S) output_r[k];
            ;

            // computation for imaginary part
            signed16temp_i = (INT16S)input[n] * (INT16S) en_i[m];
            signed8temp = signed16temp_i >>8; // we shift the value and loose some accuracy
            ;
            signed16temp_i = (INT16S) signed8temp + (INT16S) output_i[k];
            ;
        }
        output_r[k] = (INT8S) signed16temp_r ; // TODO : PROBABLY WRONG
        output_i[k] = (INT8S) signed16temp_i;
    }

    // 4 lines to print the result
    printf("---- RESULT -----\n");
    for (k = 0; k < N; k = k+1){
        printf("%d : %f +%fi",k, (((double) output_r[k])/128), (((double) output_i[k])/128));
        printf("\n");
    }
}

