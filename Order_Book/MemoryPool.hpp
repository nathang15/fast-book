#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <vector>
#include <cstddef>

template<typename T, size_t BlockSize = 4096>
class MemoryPool {
    struct Block {
        T data;
        Block* next;
    };

    union Chunk {
        T data;
        Chunk* next;
    };

    std::vector<Block*> blocks;
    Chunk* freeList;
    size_t itemsPerBlock;

public:
    MemoryPool() : freeList(nullptr) {
        itemsPerBlock = BlockSize / sizeof(Chunk);
        if (itemsPerBlock == 0) itemsPerBlock = 1;
        allocateBlock();
    }

    ~MemoryPool() {
        for (Block* block : blocks) {
            delete[] reinterpret_cast<char*>(block);
        }
    }

    T* allocate() {
        if (freeList == nullptr) {
            allocateBlock();
        }
        Chunk* chunk = freeList;
        freeList = chunk->next;
        return reinterpret_cast<T*>(chunk);
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        Chunk* chunk = reinterpret_cast<Chunk*>(ptr);
        chunk->next = freeList;
        freeList = chunk;
    }

private:
    void allocateBlock() {
        char* blockMem = new char[BlockSize];
        blocks.push_back(reinterpret_cast<Block*>(blockMem));

        Chunk* chunks = reinterpret_cast<Chunk*>(blockMem);
        for (size_t i = 0; i < itemsPerBlock - 1; ++i) {
            chunks[i].next = &chunks[i + 1];
        }
        chunks[itemsPerBlock - 1].next = freeList;
        freeList = chunks;
    }
};

#endif