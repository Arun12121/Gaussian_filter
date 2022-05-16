#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>

#define smooth_kernel_size 5
#define sigma 1.0
#define K  1.0
#define k 5

#define WIDTH1 480
#define WIDTH2 360

using namespace std;


void convolution_2D(int N[WIDTH1][WIDTH2], double M[smooth_kernel_size], int P[WIDTH1][WIDTH2]) {

    // find center position of kernel (half of kernel size)
    int kCenterX = smooth_kernel_size / 2;
    int kCenterY = smooth_kernel_size / 2;
    double T[WIDTH1][WIDTH2];

    for (int i = 0; i < WIDTH1; ++i)              // rows
    {
        for (int j = 0; j < WIDTH2; ++j)          // columns
        {
            T[i][j] = 0;
            for (int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
            {
                //int mm = MASK_WIDTH1 - 1 - m;      // row index
                int ii = i + (m - kCenterY);

                // ignore input samples which are out of bound
                if (ii >= 0 && ii < WIDTH1)
                    T[i][j] += N[ii][j] * M[m];
            }
        }
    }

    for (int i = 0; i < WIDTH1; ++i)              // rows
    {
        for (int j = 0; j < WIDTH2; ++j)          // columns
        {
            for (int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
            {
                //int mm = MASK_WIDTH1 - 1 - m;      // row index
                int jj = j + (m - kCenterX);

                // ignore input samples which are out of bound
                if (jj >= 0 && jj < WIDTH2)
                    P[i][j] += T[i][jj] * M[m];
            }
        }
    }
}
int main() {

    int image[WIDTH1][WIDTH2]; // have to get some image
    int output[WIDTH1][WIDTH2]; //have to get some outtput
    double gauss_row[smooth_kernel_size];
    double sum_row = 0;
    int i, j;

    ifstream fp("/home/arun/Course_Work/EE5332/img.txt");
    if (! fp) {
        cout << "Error, file couldn't be opened" << endl; 
        return 1; 
    }    
    for(int row = 0; row < WIDTH1; row++) {  // stop loops if nothing to read
        for(int column = 0; column < WIDTH2; column++){
                fp >> image[row][column];
                output[row][column] = 0;
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

    // for (i = 0; i < smooth_kernel_size; i++) {
    //     for (j = 0; j < smooth_kernel_size; j++) {
    //         printf("%f ", gauss[i][j]);
    //     }
    //     printf("\n");
    // }

    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    convolution_2D(image, gauss_row, output);

    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    for(int row = 0; row < WIDTH1; row++) {  // stop loops if nothing to read
        for(int column = 0; column < WIDTH2; column++){
                cout<<output[row][column]<<" ";
            }
            cout<<"\n";
    }
    printf("Time measured: %ld nanoseconds.\n", nanoseconds);
    return 0;
}
