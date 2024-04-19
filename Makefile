CC = gcc
CFLAGS = -ggdb

CUDA_INCLUDE = /usr/local/cuda/include

EXECS = test_cuda_dma_buf_supported test_cuda_dma_buf_reg test_cuda_dma_buf_vmm

all: $(EXECS)

test_cuda_dma_buf_supported: test_cuda_dma_buf_supported.c
	$(CC) $(CFLAGS) test_cuda_dma_buf_supported.c -o test_cuda_dma_buf_supported -I $(CUDA_INCLUDE) -lcuda

test_cuda_dma_buf_reg: test_cuda_dma_buf_reg.c
	$(CC) $(CFLAGS) test_cuda_dma_buf_reg.c -o test_cuda_dma_buf_reg -I $(CUDA_INCLUDE) -lcuda

test_cuda_dma_buf_vmm: test_cuda_dma_buf_vmm.c
	$(CC) $(CFLAGS) test_cuda_dma_buf_vmm.c -o test_cuda_dma_buf_vmm -I $(CUDA_INCLUDE) -lcuda



clean:
	rm -f *.o $(EXECS)