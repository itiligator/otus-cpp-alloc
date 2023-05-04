//
// Created by andrei on 01.05.23.
//

#ifndef ALLOC_IPOOL_H
#define ALLOC_IPOOL_H

#include <cstddef>

template<
        typename T,
        size_t poolSize
>
requires (
        poolSize > 0
)
class IPool {
public:

    virtual ~IPool() noexcept = default;

    [[nodiscard]] virtual T * allocate_from_pool([[maybe_unused]] std::size_t) = 0;

    virtual void deallocate_to_pool(T *, [[maybe_unused]] std::size_t) = 0;

};

#endif //ALLOC_IPOOL_H
