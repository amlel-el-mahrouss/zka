/* -------------------------------------------

    Copyright ZKA Technologies.

    Purpose: DDK allocator.

------------------------------------------- */

#include <DDK/KernelStd.h>

/**
    \brief Allocates a new heap on the kernel's side.
    \param sz the size of the heap block.
    \return the newly allocated pointer.
*/
DK_EXTERN void* kernelAlloc(size_t sz)
{
    if (!sz) ++sz;

	void* ptr = kernelCall("NewHeap", 1, &sz, sizeof(size_t));

    return ptr;
}

/**
    \brief Frees a pointer from the heap.
    \param ptr the pointer to free.
*/
DK_EXTERN void kernelFree(void* ptr)
{
    if (!ptr) return;

	kernelCall("DeleteHeap", 1, ptr, 0);
}