#include "inc/hashBucket.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using namespace SpMemoryPool;

// 测试用的简单类
class TestClass {
public:
    TestClass()
        : value(0)
    {
    }

    TestClass(int v)
        : value(v)
    {
    }

    ~TestClass() { }

    int getValue() const { return value; }
    void setValue(int v) { value = v; }

private:
    int value;
    char data[120];
};

// 测试用的带资源管理的类
class ResourceClass {
public:
    ResourceClass()
    {
        data = new int[100];
        for (int i = 0; i < 100; ++i) {
            data[i] = 0;
        }
    }

    ~ResourceClass()
    {
        delete[] data;
    }

    void setData(int index, int value)
    {
        if (index >= 0 && index < 100) {
            data[index] = value;
        }
    }

    int getData(int index) const
    {
        if (index >= 0 && index < 100) {
            return data[index];
        }
        return -1;
    }

private:
    int* data;
};

// 测试不同大小的类
class SmallClass {
    char data[16];
};
class MediumClass {
    char data[128];
};
class LargeClass {
    char data[1024];
};

// 测试固定装置（Test Fixture）测试夹具
class MemoryPoolTest : public ::testing::Test {
protected:
    // 每个测试用例开始前执行
    void SetUp() override
    {
        // 初始化内存池
        HashBucket::InitMemoryPool();
    }

    // 每个测试用例结束后执行
    void TearDown() override
    {
        // 这里不需要特别的清理，内存池会在程序结束时自动清理
    }
};

// 测试基本的分配和释放功能
TEST_F(MemoryPoolTest, BasicAllocation)
{
    TestClass* obj = NewElement<TestClass>();
    EXPECT_NE(obj, nullptr);
    obj->setValue(42);
    EXPECT_EQ(obj->getValue(), 42);
    DeleteElement(obj);
}

// 测试带参数构造对象
TEST_F(MemoryPoolTest, ObjectConstructionWithParameters)
{
    TestClass* obj = NewElement<TestClass>(100);
    EXPECT_NE(obj, nullptr);
    EXPECT_EQ(obj->getValue(), 100);
    DeleteElement(obj);
}

// 测试多对象分配和释放
TEST_F(MemoryPoolTest, MultipleObjectAllocation)
{
    const int NUM_OBJECTS = 10000;
    std::vector<TestClass*> objects;

    // 分配对象
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        TestClass* obj = NewElement<TestClass>(i);
        ASSERT_NE(obj, nullptr);
        objects.push_back(obj);
    }

    // 验证对象状态
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        EXPECT_EQ(objects[i]->getValue(), i);
    }

    // 释放对象
    for (TestClass* obj : objects) {
        DeleteElement(obj);
    }
}

// 测试不同大小的对象
TEST_F(MemoryPoolTest, DifferentSizeObjects)
{
    SmallClass* small = NewElement<SmallClass>();
    MediumClass* medium = NewElement<MediumClass>();
    LargeClass* large = NewElement<LargeClass>();

    EXPECT_NE(small, nullptr);
    EXPECT_NE(medium, nullptr);
    EXPECT_NE(large, nullptr);

    DeleteElement(small);
    DeleteElement(medium);
    DeleteElement(large);
}

// 测试资源管理（带析构函数的类）
TEST_F(MemoryPoolTest, ResourceManagement)
{
    ResourceClass* resource = NewElement<ResourceClass>();
    ASSERT_NE(resource, nullptr);

    // 设置和获取数据
    resource->setData(42, 100);
    EXPECT_EQ(resource->getData(42), 100);

    DeleteElement(resource);
}

// 测试空指针处理
TEST_F(MemoryPoolTest, NullPointerHandling)
{
    // DeleteElement应该能安全处理空指针
    EXPECT_NO_THROW(DeleteElement<TestClass>(nullptr));
}

// 测试内存复用
TEST_F(MemoryPoolTest, MemoryReuse)
{
    TestClass* obj1 = NewElement<TestClass>(1);
    void* addr1 = obj1;
    DeleteElement(obj1);

    // 再次分配应该重用之前的内存（这取决于内存池的具体实现）
    TestClass* obj2 = NewElement<TestClass>(2);
    void* addr2 = obj2;

    // 注意：内存复用不是严格保证的
    EXPECT_TRUE(addr1 == addr2);

    DeleteElement(obj2);
}

// 测试边界情况：分配大量小对象
TEST_F(MemoryPoolTest, StressTestSmallObjects)
{
    const int NUM_ITERATIONS = 10;
    const int OBJECTS_PER_ITERATION = 10000;

    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        std::vector<SmallClass*> objects;
        objects.reserve(OBJECTS_PER_ITERATION);

        // 分配对象
        for (int i = 0; i < OBJECTS_PER_ITERATION; ++i) {
            SmallClass* obj = NewElement<SmallClass>();
            ASSERT_NE(obj, nullptr);
            objects.push_back(obj);
        }

        // 释放对象
        for (SmallClass* obj : objects) {
            DeleteElement(obj);
        }
    }
}

TEST(NoFixtureTestGroup, CkeckBlockCnt)
{
    size_t blockSize = 4096;
    size_t slotSize = 128;
    size_t allocCnt = 20000;
    size_t slotCntPerBlock = (blockSize - slotSize) / slotSize;
    MemoryPool pool(blockSize);
    pool.Init(slotSize);
    EXPECT_EQ(pool.GetBlockCnt(), 0);
    EXPECT_EQ(pool.GetFreeSlotCnt(), 0);

    std::vector<void*> allocs(allocCnt);

    for (int i = 0; i < allocCnt; ++i) {
        allocs[i] = pool.Allocate();
        EXPECT_NE(allocs[i], nullptr);
        EXPECT_EQ(pool.GetBlockCnt(), i / slotCntPerBlock + 1);
    }
    for (int i = 0; i < allocCnt; ++i) {
        pool.Deallocate(allocs[i]);
        EXPECT_EQ(pool.GetFreeSlotCnt(), i + 1);
    }
    for (int i = 0; i < allocCnt / 2; ++i) {
        allocs[i] = pool.Allocate();
        EXPECT_NE(allocs[i], nullptr);
        EXPECT_EQ(pool.GetBlockCnt(), (allocCnt - 1) / slotCntPerBlock + 1);
    }

    EXPECT_EQ(pool.GetFreeSlotCnt(), allocCnt / 2);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}