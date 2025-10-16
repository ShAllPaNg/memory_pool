#include "inc/memoryPool.h"

namespace SpMemoryPool {
MemoryPool::MemoryPool(size_t blockSize)
    : m_blockSize(blockSize)
    , m_slotSize(0)
    , m_firstBlock(nullptr)
    , m_curSlot(nullptr)
    , m_lastSlot(nullptr)
    , m_freeList(nullptr)
{
}

MemoryPool::~MemoryPool()
{
    while (m_firstBlock != nullptr) {
        Slot* next = m_firstBlock->next;
        free(m_firstBlock);
        m_firstBlock = next;
    }
}

void MemoryPool::Init(size_t slotSize)
{
    m_slotSize = slotSize;
}

void* MemoryPool::Allocate()
{
    // 优先从空闲链表中分配
    void* ret = PopFreeList();
    if (ret)
        return ret;
    // 检查是否需要分配新的内存块
    if (m_firstBlock == nullptr || m_curSlot >= m_lastSlot) {
        if (!AllocateNewBlock()) {
            return nullptr;
        }
    }
    // 从当前内存块中分配
    ret = static_cast<void*>(m_curSlot);

    m_curSlot = reinterpret_cast<Slot*>(reinterpret_cast<size_t>(m_curSlot) + m_slotSize);

    return ret;
}

void MemoryPool::Deallocate(void* ptr)
{
    PushFreeList(ptr);
}

void* MemoryPool::PopFreeList()
{
    Slot* ret = nullptr;
    if (m_freeList) {
        ret = m_freeList;
        m_freeList = m_freeList->next;
    }
    return static_cast<void*>(ret);
}

void MemoryPool::PushFreeList(void* ptr)
{
    Slot* newSlot = static_cast<Slot*>(ptr);
    newSlot->next = m_freeList;
    m_freeList = newSlot;
}

bool MemoryPool::AllocateNewBlock()
{
    int tryTimes = 10;
    Slot* newBlock = nullptr;
    while (tryTimes > 0) {
        newBlock = reinterpret_cast<Slot*>(aligned_alloc(m_blockSize, m_blockSize));
        if (newBlock == nullptr) {
            tryTimes--;
            continue;
        }
        break;
    }
    if (newBlock == nullptr) {
        return false;
    }
    newBlock->next = m_firstBlock;
    m_firstBlock = newBlock;
    Slot* body = m_firstBlock + 1;
    size_t padding = Padding(body, m_slotSize);
    m_curSlot = reinterpret_cast<Slot*>(reinterpret_cast<char*>(body) + padding);
    m_lastSlot = reinterpret_cast<Slot*>(reinterpret_cast<size_t>(m_firstBlock) + m_blockSize - m_slotSize + 1);
    return true;
}

MemoryPool& HashBucket::GetMemoryPool(int index)
{
    static std::vector<MemoryPool> memoryPools(MEMORY_POOL_NUM, MemoryPool(4096));
    return memoryPools[index];
}

void HashBucket::InitMemoryPool()
{
    for (int i = 0; i < MEMORY_POOL_NUM; ++i) {
        MemoryPool& pool = GetMemoryPool(i);
        pool.Init((i + 1) * SLOT_BASE_SIZE);
    }
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


} // namespace SP_MemoryPool