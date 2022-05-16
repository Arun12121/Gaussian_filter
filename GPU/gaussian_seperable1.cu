#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cuda.h>
#include <cooperative_groups.h>

#define smooth_kernel_size 5
#define sigma 1.0
#define K  1.0

#define WIDTH1 480
#define WIDTH2 360

using namespace std;


__global__ void convolution_2D(int N[WIDTH1*WIDTH2], double M[smooth_kernel_size], int P[WIDTH1*WIDTH2], double T[WIDTH1*WIDTH2]) {
    //cooperative_groups::grid_group grid = cooperative_groups::this_grid();

    // find center position of kernel (half of kernel size)
    int kCenterX = smooth_kernel_size / 2;
    int kCenterY = smooth_kernel_size / 2;

    int i = blockIdx.x;
    int j = threadIdx.x;
    //printf("%d %d\n", gridDim.x, blockDim.x);

    T[i*WIDTH2+j] = 0;
    for (int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
    {
        //int mm = MASK_WIDTH1 - 1 - m;      // row index
        int ii = i + (m - kCenterY);

        // ignore input samples which are out of bound
        if (ii >= 0 && ii < WIDTH1)
            T[i*WIDTH2+j] += N[ii*WIDTH2+j] * M[m];
    }
    //grid.sync();
    __syncthreads();

    for (int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
    {
        //int mm = MASK_WIDTH1 - 1 - m;      // row index
        int jj = j + (m - kCenterX);

        // ignore input samples which are out of bound
        if (jj >= 0 && jj < WIDTH2)
            P[i*WIDTH2+j] += T[i*WIDTH2+jj] * M[m];
    }
}
int main() {

    int image[WIDTH1*WIDTH2]; // have to get some image
    int output[WIDTH1*WIDTH2]; //have to get some outtput
    double gauss_row[smooth_kernel_size];
    double sum_row = 0;
    int i, j;

    int *gpuImage, *gpuOutput;
    double *gpuT, *gpuM;

    cudaMalloc(&gpuImage, WIDTH1*WIDTH2*sizeof(int));
    cudaMalloc(&gpuOutput, WIDTH1*WIDTH2*sizeof(int));
    cudaMalloc(&gpuT, WIDTH1*WIDTH2*sizeof(double));
    cudaMalloc(&gpuM, smooth_kernel_size*sizeof(double));

    ifstream fp("/home/arun/Course_Work/EE5332/img.txt");
    if (! fp) {
        cout << "Error, file couldn't be opened" << endl; 
        return 1; 
    }    
    for(int row = 0; row < WIDTH1; row++) {  // stop loops if nothing to read
        for(int column = 0; column < WIDTH2; column++){
                fp >> image[row*WIDTH2+column];
                output[row*WIDTH2+column] = 0;
                if ( ! fp ) {
                cout << "Error reading file for element " << row << "," << column << endl; 
                return 1; 
                }
            }
    }

    for (i = 0; i < smooth_kernel_size; i++) {
        double x = i - (smooth_kernel_size - 1) / 2.0;
        gauss_row[i] = K * exp((pow(x, 2) / ((2 * pow(sigma, 2)))) * (-1));
        sum_row += gauss_row[i];
    }

    for (i = 0; i < smooth_kernel_size; i++) {
        gauss_row[i] /= sum_row;
    }

	cudaMemcpy(gpuImage, image, WIDTH1*WIDTH2*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(gpuOutput, output, WIDTH1*WIDTH2*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(gpuM, gauss_row, smooth_kernel_size*sizeof(double), cudaMemcpyHostToDevice);

    // for (i = 0; i < smooth_kernel_size; i++) {
    //     for (j = 0; j < smooth_kernel_size; j++) {
    //         printf("%f ", gauss[i][j]);
    //     }
    //     printf("\n");
    // }

    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    //void *args[] = {&gpuImage, &gpuM, &gpuOutput, &gpuT};
    //cudaLaunchCooperativeKernel((void*)convolution_2D, dim3(WIDTH1, 1, 1), dim3(WIDTH2, 1, 1), args);
    convolution_2D<<<480, 360>>>(gpuImage, gpuM, gpuOutput, gpuT);
    cudaDeviceSynchronize();

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

	cudaMemcpy(output, gpuOutput, WIDTH1*WIDTH2*sizeof(int), cudaMemcpyDeviceToHost);
    //cudaError_t err = cudaGetLastError();
    //printf(“%d, %s, %s\n”, err, cudaGetErrorName(err), cudaGetErrorString(err))

    for(int row = 0; row < WIDTH1; row++) {  // stop loops if nothing to read
        for(int column = 0; column < WIDTH2; column++){
                cout<<output[row*WIDTH2+column]<<" ";
            }
            cout<<"\n";
    }
    printf("Time measured: %ld nanoseconds.\n", nanoseconds);
    return 0;
}
