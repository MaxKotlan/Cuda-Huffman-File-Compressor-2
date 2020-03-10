#include "shared.h"
#include "kernels.h"

bool initalized = false;

__global__ void CalculateByteFrequency(Node* hashmap, const unsigned char* filebuffer, unsigned int filebufferSize, unsigned int bufferIteration, unsigned int fileSize){
    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    if (idx < filebufferSize && (idx + bufferIteration*filebufferSize) < fileSize) {
        unsigned char byte = filebuffer[idx];
        atomicAdd(&hashmap[byte].frequency, 1);
    }
}


unsigned char* _device_buffer;
Node*  _device_hashmap;

void Init(unsigned int bufferSize, FrequencyMap& hashmap){
    gpuErrchk(cudaMalloc((void **)&_device_buffer,                         bufferSize));
    gpuErrchk(cudaMalloc((void **)&_device_hashmap,       sizeof(Node)*hashmap.size()));
    gpuErrchk(cudaMemcpy(_device_hashmap, hashmap.data(), sizeof(Node)*hashmap.size(), cudaMemcpyHostToDevice));
    initalized = true;
}

void CudaGetCharacterFrequencies(FrequencyMap& hashmap, const std::vector<unsigned char>& buffer, unsigned int bufferIteration, unsigned int fileSize){
    if (!initalized) Init(buffer.size(), hashmap);
    gpuErrchk(cudaMemcpy(_device_buffer, buffer.data(), buffer.size(), cudaMemcpyHostToDevice));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);    

    cudaEventRecord(start);
    //std::cout << "Initalizing kernel to calculate hashmap on " << hashmap.size() << " elements each of " << sizeof(Node)*hashmap.size() << std::endl;

    CalculateByteFrequency<<< buffer.size() / 1024 + 1, 1024 >>>( _device_hashmap, _device_buffer, buffer.size(), bufferIteration, fileSize);
    cudaEventRecord(stop);
    gpuErrchk(cudaEventSynchronize(stop));

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    /*If entire file has been read*/
    if ((bufferIteration+1)*buffer.size() >= fileSize){
        gpuErrchk(cudaMemcpy(hashmap.data(), _device_hashmap, sizeof(Node)*hashmap.size(), cudaMemcpyDeviceToHost));
        cudaFree(_device_hashmap); cudaFree(_device_buffer);

        /*In serial set characters in hashmap*/
        for (int i =0; i < hashmap.size(); i++)
            hashmap[i].character = (unsigned char)i;

        initalized = false;
    }
}