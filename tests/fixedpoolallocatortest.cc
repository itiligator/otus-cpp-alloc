//
// Created by andrei on 30.03.23.
//

#include <gtest/gtest.h>
#include "allocator.h"




TEST(StackBasedPoolAllocator, NumberOfAllocatedElements) {
    struct DDD{
        char s[9];
    };
    std::cout << alignof(DDD) << std::endl;
    constexpr size_t poolSize{10};
    auto alloc = StackBasedPoolAllocator<DDD, 11>();
    std::set<char*> fetched_pointers{};

}

