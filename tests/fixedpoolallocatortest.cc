//
// Created by andrei on 30.03.23.
//

#include <gtest/gtest.h>
#include "PoolAllocator.h"




TEST(StackBasedPoolAllocator, NumberOfAllocatedElements) {
    constexpr size_t poolSize{10};
    auto alloc = LocalPoolAllocatorExtended<char, poolSize>();
    std::set<char*> fetched_pointers{};
    for (size_t i = 0; i < poolSize; ++i) {

    }
}

