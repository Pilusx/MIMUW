#include "algebra.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <ctime>
#include <stdio.h>

#define TEST 3

using namespace std;

#if TEST == 1 || TEST == 2

#define BLOCK_SIZE 16
#define N 10

#elif TEST == 3

#define BLOCK_SIZE 32
#define N 4000

#endif


typedef Vector<N, PHOST> Vec;
typedef Vector<N, PDEVICE> d_Vec;
typedef Matrix<N, N, PHOST> Mat;
typedef Matrix<N, N, PDEVICE> d_Mat;

dim3 grid_size(dim3 dimBlock, size_t x, size_t y) {
    dim3 dimGrid(x / dimBlock.x, y / dimBlock.y);
    if(x % dimBlock.x != 0) dimGrid.x++;
    if(y % dimBlock.y != 0) dimGrid.y++;
    return dimGrid;
}

void init_vector(Vec& v) {
    for(int i = 0; i < v.get_width(); i++)
        v.get(i) = i;
}

void init_matrix(Mat& M) {
    for(int i = 0; i < M.get_width(); i++) {
        for(int j = 0; j < M.get_height(); j++) {
            M.get(i, j) = i + j*N;
        }
    }
}

float random_float() {
    return (float)rand() * (float)( rand() % 100000) / (float)rand();
}

void random_matrix(Mat& M) {
    for(int i = 0; i < M.get_width(); i++) {
        for(int j = 0; j < M.get_height(); j++) {
            M.get(i, j) = random_float();
        }
    }
}


int main() {
#if TEST == 1
    Vec v, res;
    Mat A;
    d_Vec d_v, d_res;
    d_Mat d_A;

    A.alloc();
    v.alloc();
    res.alloc();
    d_v.alloc();
    d_A.alloc();
    d_res.alloc();

    init_vector(v);
    init_matrix(A);

    //VecMatMul(v, A, res);

    cudaMemcpy(d_A.elements, A.elements, A.get_size(), cudaMemcpyHostToDevice);
    cudaMemcpy(d_v.elements, v.elements, v.get_size(), cudaMemcpyHostToDevice);
    
    dim3 dimBlock(BLOCK_SIZE, 1);
    dim3 dimGrid = grid_size(dimBlock, res.get_width(), 1);
    VecMatMulKernel<<<dimGrid, dimBlock>>>(d_v, d_A, d_res);

    cudaMemcpy(res.elements, d_res.elements, res.get_size(), cudaMemcpyDeviceToHost);

    cout << v << endl << A << endl << res << endl;

    A.dealloc();
    v.dealloc();
    res.dealloc();
    d_v.dealloc();
    d_A.dealloc();
    d_res.dealloc();

#elif TEST == 2
    Mat A, D;
    d_Mat d_A, d_D;

    A.alloc();
    D.alloc();
    d_A.alloc();
    d_D.alloc();

    init_matrix(A);

    cudaMemcpy(d_A.elements, A.elements, A.get_size(), cudaMemcpyHostToDevice);
    
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid = grid_size(dimBlock, A.get_width(), A.get_height());
    MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_A, d_D);
    
    cudaMemcpy(D.elements, d_D.elements, D.get_size(), cudaMemcpyDeviceToHost);

    cout << A << endl << D << endl;

    A.dealloc();
    D.dealloc();
    d_A.dealloc();
    d_D.dealloc();

#elif TEST == 3
    Mat A, B, C;
    d_Mat d_A, d_B, d_C;
    srand(time(0));

    A.alloc();
    B.alloc();
    C.alloc();
    d_A.alloc();
    d_B.alloc();
    d_C.alloc();

    random_matrix(A);
    random_matrix(B);

    cudaMemcpy(d_A.elements, A.elements, A.get_size(), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B.elements, B.elements, B.get_size(), cudaMemcpyHostToDevice);


    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
    dim3 dimGrid = grid_size(dimBlock, N, N);

    MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);

    cudaMemcpy(C.elements, d_C.elements, C.get_size(), cudaMemcpyDeviceToHost);

    //MatMul(A, B, C);

    if(N < 5)
        cout << A << endl << B << endl << C << endl;

    A.dealloc();
    B.dealloc();
    C.dealloc();
    d_A.dealloc();
    d_B.dealloc();
    d_C.dealloc();
#endif
}
