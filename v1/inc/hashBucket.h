#ifndef __HASHBUCKET_H__
#define __HASHBUCKET_H__

#include "memoryPool.h"

namespace SpMemoryPool {

const size_t MEMORY_POOL_NUM = 64;
const size_t SLOT_BASE_SIZE = 8;
const size_t MAX_SLOT_SIZE = MEMORY_POOL_NUM * SLOT_BASE_SIZE;

class HashBucket {
public:
    template <class T, class... Args>
    friend T* NewElement(Args&&... args);

    template <class T>
    friend void DeleteElement(T* ptr);

    static MemoryPool& GetMemoryPool(int index);
    static void InitMemoryPool();

    //测试用
    static size_t GetBlockCnt();
    static size_t GetFreeSlotCnt();

private:
    static void* Allocate(size_t size);
    static void Deallocate(void* ptr, size_t size);
};

// 函数模板放在头文件中，编译时才能正常实例化
template <class T, class... Args>
T* NewElement(Args&&... args)
{
    void* addr = HashBucket::Allocate(sizeof(T));
    if (addr == nullptr)
        return nullptr;
    T* ret = new (addr) T(std::forward<Args>(args)...);
    return ret;
}

template <class T>
void DeleteElement(T* ptr)
{
    if (ptr) {
        ptr->~T();
        HashBucket::Deallocate(ptr, sizeof(T));
    }
}

}

#endif