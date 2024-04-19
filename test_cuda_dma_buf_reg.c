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

    // 3. Alloc Memory with cuMemAlloc
    uint64_t page_size = 1 << 21;
    // 200 MB chunk
    uint64_t alloc_size = 100 * page_size;

    CUdeviceptr d_ptr;
    result = cuMemAlloc(&d_ptr, alloc_size);
    if (result != CUDA_SUCCESS){
        cuGetErrorString(result, &err);
        fprintf(stderr, "Error: cuMemAlloc failed: %s\n", err);
        return -1;
    }


    // 3. GET DMA BUF
    int dma_buf_fd;

    result = cuMemGetHandleForAddressRange((void *)&dma_buf_fd, d_ptr, alloc_size, CU_MEM_RANGE_HANDLE_TYPE_DMA_BUF_FD, 0);
    if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Error: could not get dma_buf_fd: %s\n", err);
    	return -1;
    }

    printf("DMA BUF FD: %d\n", dma_buf_fd);
    
    return 0;
}