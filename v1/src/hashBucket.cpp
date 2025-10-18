

#include "inc/hashBucket.h"

namespace SpMemoryPool {

MemoryPool& HashBucket::GetMemoryPool(int index)
{
    static MemoryPool memoryPools[MEMORY_POOL_NUM];
    return memoryPools[index];
}

void HashBucket::InitMemoryPool()
{
    for (int i = 0; i < MEMORY_POOL_NUM; ++i) {
        MemoryPool& pool = GetMemoryPool(i);
        pool.Init((i + 1) * SLOT_BASE_SIZE);
    }
}

size_t HashBucket::GetBlockCnt()
{
    size_t total = 0;
    for (int i = 0; i < MEMORY_POOL_NUM; ++i) {
        total += GetMemoryPool(i).GetBlockCnt();
    }
    return total;
}
size_t HashBucket::GetFreeSlotCnt()
{
    size_t total = 0;
    for (int i = 0; i < MEMORY_POOL_NUM; ++i) {
        total += GetMemoryPool(i).GetFreeSlotCnt();
    }
    return total;
}

void* HashBucket::Allocate(size_t size)
{
    if (size == 0)
        return nullptr;

    if (size > MAX_SLOT_SIZE)
        return aligned_alloc(size, size);

    return GetMemoryPool((size - 1) / SLOT_BASE_SIZE).Allocate();
}
void HashBucket::Deallocate(void* ptr, size_t size)
{
    if (!ptr || size == 0)
        return;

    if (size > MAX_SLOT_SIZE) {
        free(ptr);
        return;
    }
    GetMemoryPool((size - 1) / SLOT_BASE_SIZE).Deallocate(ptr);
}

}