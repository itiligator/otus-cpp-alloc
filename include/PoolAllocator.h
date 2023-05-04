//
// Created by andrei on 01.05.23.
//

#ifndef ALLOC_POOLALLOCATOR_H
#define ALLOC_POOLALLOCATOR_H

#include <type_traits>
#include <memory>
#include "IPool.h"
#include "LocalPool.h"

template<
        typename T,
        std::size_t poolSize,
        template<typename> typename FallbackAllocator = std::allocator,
        template<typename, size_t> typename Pool = LocalPool
> requires(
        std::is_base_of_v<IPool<T, poolSize>, Pool<T, poolSize>>
)
class PoolAllocator {
private:
    Pool<T, poolSize> pool;
    FallbackAllocator<T> fallbackAllocator;

public:

    typedef T value_type;
    typedef T *pointer;

    [[nodiscard]] pointer allocate(size_t n) {
        try {
            return pool.allocate_from_pool(n);
        } catch (std::bad_alloc &) {
            return fallbackAllocator.allocate(n);
        }
    }

    void deallocate(pointer p, size_t n) {
        try {
            return pool.deallocate_to_pool(p, n);
        } catch (std::bad_alloc &) {
            fallbackAllocator.deallocate(p, n);
        }
    }

    ~PoolAllocator() noexcept = default;

    PoolAllocator() noexcept = default;

    PoolAllocator(const PoolAllocator &) noexcept {
    }

    PoolAllocator(const PoolAllocator &&other) noexcept:
            pool(std::move(other.pool)),
            fallbackAllocator(std::move(other.fallbackAllocator)) {
    }

    PoolAllocator &operator=(const PoolAllocator &) noexcept {
        return *this;
    }

    PoolAllocator &operator=(PoolAllocator &&other) noexcept {
        pool = std::move(other.pool);
        fallbackAllocator = std::move(other.fallbackAllocator);
        return *this;
    }


    template<class Type>
    struct rebind {
        typedef PoolAllocator<Type, poolSize, FallbackAllocator, Pool> other;
    };


    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;

};


template<typename T>
struct DummyAllocator {

    typedef T value_type;

    T *allocate([[maybe_unused]] size_t n) {
        throw std::bad_alloc();
    }

    void deallocate([[maybe_unused]] T *p, [[maybe_unused]] size_t n) {
        throw std::bad_alloc();
    }

};

template<typename T, size_t poolSize>
using LocalPoolAllocatorExtended = PoolAllocator<T, poolSize>;

template<typename T, size_t poolSize>
using LocalPoolAllocator = PoolAllocator<T, poolSize, DummyAllocator>;

#endif //ALLOC_POOLALLOCATOR_H
