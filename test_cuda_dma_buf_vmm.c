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


    // ITEMS 3-6 IF WE ARE DOING VMM ALLOC! OTHER OPTIONS IT TO CALL cuMemAlloc()

    // 3. Allocate Physcial Memory
    uint64_t page_size = 1 << 21;
    // 200 MB chunk
    uint64_t alloc_size = 100 * page_size;

    CUmemGenericAllocationHandle phys_mem_handle;
    CUmemAllocationProp prop = {};
    prop.type = CU_MEM_ALLOCATION_TYPE_PINNED;
    prop.location.type = CU_MEM_LOCATION_TYPE_DEVICE;
    prop.location.id = device_id;
    CUmemAllocationHandleType handle_type = CU_MEM_HANDLE_TYPE_POSIX_FILE_DESCRIPTOR;
    prop.requestedHandleTypes = handle_type;

    result = cuMemCreate(&phys_mem_handle, alloc_size, &prop, 0);
    if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Error: Could not get alloc handle: %s\n", err);
    	return -1;
    }


    // 4. Reserve VA Range
    CUdeviceptr d_ptr;
    // aligned by construction of alloc_size
    result = cuMemAddressReserve(&d_ptr, alloc_size, 0, 0, 0);
	if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Error: Could not allocate device range: %s\n", err);
    	return -1;
    }

    // 5. Do mapping
    result = cuMemMap(d_ptr, alloc_size, 0, phys_mem_handle, 0);
    if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Error: Could not do memory mapping: %s\n", err);
    	return -1;
    }

    // 6. Set Access
    CUmemAccessDesc accessDescriptor;
    accessDescriptor.location.id = device_id;
    accessDescriptor.location.type = CU_MEM_ALLOCATION_TYPE_PINNED;
    // ENSURE IMMUTABLE OBJECTS BECAUSE RELYING ON SHARED MEMORY AND DEDUPED IDs!
    accessDescriptor.flags = CU_MEM_ACCESS_FLAGS_PROT_READWRITE;

    result = cuMemSetAccess(d_ptr, alloc_size, &accessDescriptor, 1);
    if (result != CUDA_SUCCESS){
    	cuGetErrorString(result, &err);
    	fprintf(stderr, "Error: Could not set access: %s\n", err);
    	return -1;
    }


    // 7. GET DMA BUF
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