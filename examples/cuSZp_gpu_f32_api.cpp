#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cuda_runtime.h>
#include <cstring>
#include <cuSZp_utility.h>
#include <cuSZp_entry_f32.h>
#include <cuSZp_timer.h>

void saveToFile(const float* data, size_t n, const std::string& filename) {
    // Open a file for writing
    std::ofstream outputFile(filename, std::ios::binary);

    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
        return;
    }

    // Write the data to the file
    outputFile.write(reinterpret_cast<const char*>(data), n * sizeof(float));

    // Close the file
    outputFile.close();
}

// Function to extract the filename from a path
std::string extractFilename(const std::string& fullPath) {
    // Find the last occurrence of the directory separator
    size_t lastSeparator = fullPath.find_last_of("/\\");
    
    // If the separator is found, return the substring after it
    if (lastSeparator != std::string::npos) {
        return fullPath.substr(lastSeparator + 1);
    }
    
    // If no separator is found, the entire path is considered the filename
    return fullPath;
}

int main(int argc, char* argv[])
{
    // Read input information.
    char oriFilePath[640];
    char errorMode[20];
    int status=0;
    if(argc != 4)
    {
        printf("Ussssage: cuSZp_gpu_f32_api [srcFilePath] [errorMode] [errBound] # errorMode can only be ABS or REL\n");
        printf("Example: cuSZp_gpu_f32_api testfloat_8_8_128.dat ABS 1E-2     # compress dataset with absolute 1E-2 error bound\n");
        printf("         cuSZp_gpu_f32_api testfloat_8_8_128.dat REL 1e-3     # compress dataset with relative 1E-3 error bound\n");
        exit(0);
    }
    sprintf(oriFilePath, "%s", argv[1]);
    sprintf(errorMode, "%s", argv[2]);
    float errorBound = atof(argv[3]);

    // For measuring the end-to-end throughput.
    TimingGPU timer_GPU;

    // Input data preparation on CPU.
    float* oriData = NULL;
    float* decData = NULL;
    unsigned char* cmpBytes = NULL;
    size_t nbEle = 0;
    size_t cmpSize = 0;
    oriData = readFloatData_Yafan(oriFilePath, &nbEle, &status);
    decData = (float*)malloc(nbEle*sizeof(float));
    cmpBytes = (unsigned char*)malloc(nbEle*sizeof(float));

    // Generating error bounds.

    float max_val = oriData[0];
    float min_val = oriData[0];
    for(size_t i=0; i<nbEle; i++)
    {
        if(oriData[i]>max_val)
            max_val = oriData[i];
        else if(oriData[i]<min_val)
            min_val = oriData[i];
    }

    printf("Max: %f, Min: %f\n", max_val, min_val);

    if(strcmp(errorMode, "REL")==0)
    {
        errorBound = errorBound * (max_val - min_val);
    }
    else if(strcmp(errorMode, "ABS")!=0)
    {
        printf("invalid errorMode! errorMode can only be ABS or REL.\n");
        exit(0);
    }

    // Input data preparation on GPU.
    float* d_oriData;
    float* d_decData;
    unsigned char* d_cmpBytes;
    size_t pad_nbEle = (nbEle + 262144 - 1) / 262144 * 262144; // A temp demo, will add more block sizes in future implementation.
    cudaMalloc((void**)&d_oriData, sizeof(float)*pad_nbEle);
    cudaMemcpy(d_oriData, oriData, sizeof(float)*pad_nbEle, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&d_decData, sizeof(float)*pad_nbEle);
    cudaMemset(d_decData, 0, sizeof(float)*pad_nbEle);
    cudaMalloc((void**)&d_cmpBytes, sizeof(float)*pad_nbEle);

    // Initializing CUDA Stream.
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    // Just a warmup.
    for(int i=0; i<3; i++)
        SZp_compress_deviceptr_f32(d_oriData, d_cmpBytes, nbEle, &cmpSize, errorBound, stream);

    // cuSZp compression.
    timer_GPU.StartCounter(); // set timer
    SZp_compress_deviceptr_f32(d_oriData, d_cmpBytes, nbEle, &cmpSize, errorBound, stream);
    float cmpTime = timer_GPU.GetCounter();
    
    // cuSZp decompression.
    timer_GPU.StartCounter(); // set timer
    SZp_decompress_deviceptr_f32(d_decData, d_cmpBytes, nbEle, cmpSize, errorBound, stream);
    float decTime = timer_GPU.GetCounter();

    // Print result.
    //printf("cuSZp finished!\n");
    //printf("cuSZp compression   end-to-end speed: %f GB/s\n", (nbEle*sizeof(float)/1024.0/1024.0)/cmpTime);
    //printf("cuSZp decompression end-to-end speed: %f GB/s\n", (nbEle*sizeof(float)/1024.0/1024.0)/decTime);
    //printf("cuSZp compression ratio: %f\n", (nbEle*sizeof(float)/1024.0/1024.0)/(cmpSize*sizeof(unsigned char)/1024.0/1024.0));
    //printf("cuSZp compressed size: %zu bytes\n", cmpSize);
    //printf("cuSZp compression time: %f ms\n", cmpTime);
    //printf("cuSZp decompression time: %f ms\n\n", decTime);

    // for benchmarking only print the values (ratio,cmpTime,decTime)
    printf("%f,%f,%f\n", (nbEle*sizeof(float)/1024.0/1024.0)/(cmpSize*sizeof(unsigned char)/1024.0/1024.0), cmpTime, decTime);

    // Error check
    cudaMemcpy(cmpBytes, d_cmpBytes, cmpSize*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    cudaMemcpy(decData, d_decData, sizeof(float)*nbEle, cudaMemcpyDeviceToHost);
    int not_bound = 0;
    for(size_t i=0; i<nbEle; i+=1)
    {
        if(abs(oriData[i]-decData[i]) > errorBound*1.1)
        {
            not_bound++;
            // printf("not bound: %zu oriData: %f, decData: %f, errors: %f, bound: %f\n", i, oriData[i], decData[i], abs(oriData[i]-decData[i]), errBound);
        }
    }
    //if(!not_bound) printf("\033[0;32mPass error check!\033[0m\n");
    //else printf("\033[0;31mFail error check!\033[0m\n");

        // Extract the second and fourth arguments
    
    std::string arg4 = argv[3];

    // Extract the last character of the fourth argument
    std::string x(1, arg4.back());

    // Create the desired path
    std::string path = "/beegfs/ws/1/anbr392b-test-workspace/vdi_compression/decompressedVDIs/" + x + "/" + extractFilename(argv[1]);


    // before dumping the data save it to a file
    std::cout << path << "\n";
  
    saveToFile(decData , nbEle, path);
    
    // Free allocated data.
    free(oriData);
    free(decData);
    free(cmpBytes);
    cudaFree(d_oriData);
    cudaFree(d_decData);
    cudaFree(d_cmpBytes);
    cudaStreamDestroy(stream);
    return 0;
}
