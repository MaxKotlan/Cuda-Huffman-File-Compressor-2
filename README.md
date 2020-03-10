# Cuda Accelerated File Compressor #

This project is an example of a Cuda accelerated file compressor. When I started planning out the design of this project, I did not think Cuda would be useful, because I thought file compression mainly relied on I/O speed. However, for large enough files, there is a noticeable increase in speed.

## Quick usage guide ##

Two parameters are required. Either --compress or --decompress and a filename. There are also a few optional parameters which may improve performance. At the moment, only compression works. 

    Compress.exe [ --compress | --decompress ] [ filename ] 

Example usage 

    Compress.exe --compress lotr.txt

The following is a list of optional parameters 

| Parameter | Description |
|--|--|
| \-\-disablefilebuffer  |  if this flag is present, the entire file will be copied into ram and vram in a single copy|
|\-\-filebuffersize [ int ]| if filebuffer is enabled (by default it is), this sets the size|
|\-\-cuda| Will use gpu acceleration for finding the frequency of each byte|

Example usage:

    Compress.exe --compress lotr.txt --cuda --disablefilebuffer

## Performance ##

At the moment, the only gpu accelerated section of this program is the portion that calculates the byte frequency. Testing with a 488 Mb file, a Geforce Gtx 980, and a Samsung Evo SSD, I was able to calculate the frequency of each byte in the file in  0.818 seconds. Using the CPU version of the same algorithm, I was able to calculate the frequency of each byte in 6.414 seconds (using an i5-3570k).  When testing the Cuda version I disabled the filebuffer (meaning the entire file was copied into ram). If the filebuffer is not disabled and set to its default size of 1024 bytes, Cuda version runs much slower. In my case, it took 7.054 seconds.

I think in the future, more steps in the process could be accelerated, leading to extremely fast file compression.

## What is working ##

 - Frequency of each byte in the file is calculated
 - Huffman Tree builds correctly
 - Correct Huffman codes being generated (when using strings, not bit-shifting )
 - File headers are correctly generated ( file headers will store the frequency of each character )
 - Compressed files are being saved, with the correct size (compressed data is close, but something is slightly off)

## What still needs to be done ##

 - More parallelized sections of code
 - Completing the bit shifting algorithm, to correctly save the file to disk.  A few bytes are incorrect when comparing with the correctly verified string version. 
 - Decompression
 - More configuration options
