#include "shared.h"
#include "kernels.h"

bool initalized = false;

__global__ void CalculateByteFrequency(Node* hashmap, const unsigned char* filebuffer, unsigned int filebufferSize ){
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < filebufferSize) {
    
        unsigned char byte = filebuffer[idx];
        //hashmap[byte].character = byte;
        //hashmap[byte].frequency++;

    }
}


unsigned char* _device_buffer;
Node*  _device_hashmap;

void Init(unsigned int bufferSize, FrequencyMap& hashmap){
    gpuErrchk(cudaMalloc((void **)&_device_buffer,                   bufferSize));
    gpuErrchk(cudaMalloc((void **)&_device_hashmap, sizeof(Node)*hashmap.size()));
    gpuErrchk(cudaMemcpy(_device_hashmap, hashmap.data(), sizeof(Node)*hashmap.size(), cudaMemcpyHostToDevice));
    initalized = true;
}

void CudaGetCharacterFrequencies(FrequencyMap& hashmap, const std::vector<unsigned char>& buffer){
    if (!initalized) Init(buffer.size(), hashmap);
    gpuErrchk(cudaMemcpy(_device_buffer, buffer.data(), buffer.size(), cudaMemcpyHostToDevice));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);    

    cudaEventRecord(start);
    CalculateByteFrequency<<< buffer.size() / 1024 + 1, 1024 >>>( hashmap.data(), buffer.data(), buffer.size() );
    cudaEventRecord(stop);
    gpuErrchk(cudaEventSynchronize(stop));

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    std::cout << "Cuda Kernel Execution took " << milliseconds << std::endl;
    gpuErrchk(cudaMemcpy(hashmap.data(), _device_hashmap, sizeof(Node)*hashmap.size(), cudaMemcpyDeviceToHost));
}