/*
Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
// Collection of code to make sure that various features in the hip kernel language compile.

#include <hip_runtime.h>
#include <test_common.h>

#ifdef __HCC__
#include <amp.h>
#endif

// cudaA

// Simple tests for variable type qualifiers:
__device__ int deviceVar;

// TODO-HCC __constant__ not working yet.
__constant__ int constantVar1;

__constant__ __device__ int constantVar2; 

// Test HOST space:
__host__ void foo() {
    printf ("foo!\n");
}

__device__ __noinline__    int sum1_noinline(int a)    { return a+1;};
__device__ __forceinline__ int sum1_forceinline(int a) { return a+1;};


__device__ __host__ float PlusOne(float x) 
{
    return x + 1.0;
}

__global__ void MyKernel (const hipLaunchParm lp, const float *a, const float *b, float *c, unsigned N)
{
    //KERNELBEGIN;

    unsigned gid = hipThreadIdx_x;
    if (gid < N) {
        c[gid] = a[gid] + PlusOne(b[gid]);
    }

    //KERNELEND;
}


void callMyKernel()
{
    float *a, *b, *c;
    unsigned N;
    const unsigned blockSize = 256;

    hipLaunchKernel(MyKernel, dim3(N/blockSize), dim3(blockSize), 0, 0, a,b,c,N);
}


template <typename T>
__global__ void
vectorADD(const hipLaunchParm lp,
          T __restrict__ *A_d,
          T *B_d,
          T *C_d,
          size_t N)
{
//    KERNELBEGIN;
    int ws = warpSize;


    int zuzu = deviceVar + 1;


    int b = hipThreadIdx_x;
    int c;
#ifdef NOT_YET
    int a = __shfl_up(x, 1);
#endif

    float x;
    float z = sin(x);
#ifdef NOT_YET
    float fastZ = __sin(x);
#endif

#ifdef __HCC__
	// TODO - move to HIP atomics when ready.
    concurrency :: atomic_fetch_add(&c, b);
    //Concurrency::atomic_add_unsigned (&x, a);

    //concurrency ::atomic_add_ (x, a);
#endif

    __syncthreads();


    size_t offset = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);
    size_t stride = hipBlockDim_x * hipGridDim_x ;

    for (size_t i=offset; i<N; i+=stride) {
		C_d[i] = A_d[i] + B_d[i];
	}

//    KERNELEND;
}


int main() {
    printf ("Hello world\n");
    passed();
}
