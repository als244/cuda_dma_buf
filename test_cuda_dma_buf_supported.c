#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cuda.h>


int main(int argc, char * argv[]){
	
	CUresult result;
	const char * err;

	// 1. Initialize Driver
	unsigned int init_flags = 0;
    result = cuInit(init_flags);
    if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Could not init cuda: %s\n", err);
    	return -1;
    }

    // 2. Create Context
 	//		- not actually necessary because primary context created upon init
 	//		- for clarity

    int device_id = 0;

 	CUcontext ctx;
 	result = cuCtxCreate(&ctx, 0, (CUdevice) device_id);
 	if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Could not create context: %s\n", err);
    	return -1;
    }

    // 3. Test support
    int is_supported;

    result = cuDeviceGetAttribute(&is_supported, CU_DEVICE_ATTRIBUTE_DMA_BUF_SUPPORTED, device_id);
    if (result != CUDA_SUCCESS){
        cuGetErrorString(result, &err);
        fprintf(stderr, "Could not get device attribute: %s\n", err);
        return -1;
    }

    printf("DMA BUF supported: %d\n", is_supported);
   
    
    return 0;
}