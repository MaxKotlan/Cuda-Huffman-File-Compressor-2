all:
	nvcc -o Compress main.cpp kernels\kernels.cu