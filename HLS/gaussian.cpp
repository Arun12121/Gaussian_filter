#define smooth_kernel_size 5
#define sigma 1.0
#define K  1

#define WIDTH1 480
#define WIDTH2 360


void convolution_2D(int N[WIDTH1][WIDTH2], float M[smooth_kernel_size][smooth_kernel_size], int P[WIDTH1][WIDTH2]) {
#pragma HLS INTERFACE mode=m_axi port=P
#pragma HLS INTERFACE mode=m_axi port=M
#pragma HLS INTERFACE mode=m_axi port=N

    // find center position of kernel (half of kernel size)
    int kCenterX = smooth_kernel_size / 2;
    int kCenterY = smooth_kernel_size / 2;

    for (int i = 0; i < WIDTH1; ++i)              // rows
    {
        for (int j = 0; j < WIDTH2; ++j)          // columns
        {
#pragma HLS UNROLL
            for (int m = 0; m < smooth_kernel_size; ++m)     // kernel rows
            {
                for (int n = 0; n < smooth_kernel_size; ++n) // kernel columns
                {
                    // index of input signal, used for checking boundary
                    int ii = i + (m - kCenterY);
                    int jj = j + (n - kCenterX);

                    // ignore input samples which are out of bound
                    if (ii >= 0 && ii < WIDTH1 && jj >= 0 && jj < WIDTH2)
                        P[i][j] += N[ii][jj] * M[m][n];
                }
            }
        }
    }
}
