#include <iostream>
#include <initializer_list>
using namespace std;

#define PHOST 1
#define PDEVICE 2
#define Place size_t


template<size_t size, Place P>
class Array {
public:
    float* elements;
    size_t static get_size() { return sizeof(float) * size; }
    void alloc() {
        if(P == PHOST)
            elements = (float*)malloc(get_size());
        else
            cudaMalloc(&elements, get_size());
    }
    void dealloc() {
        if(P == PHOST)
            free(elements);
        else
            cudaFree(elements);
    }
};


template<size_t width, Place P>
class Vector : public Array<width, P> {
public:
    __host__ __device__ __inline__ float& get(size_t x) const { return this->elements[x]; }
    __host__ __device__ __inline__ static constexpr size_t get_width() { return width; }

    template<size_t w, Place place>
    friend ostream& operator<<(ostream&, const Vector<w, place>&);
};

template<size_t width, size_t height, Place P>
class Matrix : public Array<width*height, P> {
public:
    __host__ __device__ __inline__ static constexpr size_t idx(size_t x, size_t y) { return width*y + x; }
    __host__ __device__ __inline__ float& get(size_t x, size_t y) const { return this->elements[idx(x, y)]; }
    __host__ __device__ __inline__ static constexpr size_t get_width() { return width; }
    __host__ __device__ __inline__ static constexpr size_t get_height() { return height; }

    template<size_t w, size_t h, Place place>
    __host__ friend ostream& operator<<(ostream&, const Matrix<w, h, place>&);
};


template <size_t width, Place place>
__host__ ostream& operator<<(ostream &os, const Vector<width, place> &v)
{
    for (int x = 0; x < width; x++)
        printf("%f ", v.get(x));
    printf("\n");
    return os;
}

template <size_t width, size_t height, Place place>
__host__ ostream &operator<<(ostream &os, const Matrix<width, height, place> &A)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
            printf("%f ", A.get(x, y));
        printf("\n");
    }
    return os;
}

template <size_t N, size_t M, size_t R, Place P>
__host__ void MatMul(const Matrix<N, M, P> &A, const Matrix<M, R, P> &B, Matrix<N, R, P> &C)
{
    static_assert(P == PHOST, "WRONG DEVICE");
    for (int i = 0; i < N; i++)
        for (int j = 0; j < R; j++)
        {
            C.get(i, j) = 0;
            for (int k = 0; k < M; k++)
                C.get(i, j) += A.get(i, k) * B.get(k, j);
        }
}

template<size_t N, size_t M, size_t R, Place P>
__global__ void MatMulKernel(const Matrix<N, M, P> A, const Matrix<M, R, P> B, 
        Matrix<N, R, P> C) {
    static_assert(P == PDEVICE, "WRONG DEVICE");
    
    float CValue = 0;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if(row < N && col < R) {
        for(int e = 0; e < M; e++)
            CValue += A.get(row, e) * B.get(e, col);
    
        C.get(row, col) = CValue;
    }
}

template<size_t N, size_t M, Place P>
__global__ void VecMatMulKernel(const Vector<N, P> v, const Matrix<N, M, P> A,
        Vector<M, P> res) {
    static_assert(P == PDEVICE, "WRONGE DEVICE");

    int row = blockIdx.x * blockDim.x + threadIdx.x;
    float value = 0;

    if(row >= M)
        return;
    
    for(int j = 0; j < N; j++)
        value += v.get(j) * A.get(j, row);
        
    res.get(row) = value;
}



template <size_t N, size_t M, Place P>
__host__ void VecMatMul(const Vector<N, P> &v, const Matrix<N, M, P> &A, Vector<M, P> &c)
{
    static_assert(P == PHOST, "WRONG DEVICE");
    for (int i = 0; i < M; i++)
    {
        c.get(i) = 0;
        for (int j = 0; j < N; j++)
            c.get(i) += v.get(j) * A.get(j, i);
    }
}
