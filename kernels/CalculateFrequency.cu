#include <cuda>
#include "shared.h"

__global__ 

void GetCharacterFrequencies(std::vector<FrequencyMap>& hashmap, const std::vector<unsigned char>& buffer){
    //gpuErrchk(cudaMalloc((void **)&dev_mat_a,          allocationsize));
}