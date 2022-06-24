#include <stdio.h>
#include <dlfcn.h>

#include "hooking.h"

extern "C"
{
    void *__libc_dlsym(void *map, const char *name);
}
extern "C"
{
    void *__libc_dlopen_mode(const char *name, int mode);
}
typedef void *(*fn_dlsym)(void *, const char *);
void *real_dlsym(void *handle, const char *symbol);

CUresult cuLaunchKernel( CUfunction f, unsigned int  gridDimX, unsigned int  gridDimY, unsigned int  gridDimZ, unsigned int  blockDimX, unsigned int  blockDimY, unsigned int  blockDimZ, unsigned int  sharedMemBytes, CUstream hStream, void** kernelParams, void** extra)
{
    printf("wrapped cuLaunchKernel start\n");
    static fn_cuLaunchKernel orig_cuLaunchKernel = NULL;
    static void *handle = NULL;
    CUresult err = CUDA_ERROR_INVALID_VALUE;

    if (!handle)
    {
        handle = __libc_dlopen_mode("libcuda.so.1", RTLD_LAZY);
        orig_cuLaunchKernel = (fn_cuLaunchKernel)real_dlsym(handle, "cuLaunchKernel");
    }
    if (orig_cuLaunchKernel == NULL)
    {
        printf(" orig function is NULL ptr!\n");
        return err;
    }

    printf("grid X: %u, grid Y: %u, grid Z: %u, ", gridDimX, gridDimY, gridDimZ);
    printf("block X: %u, block Y: %u, block Z: %u, ", blockDimX, blockDimY, blockDimZ);
    printf("sharedMemBytes: %u\n", sharedMemBytes);
    

    err = orig_cuLaunchKernel(f, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, sharedMemBytes, hStream, kernelParams, extra);    

    return err;
}

CUresult cuStreamSynchronize ( CUstream hStream )
{
    printf("wrapped cuStreamSynchronize start\n");
    static fn_cuStreamSynchronize orig_cuStreamSynchronize = NULL;
    static void *handle = NULL;
    CUresult err = CUDA_ERROR_INVALID_VALUE;

    if (!handle)
    {
        handle = __libc_dlopen_mode("libcuda.so.1", RTLD_LAZY);
        orig_cuStreamSynchronize = (fn_cuStreamSynchronize)real_dlsym(handle, "cuStreamSynchronize");
    }
    if (orig_cuStreamSynchronize == NULL)
    {
        printf(" orig function is NULL ptr!\n");
        return err;
    }


    err = orig_cuStreamSynchronize( hStream );    

    return err;
}

void *real_dlsym(void *handle, const char *symbol)
{
    static fn_dlsym internal_dlsym = (fn_dlsym)__libc_dlsym(__libc_dlopen_mode("libdl.so.2", RTLD_LAZY), "dlsym");
    return (*internal_dlsym)(handle, symbol);
}