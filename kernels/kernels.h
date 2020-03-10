#ifndef USE_CUDA_KERNELS
#define USE_CUDA_KERNELS

#include <vector>
#include "../Node.h"

typedef std::vector<Node> FrequencyMap;

void CudaGetCharacterFrequencies(FrequencyMap& hashmap, const std::vector<unsigned char>& buffer);

#endif