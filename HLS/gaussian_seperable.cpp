#include <ap_fixed.h>
#include <ap_int.h>

#define smooth_kernel_size 5
#define sigma 1.0
#define K  1.0

#define WIDTH1 480
#define WIDTH2 360


void convolution_2D(int N[WIDTH1][WIDTH2], float M_float[smooth_kernel_size], int P[WIDTH1][WIDTH2]) {\

#pragma HLS ARRAY_PARTITION dim=2 factor=30 type=cyclic variable=N
#pragma HLS ARRAY_RESHAPE dim=2 factor=20 type=cyclic variable=P
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=M_float

    // find center position of kernel (half of kernel size)
	unsigned int kCenterX = smooth_kernel_size / 2;
	unsigned int kCenterY = smooth_kernel_size / 2;
    ap_ufixed<16,8> T[WIDTH1][WIDTH2];
    ap_ufixed<8, 0> M[smooth_kernel_size];
#pragma HLS ARRAY_PARTITION dim=2 factor=20 type=cyclic variable=T
#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=M


    for (int i = 0; i < smooth_kernel_size; ++i)
    {
#pragma HLS UNROLL
    	M[i] = M_float[i];
    }



    for (unsigned int i = 0; i < WIDTH1; ++i)              // rows
    {
        for (unsigned int j = 0; j < WIDTH2; ++j)          // columns
        {
#pragma HLS UNROLL factor=20 skip_exit_check
            T[i][j] = 0;
            for (unsigned int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
            {
#pragma HLS UNROLL
                //int mm = MASK_WIDTH1 - 1 - m;      // row index
            	unsigned int ii = i + (m - kCenterY);

                // ignore input samples which are out of bound
                if (ii >= 0 && ii < WIDTH1)
                    T[i][j] += N[ii][j] * M[m];
            }
        }
    }

    for (unsigned int i = 0; i < WIDTH1; ++i)              // rows
    {
        for (unsigned int j = 0; j < WIDTH2; ++j)          // columns
        {
#pragma HLS UNROLL factor=20 skip_exit_check
            for (unsigned int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
            {
#pragma HLS UNROLL
                //int mm = MASK_WIDTH1 - 1 - m;      // row index
            	unsigned int jj = j + (m - kCenterX);

                // ignore input samples which are out of bound
                if (jj >= 0 && jj < WIDTH2)
                    P[i][j] += (unsigned int)(T[i][jj] * M[m]);
            }
        }
    }
}
