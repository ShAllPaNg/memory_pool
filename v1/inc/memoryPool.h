#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>
namespace SP_MemoryPool {

const size_t MEMORY_POOL_NUM = 64;
const size_t SLOT_BASE_SIZE = 8;
const size_t MAX_SLOT_SIZE = MEMORY_POOL_NUM * SLOT_BASE_SIZE;

class MemoryPool {
public:
    MemoryPool(size_t blockSize = 4096);
    ~MemoryPool();

    void Init(size_t slotSize);

    void* Allocate();
    void Deallocate(void* ptr);

private:
    union Slot {
        Slot* next;
    };

    void* PopFreeList();
    void PushFreeList(void* ptr);

    bool AllocateNewBlock();
    inline size_t Padding(void* addr, size_t align)
    {
        return align - (reinterpret_cast<size_t>(addr) % align);
    }

    size_t m_blockSize;
    size_t m_slotSize;
    // 大内存块链表
    Slot* m_firstBlock;
    Slot* m_curSlot;
    Slot* m_lastSlot;
    // 空闲空间链表
    Slot* m_freeList;
};

class HashBucket {
public:
    template <class T, class... Args>
    friend T* NewElement(Args&&... args);

    template <class T>
    friend void DeleteElement(T* ptr);

private:
    static MemoryPool& GetMemoryPoll(int index);
    static void InitMemoryPoll();

    static void* Allocate(size_t size);
    static void Deallocate(void* ptr, size_t size);
};

template <class T, class... Args>
T* NewElement(Args&&... args);

template <class T>
void DeleteElement(T* ptr);

} // namespace SP_MemoryPool

#endif //_MEMORY_POOL_H_