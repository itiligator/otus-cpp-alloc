//
// Created by andrei on 28.03.23.
//

#ifndef ALLOC_ALLOCATOR_H
#define ALLOC_ALLOCATOR_H

#include <memory>
#include <type_traits>

/**
 *
 * Simple pool allocator, that actually doesn't allocate anything.
 * It manages pool for storing elements. Size of this pool is
 * predefined by poolSize template parameter.
 *
 * allocate() returns pointer to the free place in the pool.
 * If n>1 or there is no free space left in pool allocate fallbacks to Allocator.allocate()
 * deallocate() marks pool element as free place or deallocates object that
 * previously was allocated by fallback allocator.
 *
 * @tparam T type of the stored elements
 * @tparam poolSize how many elements can be stored in the pool
 */

template<typename T, size_t poolSize, class Allocator = std::allocator<T>>
class StackBasedPoolAllocator {
public:

    typedef T value_type;
    static_assert(std::is_same<typename std::allocator_traits<Allocator>::value_type, value_type>::value);
    typedef T *pointer;
    static_assert(std::is_same<typename std::allocator_traits<Allocator>::pointer, pointer>::value);
    typedef const T *const_pointer;
    static_assert(std::is_same<typename std::allocator_traits<Allocator>::const_pointer, const_pointer>::value);
    typedef Allocator fallback_allocator_type;

private:

    template<class StoredObjectType, bool Padded = false>
    struct StoragePlaceholder {
        /**
         * When a placeholder is free, the `next` contains the
         * address of the next free placeholder in a list. Otherwise
         * memory for placeholder is (probably fully) used for storing one object
         */
        StoragePlaceholder *next{nullptr};
    };

    template<class StoredObjectType>
    struct StoragePlaceholder<StoredObjectType, true> {
        StoragePlaceholder *next{nullptr};
        // additional padding in case of sizeof(Placeholder*) < sizeof(StoredObjectType)
        char _[(sizeof(StoredObjectType) - sizeof(void *)) / sizeof(char)];
    };


    using Placeholder = StoragePlaceholder<value_type, (sizeof(value_type) > sizeof(void*))>;

    static constexpr size_t storage_alignment = std::max(alignof(Placeholder), alignof(value_type));
    alignas(storage_alignment) Placeholder storage[poolSize];
    Placeholder *current_free_placeholder = storage;
    fallback_allocator_type fallback_allocator{};


    bool pointer_to_internal_buffer(pointer p) const {
        auto casted_p = reinterpret_cast<Placeholder * >(p);
        return (casted_p >= storage) && (casted_p < storage + poolSize);
    }

public:

    [[nodiscard]] pointer allocate(std::size_t n) {
        if (n == 1 && current_free_placeholder != nullptr) { // then "allocate" from internal buffer
            auto pointer_to_return = reinterpret_cast<pointer>(current_free_placeholder);
            current_free_placeholder = current_free_placeholder->next;
            return pointer_to_return;
        } else { // allocate using fallback allocator
            return fallback_allocator.allocate(n);
        }
    }

    void deallocate(value_type *deallocated_obj, size_t size) {
        if (pointer_to_internal_buffer(deallocated_obj)) { // "deallocating" from internal buffer
            if (size == 1) { // only allowed deallocation of one obj at time
                reinterpret_cast<Placeholder *>(deallocated_obj)->next = current_free_placeholder;
                current_free_placeholder = reinterpret_cast<Placeholder * >(deallocated_obj);
            } else {
                throw std::bad_alloc();
            }
        } else { // deallocate using fallback allocator
            fallback_allocator.deallocate(deallocated_obj, size);
        }

    }


    StackBasedPoolAllocator() noexcept {
        // Chain all placeholders in storage
        // the last one points to nullptr by default
        for (size_t i = 0; i < poolSize - 1; ++i) {
            storage[i].next = &storage[i + 1];
        }
    };

    ~StackBasedPoolAllocator() noexcept = default;

    StackBasedPoolAllocator(const StackBasedPoolAllocator &) noexcept {
    }

    StackBasedPoolAllocator(const StackBasedPoolAllocator &&other) noexcept:
            current_free_placeholder(other.current_free_placeholder),
            storage(other.storage),
            fallback_allocator(other.fallback_allocator) {
        other.current_free_placeholder = nullptr;
        other.storage = nullptr;
        other.fallback_allocator = std::allocator<value_type>();
    }

    StackBasedPoolAllocator &operator=(const StackBasedPoolAllocator &) noexcept {
        return *this;
    }

    StackBasedPoolAllocator &operator=(StackBasedPoolAllocator &&other) noexcept {
        current_free_placeholder = other.current_free_placeholder;
        other.current_free_placeholder = nullptr;
        storage = other.storage;
        other.storage = nullptr;
        fallback_allocator = other.fallback_allocator;
        other.fallback_allocator = std::allocator<value_type>();
        return *this;
    }


    template<class Type>
    struct rebind {
        typedef StackBasedPoolAllocator<Type, poolSize, typename std::allocator_traits<Allocator>::template rebind_alloc<Type>> other;
    };

    static constexpr size_t max_size() noexcept {
        return std::max(poolSize, std::allocator_traits<Allocator>::max_size());
    }

    // TODO: разобраться
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
};

template<typename T, typename U, int N, int K>
inline bool operator==(const StackBasedPoolAllocator<T, N> &, const StackBasedPoolAllocator<U, K> &) {
    return sizeof(StoragePlaceholder<T>) == sizeof(StoragePlaceholder<U>) && N == K;
}

template<typename T, typename U, int N, int K>
inline bool operator!=(const StackBasedPoolAllocator<T, N> &lhs, const StackBasedPoolAllocator<U, K> &rhs) {
    return !(rhs == lhs);
}


#endif //ALLOC_ALLOCATOR_H
