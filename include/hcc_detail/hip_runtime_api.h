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
//#pragma once
#ifndef HIP_RUNTIME_API_H
#define HIP_RUNTIME_API_H
/**
 *  @file  hcc_detail/hip_runtime_api.h
 *  @brief Contains C function APIs for HIP runtime. This file does not use any HCC builtin or special language extensions (-hc mode) ; those functions in hip_runtime.h.
 */

#include <stdint.h>
#include <stddef.h>

#include <hip/hcc_detail/host_defines.h>
#include <hip/hip_runtime_api.h>
//#include "hip/hip_hcc.h"

#if defined (__HCC__) &&  (__hcc_workweek__ < 16155)
#error("This version of HIP requires a newer version of HCC.");
#endif

// Structure definitions:
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ihipStream_t *hipStream_t;
typedef struct hipEvent_t {
    struct ihipEvent_t *_handle;
} hipEvent_t;


/**
 * @addtogroup GlobalDefs More
 * @{
 */
//! Flags that can be used with hipStreamCreateWithFlags
#define hipStreamDefault            0x00 ///< Default stream creation flags. These are used with hipStreamCreate().
#define hipStreamNonBlocking        0x01 ///< Stream does not implicitly synchronize with null stream


//! Flags that can be used with hipEventCreateWithFlags:
#define hipEventDefault             0x0  ///< Default flags
#define hipEventBlockingSync        0x1  ///< Waiting will yield CPU.  Power-friendly and usage-friendly but may increase latency.
#define hipEventDisableTiming       0x2  ///< Disable event's capability to record timing information.  May improve performance.
#define hipEventInterprocess        0x4  ///< Event can support IPC.  @warning - not supported in HIP.


//! Flags that can be used with hipHostMalloc
#define hipHostMallocDefault        0x0
#define hipHostMallocPortable       0x1
#define hipHostMallocMapped         0x2
#define hipHostMallocWriteCombined  0x4

//! Flags that can be used with hipHostRegister
#define hipHostRegisterDefault      0x0  ///< Memory is Mapped and Portable
#define hipHostRegisterPortable     0x1  ///< Memory is considered registered by all contexts.  HIP only supports one context so this is always assumed true.
#define hipHostRegisterMapped       0x2  ///< Map the allocation into the address space for the current device.  The device pointer can be obtained with #hipHostGetDevicePointer.
#define hipHostRegisterIoMemory     0x4  ///< Not supported.


#define hipDeviceScheduleAuto       0x0
#define hipDeviceScheduleSpin       0x1
#define hipDeviceScheduleYield      0x2
#define hipDeviceBlockingSync       0x4
#define hipDeviceMapHost            0x8
#define hipDeviceLmemResizeToMax    0x16

/**
 * @warning On AMD devices and recent Nvidia devices, these hints and controls are ignored.
 */
typedef enum hipFuncCache {
    hipFuncCachePreferNone, ///< no preference for shared memory or L1 (default)
    hipFuncCachePreferShared, ///< prefer larger shared memory and smaller L1 cache
    hipFuncCachePreferL1, ///< prefer larger L1 cache and smaller shared memory
    hipFuncCachePreferEqual, ///< prefer equal size L1 cache and shared memory
} hipFuncCache;


/**
 * @warning On AMD devices and recent Nvidia devices, these hints and controls are ignored.
 */
typedef enum hipSharedMemConfig {
    hipSharedMemBankSizeDefault,   ///< The compiler selects a device-specific value for the banking.
    hipSharedMemBankSizeFourByte,  ///< Shared mem is banked at 4-bytes intervals and performs best when adjacent threads access data 4 bytes apart.
    hipSharedMemBankSizeEightByte  ///< Shared mem is banked at 8-byte intervals and performs best when adjacent threads access data 4 bytes apart.
} hipSharedMemConfig;



/**
 * Struct for data in 3D
 *
 */
typedef struct dim3 {
  uint32_t x;                 ///< x
  uint32_t y;                 ///< y
  uint32_t z;                 ///< z
#ifdef __cplusplus
  dim3(uint32_t _x=1, uint32_t _y=1, uint32_t _z=1) : x(_x), y(_y), z(_z) {};
#endif
} dim3;


/**
 * Memory copy types
 *
 */
typedef enum hipMemcpyKind {
   hipMemcpyHostToHost = 0    ///< Host-to-Host Copy
  ,hipMemcpyHostToDevice = 1  ///< Host-to-Device Copy
  ,hipMemcpyDeviceToHost = 2  ///< Device-to-Host Copy
  ,hipMemcpyDeviceToDevice =3 ///< Device-to-Device Copy
  ,hipMemcpyDefault = 4,      ///< Runtime will automatically determine copy-kind based on virtual addresses.
} hipMemcpyKind;




// Doxygen end group GlobalDefs
/**  @} */


//-------------------------------------------------------------------------------------------------


// The handle allows the async commands to use the stream even if the parent hipStream_t goes out-of-scope.
//typedef class ihipStream_t * hipStream_t;


/*
 * Opaque structure allows the true event (pointed at by the handle) to remain "live" even if the surrounding hipEvent_t goes out-of-scope.
 * This is handy for cases where the hipEvent_t goes out-of-scope but the true event is being written by some async queue or device */
//typedef struct hipEvent_t {
//    struct ihipEvent_t *_handle;
//} hipEvent_t;







/**
 *  @defgroup API HIP API
 *  @{
 *
 *  Defines the HIP API.  See the individual sections for more information.
 */



/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Device Device Management
 *  @{
 */

/**
 * @brief Blocks until the default device has completed all preceding requested tasks.
 *
 * This function waits for all streams on the default device to complete execution, and then returns.
 *
 * @see hipSetDevice, hipDeviceReset
*/
hipError_t hipDeviceSynchronize(void);



/**
 * @brief Destroy all resources and reset all state on the default device in the current process.
 *
 * Explicity destroy all memory allocations, events, and queues associated with the default device in the current process.
 *
 * This function will reset the device immmediately, and then return after all resources have been freed.
 * The caller must ensure that the device is not being accessed by any other host threads from the active process when this function is called.
 *
 * @see hipDeviceSynchronize
 */
hipError_t hipDeviceReset(void) ;


/**
 * @brief Set default device to be used for subsequent hip API calls from this thread.
 *
 * @param[in] device Valid device in range 0...hipGetDeviceCount().
 *
 * Sets @p device as the default device for the calling host thread.  Valid device id's are 0... (hipGetDeviceCount()-1).
 *
 * Many HIP APIs implicitly use the "default device" :
 *
 * - Any device memory subsequently allocated from this host thread (using hipMalloc) will be allocated on device.
 * - Any streams or events created from this host thread will be associated with device.
 * - Any kernels launched from this host thread (using hipLaunchKernel) will be executed on device (unless a specific stream is specified,
 * in which case the device associated with that stream will be used).
 *
 * This function may be called from any host thread.  Multiple host threads may use the same device.
 * This function does no synchronization with the previous or new device, and has very little runtime overhead.
 * Applications can use hipSetDevice to quickly switch the default device before making a HIP runtime call which uses the default device.
 *
 * The default device is stored in thread-local-storage for each thread.
 * Thread-pool implementations may inherit the default device of the previous thread.  A good practice is to always call hipSetDevice
 * at the start of HIP coding sequency to establish a known standard device.
 *
 * @see hipGetDevice, hipGetDeviceCount
 */
hipError_t hipSetDevice(int device);


/**
 * @brief Return the default device id for the calling host thread.
 *
 * @param [out] device *device is written with the default device
 *
 * HIP maintains an default device for each thread using thread-local-storage.
 * This device is used implicitly for HIP runtime APIs called by this thread.
 * hipGetDevice returns in * @p device the default device for the calling host thread.
 *
 * @see hipSetDevice, hipGetDevicesizeBytes
 */
hipError_t hipGetDevice(int *device);


/**
 * @brief Return number of compute-capable devices.
 * @param [output] count Returns number of compute-capable devices.
 *
 * Returns in @p *count the number of devices that have ability to run compute commands.  If there are no such devices, then @ref hipGetDeviceCount will return #hipErrorNoDevice.
 * If 1 or more devices can be found, then hipGetDeviceCount returns #hipSuccess.
 */
hipError_t hipGetDeviceCount(int *count);

/**
 * @brief Query device attribute.
 * @param [out] pi pointer to value to return
 * @param [in] attr attribute to query
 * @param [in] device which device to query for information
 */
hipError_t hipDeviceGetAttribute(int* pi, hipDeviceAttribute_t attr, int device);

/**
 * @brief Returns device properties.
 *
 * @param [out] prop written with device properties
 * @param [in]  device which device to query for information
 *
 * Populates hipGetDeviceProperties with information for the specified device.
 */
hipError_t hipGetDeviceProperties(hipDeviceProp_t* prop, int device);



//Cache partitioning functions:

/**
 * @brief Set L1/Shared cache partition.
 *
 * Note: AMD devices and recent Nvidia GPUS do not support reconfigurable cache.  This hint is ignored on those architectures.
 *
 */
hipError_t hipDeviceSetCacheConfig ( hipFuncCache cacheConfig );


/**
 * @brief Set Cache configuration for a specific function
 *
 * Note: AMD devices and recent Nvidia GPUS do not support reconfigurable cache.  This hint is ignored on those architectures.
 *
 */
hipError_t hipDeviceGetCacheConfig ( hipFuncCache *cacheConfig );


/**
 * @brief Set Cache configuration for a specific function
 *
 * Note: AMD devices and recent Nvidia GPUS do not support reconfigurable cache.  This hint is ignored on those architectures.
 *
 */
hipError_t hipFuncSetCacheConfig ( hipFuncCache config );

//---
//Shared bank config functions:

/**
 * @brief Get Shared memory bank configuration.
 *
 * Note: AMD devices and recent Nvidia GPUS do not support shared cache banking, and the hint is ignored on those architectures.
 *
 */
hipError_t hipDeviceGetSharedMemConfig ( hipSharedMemConfig * pConfig );


/**
 * @brief Set Shared memory bank configuration.
 *
 * Note: AMD devices and recent Nvidia GPUS do not support shared cache banking, and the hint is ignored on those architectures.
 *
 */
hipError_t hipDeviceSetSharedMemConfig ( hipSharedMemConfig config );

/**
 * @brief Set Device flags
 *
 * Note: Only hipDeviceScheduleAuto and hipDeviceMapHost are supported
 *
*/
hipError_t hipSetDeviceFlags ( unsigned flags);

// end doxygen Device
/**
 * @}
 */


/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Error Error Handling
 *  @{
 */

/**
 * @brief Return last error returned by any HIP runtime API call and resets the stored error code to #hipSuccess
 *
 * Returns the last error that has been returned by any of the runtime calls in the same host thread,
 * and then resets the saved error to #hipSuccess.
 *
 */
hipError_t hipGetLastError(void);


/**
 * @brief Return last error returned by any HIP runtime API call.
 *
 * @return #hipSuccess
 *
 * Returns the last error that has been returned by any of the runtime calls in the same host thread.
 * Unlike hipGetLastError, this function does not reset the saved error code.
 *
 *
 *
 */
hipError_t hipPeekAtLastError(void);



/**
 * @brief Return name of the specified error code in text form.
 *
 * @param hip_error Error code to convert to name.
 * @return const char pointer to the NULL-terminated error name
 *
 * @see hipGetErrorString, hipGetLastError, hipPeakAtLastError, hipError_t
 */
const char *hipGetErrorName(hipError_t hip_error);


/**
 * @brief Return handy text string message to explain the error which occurred
 *
 * @param hip_error Error code to convert to string.
 * @return const char pointer to the NULL-terminated error string
 *
 * @warning : on HCC, this function returns the name of the error (same as hipGetErrorName)
 *
 * @see hipGetErrorName, hipGetLastError, hipPeakAtLastError, hipError_t
 */
const char *hipGetErrorString(hipError_t hip_error);

// end doxygen Error
/**
 * @}
 */



/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Stream Stream Management
 *  @{
 *
 *  The following Stream APIs are not (yet) supported in HIP:
 *  - cudaStreamCreateWithPriority
 *  - cudaStreamGetPriority
 */

/**
 * @brief Create an asynchronous stream.
 *
 * @param[in, out] stream Pointer to new stream
 * @param[in ] flags to control stream creation.
 * @return #hipSuccess, #hipErrorInvalidValue
 *
 * Create a new asynchronous stream.  @p stream returns an opaque handle that can be used to reference the newly
 * created stream in subsequent hipStream* commands.  The stream is allocated on the heap and will remain allocated 
 *
 * even if the handle goes out-of-scope.  To release the memory used by the stream, applicaiton must call hipStreamDestroy.
 * Flags controls behavior of the stream.  See #hipStreamDefault, #hipStreamNonBlocking.
 * @error hipStream_t are under development - with current HIP use the NULL stream.
 */

hipError_t hipStreamCreateWithFlags(hipStream_t *stream, unsigned int flags);



/**
 * @brief Create an asynchronous stream.
 *
 * @param[in, out] stream Valid pointer to hipStream_t.  This function writes the memory with the newly created stream.
 * @return #hipSuccess, #hipErrorInvalidValue
 *
 * Create a new asynchronous stream.  @p stream returns an opaque handle that can be used to reference the newly
 * created stream in subsequent hipStream* commands.  The stream is allocated on the heap and will remain allocated 
 * even if the handle goes out-of-scope.  To release the memory used by the stream, applicaiton must call hipStreamDestroy.
 * 
 *
 * @see hipStreamDestroy
 *
 */
hipError_t hipStreamCreate(hipStream_t *stream);


/**
 * @brief Make the specified compute stream wait for an event
 *
 * @param[in] stream stream to make wait.
 * @param[in] event event to wait on
 * @param[in] flags control operation [must be 0]
 *
 * @return #hipSuccess, #hipErrorInvalidResourceHandle
 *
 * This function inserts a wait operation into the specified stream.
 * All future work submitted to @p stream will wait until @p event reports completion before beginning execution.
 * This function is host-asynchronous and the function may return before the wait has completed.
 *
 *
 */
hipError_t hipStreamWaitEvent(hipStream_t stream, hipEvent_t event, unsigned int flags);


/**
 * @brief Wait for all commands in stream to complete.
 *
 * If the null stream is specified, this command blocks until all
 *
 * This command honors the hipDeviceLaunchBlocking flag, which controls whether the wait is active or blocking.
 *
 * This command is host-synchronous : the host will block until the stream is empty.
 *
 * TODO
 */
hipError_t hipStreamSynchronize(hipStream_t stream);


/**
 * @brief Destroys the specified stream.
 *
 * @param[in, out] stream Valid pointer to hipStream_t.  This function writes the memory with the newly created stream.
 * @return #hipSuccess
 *
 * Destroys the specified stream.
 *
 * If commands are still executing on the specified stream, some may complete execution before the queue is deleted.
 *
 * The queue may be destroyed while some commands are still inflight, or may wait for all commands queued to the stream
 * before destroying it.
 */
hipError_t hipStreamDestroy(hipStream_t stream);


/**
 * @brief Return flags associated with this stream.
 *
 * @param[in] stream
 * @param[in,out] flags
 * @return #hipSuccess, #hipErrorInvalidValue, #hipErrorInvalidResourceHandle
 *
 * Return flags associated with this stream in *@p flags.
 *
 * @see hipStreamCreateWithFlags
 *
 * @returns #hipSuccess
 */
hipError_t hipStreamGetFlags(hipStream_t stream, unsigned int *flags);


// end doxygen Stream
/**
 * @}
 */




/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Event Event Management
 *  @{
 */

/**
 * @brief Create an event with the specified flags
 *
 * @param[in,out] event Returns the newly created event.
 * @param[in] flags     Flags to control event behavior.  #hipEventDefault, #hipEventBlockingSync, #hipEventDisableTiming, #hipEventInterprocess
 *
 * @warning On HCC platform, #hipEventInterprocess is not supported.
 *
 * @returns #cudaSuccess
 */
hipError_t hipEventCreateWithFlags(hipEvent_t* event, unsigned flags);


/**
 *  Create an event
 *
 * @param[in,out] event Returns the newly created event.
 *
 */
hipError_t hipEventCreate(hipEvent_t* event);


/**
 * @brief Record an event in the specified stream.
 *
 * @param[in] event event to record.
 * @param[in] stream stream in which to record event.
 * @returns #hipSuccess, #hipErrorInvalidResourceHandle
 *
 * hipEventQuery or hipEventSynchronize must be used to determine when the event
 * transitions from "recording" (after eventRecord is called) to "recorded"
 * (when timestamps are set, if requested).
 *
 * Events which are recorded in a non-NULL stream will transition to
 * from recording to "recorded" state when they reach the head of
 * the specified stream, after all previous
 * commands in that stream have completed executing.
 *
 * If hipEventRecord has been previously called aon event, then this call will overwrite any existing state in event.
 *
 * If this function is called on a an event that is currently being recorded, results are undefined - either
 * outstanding recording may save state into the event, and the order is not guaranteed.  This shoul be avoided.
 *
 * @see hipEventElapsedTime
 *
 */
#ifdef __cplusplus
hipError_t hipEventRecord(hipEvent_t event, hipStream_t stream = NULL);
#else
hipError_t hipEventRecord(hipEvent_t event, hipStream_t stream);
#endif

/**
 *  @brief Destroy the specified event.
 *
 *  @param[in] event Event to destroy.
 *  @return : #hipSuccess,
 *
 *  Releases memory associated with the event.  If the event is recording but has not completed recording when hipEventDestroy is called,
 *  the function will return immediately and the completion_future resources will be released later, when the hipDevice is synchronized.
 *
 */
hipError_t hipEventDestroy(hipEvent_t event);


/**
 *  @brief: Wait for an event to complete.
 *
 *  This function will block until the event is ready, waiting for all previous work in the stream specified when event was recorded with hipEventRecord.
 *
 *  If hipEventRecord has not been called on @p event, this function returns immediately.
 *
 *  TODO-hcc - This function needs to support hipEventBlockingSync parameter.
 *
 *  @param[in] event Event on which to wait.
 *  @return #hipSuccess, #hipErrorInvalidResourceHandle,
 *
 */
hipError_t hipEventSynchronize(hipEvent_t event);


/**
 * @brief Return the elapsed time between two events.
 *
 * @param[out]] ms : Return time between start and stop in ms.
 * @param[in]   start : Start event.
 * @param[in]   stop  : Stop event.
 * @return : #hipSuccess, #hipErrorInvalidResourceHandle, #hipErrorNotReady,
 *
 * Computes the elapsed time between two events. Time is computed in ms, with
 * a resolution of approximately 1 us.
 *
 * Events which are recorded in a NULL stream will block until all commands
 * on all other streams complete execution, and then record the timestamp.
 *
 * Events which are recorded in a non-NULL stream will record their timestamp
 * when they reach the head of the specified stream, after all previous
 * commands in that stream have completed executing.  Thus the time that
 * the event recorded may be significantly after the host calls hipEventRecord.
 *
 * If hipEventRecord has not been called on either event, then #hipErrorInvalidResourceHandle is returned.
 * If hipEventRecord has been called on both events, but the timestamp has not yet been recorded on one or
 * both events (that is, hipEventQuery would return #hipErrorNotReady on at least one of the events), then
 * #hipErrorNotReady is returned.
 */
hipError_t hipEventElapsedTime(float *ms, hipEvent_t start, hipEvent_t stop);


/**
 * @brief Query event status
 *
 * @param[in] event Event to query.
 * @returns #hipSuccess, hipEventNotReady
 *
 * Query the status of the specified event.  This function will return #hipErrorNotReady if all commands
 * in the appropriate stream (specified to hipEventRecord) have completed.  If that work has not completed,
 * or if hipEventRecord was not called on the event, then hipSuccess is returned.
 *
 *
 */
hipError_t hipEventQuery(hipEvent_t event) ;


// end doxygen Events
/**
 * @}
 */



/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Memory Memory Management
 *  @{
 *
 *  The following CUDA APIs are not currently supported:
 *  - cudaMalloc3D
 *  - cudaMalloc3DArray
 *  - TODO - more 2D, 3D, array APIs here.
 *
 *
 */


/**
 *  @brief Return attributes for the specified pointer
 */
hipError_t hipPointerGetAttributes(hipPointerAttribute_t *attributes, void* ptr);

/**
 *  @brief Allocate memory on the default accelerator
 *
 *  @param[out] ptr Pointer to the allocated memory
 *  @param[in]  size Requested memory size
 *  @return #hipSuccess
 */
hipError_t hipMalloc(void** ptr, size_t size) ;


/**
 *  @brief Allocate pinned host memory
 *
 *  @param[out]  ptr Pointer to the allocated host pinned memory
 *  @param[in] size Requested memory size
 *  @return Error code
 */
hipError_t hipMallocHost(void** ptr, size_t size) __attribute__((deprecated("use hipHostMalloc instead"))) ;

/**
 *  @brief Allocate device accessible page locked host memory 
 *
 *  @param[out]  ptr Pointer to the allocated host pinned memory
 *  @param[in] size Requested memory size
 *  @param[in] flags Type of host memory allocation
 *  @return Error code
 */
hipError_t hipHostMalloc(void** ptr, size_t size, unsigned int flags) ;
hipError_t hipHostAlloc(void** ptr, size_t size, unsigned int flags) __attribute__((deprecated("use hipHostMalloc instead"))) ;;

/**
 *  @brief Get Device pointer from Host Pointer allocated through hipHostAlloc
 *
 *  @param[out]  dstPtr Device Pointer mapped to passed host pointer
 *  @param[in] hstPtr Host Pointer allocated through hipHostAlloc
 *  @param[in] flags Flags to be passed for extension
 *  @return Error code
 */
hipError_t hipHostGetDevicePointer(void** devPtr, void* hstPtr, unsigned int flags) ;

/**
 *  @brief Get flags associated with host pointer
 *
 *  @param[out]  flagsPtr Memory location to store flags
 *  @param[in] hostPtr Host Pointer allocated through hipHostMalloc
 *  @return Error code
 */
hipError_t hipHostGetFlags(unsigned int* flagsPtr, void* hostPtr) ;

/**
 *  @brief Register host memory so it can be accessed from the current device.
 *
 *  @param[out] hostPtr Pointer to host memory to be registered.
 *  @param[in] sizeBytes size of the host memory
 *  @param[in] flags.  See below.
 *
 *  Flags:
 *  - #hipHostRegisterDefault   Memory is Mapped and Portable
 *  - #hipHostRegisterPortable  Memory is considered registered by all contexts.  HIP only supports one context so this is always assumed true.
 *  - #hipHostRegisterMapped    Map the allocation into the address space for the current device.  The device pointer can be obtained with #hipHostGetDevicePointer.
 *
 *
 *  After registering the memory, use #hipHostGetDevicePointer to obtain the mapped device pointer.  
 *  On many systems, the mapped device pointer will have a different value than the mapped host pointer.  Applications
 *  must use the device pointer in device code, and the host pointer in device code.  
 *
 *  On some systems, registered memory is pinned.  On some systems, registered memory may not be actually be pinned
 *  but uses OS or hardware facilities to all GPU access to the host memory.
 *
 *  Developers are strongly encouraged to register memory blocks which are aligned to the host cache-line size.
 *  (typically 64-bytes but can be obtains from the CPUID instruction).
 *
 *  If registering non-aligned pointers, the application must take care when register pointers from the same cache line 
 *  on different devices.  HIP's coarse-grained synchronization model does not guarantee correct results if different
 *  devices write to different parts of the same cache block - typically one of the writes will "win" and overwrite data
 *  from the other registered memory region.
 *
 *  @return #hipSuccess, #hipErrorMemoryAllocation
 */
hipError_t hipHostRegister(void* hostPtr, size_t sizeBytes, unsigned int flags) ;

/**
 *  @brief Un-register host pointer
 *
 *  @param[in] hostPtr Host pointer previously registered with #hipHostRegister
 *  @return Error code
 */
hipError_t hipHostUnregister(void* hostPtr) ;

/**
 *  Allocates at least width (in bytes) * height bytes of linear memory
 *  Padding may occur to ensure alighnment requirements are met for the given row
 *  The change in width size due to padding will be returned in *pitch.
 *  Currently the alignment is set to 128 bytes
 *
 *  @param[out] ptr Pointer to the allocated device memory
 *  @param[out] pitch Pitch for allocation (in bytes)
 *  @param[in]  width Requested pitched allocation width (in bytes)
 *  @param[in]  height Requested pitched allocation height
 *  @return Error code
 */

hipError_t hipMallocPitch(void** ptr, size_t* pitch, size_t width, size_t height);

/**
 *  @brief Free memory allocated by the hcc hip memory allocation API.
 *  This API performs an implicit hipDeviceSynchronize() call.
 *  If pointer is NULL, the hip runtime is initialized and hipSuccess is returned.
 *
 *  @param[in] ptr Pointer to memory to be freed
 *  @return #hipSuccess 
 *  @return #hipErrorInvalidDevicePointer (if pointer is invalid, including host pointers allocated with hipHostMalloc)
 */
hipError_t hipFree(void* ptr);



/**
 *  @brief Free memory allocated by the hcc hip host memory allocation API.  [Deprecated.]
 *
 *  @see hipHostFree
 */
hipError_t hipFreeHost(void* ptr) __attribute__((deprecated("use hipHostFree instead")))  ;


/**
 *  @brief Free memory allocated by the hcc hip host memory allocation API
 *  This API performs an implicit hipDeviceSynchronize() call.
 *  If pointer is NULL, the hip runtime is initialized and hipSuccess is returned.
 *
 *  @param[in] ptr Pointer to memory to be freed
 *  @return #hipSuccess, 
 *          #hipErrorInvalidValue (if pointer is invalid, including device pointers allocated with hipMalloc)
 */
hipError_t hipHostFree(void* ptr);



/**
 *  @brief Copy data from src to dst.
 *
 *  It supports memory from host to device,
 *  device to host, device to device and host to host
 *  The src and dst must not overlap.
 *
 *  For hipMemcpy, the copy is always performed by the current device (set by hipSetDevice).
 *  For multi-gpu or peer-to-peer configurations, it is recommended to set the current device to the device where the src data is physically located.
 *  For optimal peer-to-peer copies, the copy device must be able to access the src and dst pointers (by calling hipDeviceEnablePeerAccess with copy agent as the 
 *  current device and src/dest as the peerDevice argument.  if this is not done, the hipMemcpy will still work, but will perform the copy using a staging buffer
 *  on the host.
 *
 *  @param[out]  dst Data being copy to
 *  @param[in]  src Data being copy from
 *  @param[in]  sizeBytes Data size in bytes
 *  @param[in]  copyType Memory copy type
 *  @return #hipSuccess, #hipErrorInvalidValue, #hipErrorMemoryFree, #hipErrorUnknown
 */
hipError_t hipMemcpy(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind);


/**
 *  @brief Copies @p sizeBytes bytes from the memory area pointed to by @p src to the memory area pointed to by @p offset bytes from the start of symbol @p symbol.
 *
 *  The memory areas may not overlap. Symbol can either be a variable that resides in global or constant memory space, or it can be a character string,
 *  naming a variable that resides in global or constant memory space. Kind can be either hipMemcpyHostToDevice or hipMemcpyDeviceToDevice
 *  TODO: cudaErrorInvalidSymbol and cudaErrorInvalidMemcpyDirection is not supported, use hipErrorUnknown for now.  
 *
 *  @param[in]  symbolName - Symbol destination on device
 *  @param[in]  src - Data being copy from
 *  @param[in]  sizeBytes - Data size in bytes
 *  @param[in]  offset - Offset from start of symbol in bytes
 *  @param[in]  kind - Type of transfer
 *  @return #hipSuccess, #hipErrorInvalidValue, #hipErrorMemoryFree, #hipErrorUnknown
 */
hipError_t hipMemcpyToSymbol(const char* symbolName, const void *src, size_t sizeBytes, size_t offset, hipMemcpyKind kind);


/**
 *  @brief Copy data from src to dst asynchronously.
 *
 *  @warning If host or dest are not pinned, the memory copy will be performed synchronously.  For best performance, use hipHostMalloc to
 *  allocate host memory that is transferred asynchronously.
 *
 *  For hipMemcpy, the copy is always performed by the device associated with the specified stream.
 *
 *  For multi-gpu or peer-to-peer configurations, it is recommended to use a stream which is a attached to the device where the src data is physically located.
 *  For optimal peer-to-peer copies, the copy device must be able to access the src and dst pointers (by calling hipDeviceEnablePeerAccess with copy agent as the 
 *  current device and src/dest as the peerDevice argument.  if this is not done, the hipMemcpy will still work, but will perform the copy using a staging buffer
 *  on the host.
 *
 *  @param[out] dst Data being copy to
 *  @param[in]  src Data being copy from
 *  @param[in]  sizeBytes Data size in bytes
 *  @param[in]  accelerator_view Accelerator view which the copy is being enqueued
 *  @return #hipSuccess, #hipErrorInvalidValue, #hipErrorMemoryFree, #hipErrorUnknown
 */
#if __cplusplus
hipError_t hipMemcpyAsync(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind, hipStream_t stream=0);
#else
hipError_t hipMemcpyAsync(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind, hipStream_t stream);
#endif

/**
 *  @brief Copy data from src to dst asynchronously.
 *
 * It supports memory from host to device,
 *  device to host, device to device and host to host.
 *
 *  @param[out] dst Data being copy to
 *  @param[in]  src Data being copy from
 *  @param[in]  sizeBytes Data size in bytes
 *  @param[in]  accelerator_view Accelerator view which the copy is being enqueued
 *  @return #hipSuccess, #hipErrorInvalidValue, #hipErrorMemoryFree
 */
hipError_t hipMemset(void* dst, int  value, size_t sizeBytes );


/**
 *  @brief Fills the first sizeBytes bytes of the memory area pointed to by dev with the constant byte value value.
 *
 *  hipMemsetAsync() is asynchronous with respect to the host, so the call may return before the memset is complete.
 *  The operation can optionally be associated to a stream by passing a non-zero stream argument.
 *  If stream is non-zero, the operation may overlap with operations in other streams.
 *
 *  @param[out] dst Pointer to device memory
 *  @param[in]  value - Value to set for each byte of specified memory
 *  @param[in]  sizeBytes - Size in bytes to set
 *  @param[in]  stream - Stream identifier
 *  @return #hipSuccess, #hipErrorInvalidValue, #hipErrorMemoryFree
 */
#if __cplusplus
hipError_t hipMemsetAsync(void* dst, int  value, size_t sizeBytes, hipStream_t = 0 );
#else
hipError_t hipMemsetAsync(void* dst, int value, size_t sizeBytes, hipStream_t stream);
#endif

/**
 * @brief Query memory info.
 * Return snapshot of free memory, and total allocatable memory on the device.
 *
 * Returns in *free a snapshot of the current free memory o
 **/
hipError_t hipMemGetInfo  (size_t * free, size_t * total)   ;

// doxygen end Memory
/**
 * @}
 */



/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup PeerToPeer Device Memory Access
 *  @{
 *
 *  @warning PeerToPeer support is experimental.
 *
 */

/**
 * @brief Determine if a device can access a peer's memory.
 *
 * @param [out] canAccessPeer Returns the peer access capability (0 or 1)
 * @param [in] device - device from where memory may be accessed.
 * @param [in] peerDevice - device where memory is physically located
 *
 * Returns "1" in @p canAccessPeer if the specified @p device is capable
 * of directly accessing memory physically located on peerDevice , or "0" if not.
 *
 * Returns "0" in @p canAccessPeer if deviceId == peerDeviceId, and both are valid devices : a device is not a peer of itself.
 *
 * @returns #hipSuccess, 
 * @returns #hipErrorInvalidDevice if deviceId or peerDeviceId are not valid devices
 * @warning PeerToPeer support is experimental.
 */
hipError_t hipDeviceCanAccessPeer (int* canAccessPeer, int deviceId, int peerDeviceId);


/**
 * @brief Enable direct access from current device's virtual address space to memory allocations physically located on a peer device.  
 *
 * Memory which already allocated on peer device will be mapped into the address space of the current device.  In addition, all
 * future memory allocations on peerDeviceId will be mapped into the address space of the current device when the memory is allocated.
 * The peer memory remains accessible from the current device until a call to hipDeviceDisablePeerAccess or hipDeviceReset.
 *
 *
 * @param [in] peerDeviceId
 * @param [in] flags
 *
 * Returns #hipSuccess, #hipErrorInvalidDevice, #hipErrorInvalidValue, 
 * @returns #hipErrorPeerAccessAlreadyEnabled if peer access is already enabled for this device.
 * @warning PeerToPeer support is experimental.
 */
hipError_t  hipDeviceEnablePeerAccess (int  peerDeviceId, unsigned int flags);


/**
 * @brief Disable direct access from current device's virtual address space to memory allocations physically located on a peer device.  
 *
 * Returns hipErrorPeerAccessNotEnabled if direct access to memory on peerDevice has not yet been enabled from the current device.
 *
 * @param [in] peerDeviceId
 *
 * @returns #hipSuccess, #hipErrorPeerAccessNotEnabled
 * @warning PeerToPeer support is experimental.
 */
hipError_t  hipDeviceDisablePeerAccess (int peerDeviceId);


#ifdef PEER_NON_UNIFIED
/**
 * @brief Copies memory from one device to memory on another device.
 *
 * @param [out] dst - Destination device pointer.
 * @param [in] dstDeviceId - Destination device
 * @param [in] src - Source device pointer
 * @param [in] srcDeviceId - Source device
 * @param [in] sizeBytes - Size of memory copy in bytes
 *
 * @returns #hipSuccess, #hipErrorInvalidValue, #hipErrorInvalidDevice
 * @warning PeerToPeer support is experimental.
 */
hipError_t hipMemcpyPeer (void* dst, int dstDeviceId, const void* src, int srcDeviceId, size_t sizeBytes);

/**
 * @brief Copies memory from one device to memory on another device.
 *
 * @param [out] dst - Destination device pointer.
 * @param [in] dstDevice - Destination device
 * @param [in] src - Source device pointer
 * @param [in] srcDevice - Source device
 * @param [in] sizeBytes - Size of memory copy in bytes
 * @param [in] stream - Stream identifier
 *
 * @returns #hipSuccess, #hipErrorInvalidValue, #hipErrorInvalidDevice
 */
#if __cplusplus
hipError_t hipMemcpyPeerAsync ( void* dst, int  dstDeviceId, const void* src, int  srcDevice, size_t sizeBytes, hipStream_t stream=0 );
#else
hipError_t hipMemcpyPeerAsync(void* dst, int dstDevice, const void* src, int srcDevice, size_t sizeBytes, hipStream_t stream);
#endif
#endif


// doxygen end PeerToPeer
/**
 * @}
 */



/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Version Management
 *  @{
 *
 */

/**
 * @brief Returns the approximate HIP driver version.
 *
 * @warning The HIP feature set does not correspond to an exact CUDA SDK driver revision.
 * This function always set *driverVersion to 4 as an approximation though HIP supports
 * some features which were introduced in later CUDA SDK revisions.
 * HIP apps code should not rely on the driver revision number here and should
 * use arch feature flags to test device capabilities or conditional compilation.
 *
 */
hipError_t hipDriverGetVersion(int *driverVersion) ;



// doxygen end Version Management
/**
 * @}
 */


/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup Profiler Control
 *  @{
 *
 *
 *  @warning The cudaProfilerInitialize API format for "configFile" is not supported.
 *
 *  On AMD platforms, hipProfilerStart and hipProfilerStop require installation of AMD's GPU
 *  perf counter API and defining GPU_PERF
 */


/**
 * @}
 */




#ifdef __cplusplus
} /* extern "c" */
#endif


/**
 *-------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------
 *  @defgroup HCC_Specific HCC-Specific Accessors
 *  @{
 *
 * The following calls are only supported when compiler HIP with HCC.
 * To produce portable code, use of these calls must be guarded #ifdef checks:
 * @code
 * #ifdef __HCC__
 *  hc::accelerator acc;
    hipError_t err = hipHccGetAccelerator(deviceId, &acc)
 * #endif
 * @endcode
 *
 */

// end-group HCC_Specific
/**
 * @}
 */



// doxygen end HIP API
/**
 *   @}
 */

#endif
