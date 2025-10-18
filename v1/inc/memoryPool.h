#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_


#include <mutex>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>


namespace SpMemoryPool {
class MemoryPool {
public:
    MemoryPool(size_t blockSize = 4096);
    ~MemoryPool();

    void Init(size_t slotSize);

    void* Allocate();
    void Deallocate(void* ptr);

    size_t GetBlockCnt()
    {
        return m_blockCnt;
    }
    size_t GetFreeSlotCnt()
    {
        return m_freeSlotCnt;
    }

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

    std::mutex m_blockMutex, m_freeListMutex;
    size_t m_blockSize;
    size_t m_slotSize;
    // 大内存块链表
    Slot* m_firstBlock;
    Slot* m_curSlot;
    Slot* m_lastSlot;
    // 空闲空间链表
    Slot* m_freeList;

    // 错误检查
    size_t m_blockCnt;
    size_t m_freeSlotCnt;
};

} // namespace SP_MemoryPool

#endif //_MEMORY_POOL_H_