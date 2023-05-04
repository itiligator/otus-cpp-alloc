//
// Created by andrei on 01.05.23.
//

#ifndef ALLOC_LOCALPOOL_H
#define ALLOC_LOCALPOOL_H


#include <memory>
#include "IPool.h"

/**
 * Simple local pool.
 * It manages pool for storing poolSize elements.
 *
 * allocate_from_pool() returns pointer to the free place in the pool. Only n==1 is allowed.
 * deallocate_to_pool() marks pool element as free place.
 *
 * @tparam T type of the stored elements
 * @tparam poolSize how many elements can be stored in the pool
 */

template<
        typename T,
        size_t poolSize
>
class LocalPool : IPool<T, poolSize> {

private:

    static constexpr auto storage_alignment = std::max(alignof(void*), alignof(T));

    template<typename StoredType, bool PaddingNeeded>
    struct StoragePlaceholder;

    template<typename StoredType>
    struct alignas(storage_alignment) StoragePlaceholder<StoredType, false> {
        /**
         * When a placeholder is free, the `next` contains the
         * address of the next free placeholder in a list. Otherwise
         * memory for placeholder is (probably fully) used for storing one object
         */
        StoragePlaceholder *next{nullptr};
    };

    template<typename StoredType>
    struct alignas(storage_alignment) StoragePlaceholder<StoredType, true> {
        StoragePlaceholder *next{nullptr};
        // additional padding in case of sizeof(Placeholder*) < sizeof(StoredObjectType)
        char _[(sizeof(StoredType) - sizeof(StoragePlaceholder *)) / sizeof(char)]{};
    };

    using Placeholder = StoragePlaceholder<T, (sizeof(T) > sizeof(void *))>;

    Placeholder storage[poolSize];

    Placeholder *current_free_placeholder = storage;

    bool pointer_to_internal_buffer(void *p) const {
        auto casted_p = reinterpret_cast<Placeholder * >(p);
        return (casted_p >= storage) && (casted_p < storage + poolSize);
    }

public:

    static_assert(poolSize > 0, "cannot create empty pool");

    [[nodiscard]] T *
    allocate_from_pool([[maybe_unused]] std::size_t n) override {
        [[likely]] if (current_free_placeholder != nullptr && n == 1) {
            auto pointer_to_return = reinterpret_cast<T *>(current_free_placeholder);
            current_free_placeholder = current_free_placeholder->next;
            return pointer_to_return;
        } else throw std::bad_alloc();

    }

    void deallocate_to_pool(T *deallocated_obj, std::size_t size) override {
        [[likely]] if (pointer_to_internal_buffer(deallocated_obj) && size == 1) {
            reinterpret_cast<Placeholder *>(deallocated_obj)->next = current_free_placeholder;
            current_free_placeholder = reinterpret_cast<Placeholder * >(deallocated_obj);
        } else throw std::bad_alloc();
    }


    LocalPool() noexcept {
        // Chain all placeholders in storage
        // the last one points to nullptr by default
        for (size_t i = 0; i < poolSize - 1; ++i) {
            storage[i].next = &storage[i + 1];
        }
    };

    ~LocalPool() noexcept override = default;

    LocalPool(const LocalPool &) noexcept {
    }

    LocalPool(LocalPool &&other) noexcept:
            storage(other.storage),
            current_free_placeholder(other.current_free_placeholder) {
        other.current_free_placeholder = nullptr;
        other.storage = nullptr;
    }

    LocalPool &operator=(const LocalPool &) noexcept {
        return *this;
    }

    LocalPool &operator=(LocalPool &&other) noexcept {
        current_free_placeholder = other.current_free_placeholder;
        other.current_free_placeholder = nullptr;
        storage = other.storage;
        other.storage = nullptr;
        return *this;
    }

};

template<typename T, size_t poolSize>
inline bool operator==(const LocalPool<T, poolSize> &,
                       const LocalPool<T, poolSize> &) {
    return true;
}

template<typename T, size_t poolSize_l,
        typename U, size_t poolSize_r>
inline bool operator!=(const LocalPool<T, poolSize_l> & lhs,
                       const LocalPool<U, poolSize_r> & rhs) {
    return !(rhs == lhs);
}


#endif //ALLOC_LOCALPOOL_H
