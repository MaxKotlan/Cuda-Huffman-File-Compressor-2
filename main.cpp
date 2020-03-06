#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <string> 

/*Datastructure Which Holds a character, and an integer with the number of occurences in a file*/
typedef std::pair<char, unsigned int> CharacterFrequency;
/*HashmapForCharacterFrequency*/
typedef std::vector<CharacterFrequency> FrequencyMap;

FrequencyMap findFileFrequencies(){
    std::ifstream fin("Compress.exe", std::ifstream::binary);
    std::cout << sizeof(char);
    FrequencyMap hashmap(256);

    std::vector<char> buffer (1024); 
    do {
        fin.read(buffer.data(), buffer.size());
        for (int i = 0; i < fin.gcount(); i++){
            char letter = buffer[i];
            hashmap[letter].first = letter;
            hashmap[letter].second ++;
        }
    } while(fin);
    return hashmap;
}

// comparator function to make min heap 
struct greaters{ 
  bool operator()(const CharacterFrequency& a,const CharacterFrequency& b) const{ 
    return a.second>=b.second; 
  } 
}; 
  

int main(){
    auto map = findFileFrequencies();

    for (auto it = map.begin(); it != map.end(); it++){
        std::cout << it->second << " ";
    }std::cout << std::endl << std::endl;

    std::make_heap(map.begin(), map.end(), greaters());

    for (auto it = map.begin(); it != map.end(); it++){
        std::cout << it->second << " ";
    }std::cout << std::endl << std::endl;

    /*Remove All Characters with Zero Frequencies from the Heap*/
    while (map.front().second == 0) {
        std::pop_heap (map.begin(),map.end(), greaters()); map.pop_back();
    }
    
    /*Remove All Zeros from the heap*/
    while (map.size() > 0) {
        std::cout << map.front().second << " ";
        std::pop_heap (map.begin(),map.end(), greaters()); map.pop_back();
        //std::sort_heap (map.begin(),map.end());
    }

/*
    for (auto it = map.begin(); it != map.end(); it++){
        std::cout << it->second << " ";
    }*/

}