//
// Created by andrei on 30.03.23.
//

#include <gtest/gtest.h>
#include "allocator.h"


TEST(StackBasedPoolAllocator, RebindIntegrationTest) {
    std::map<int, int, std::greater<>, StackBasedPoolAllocator<std::pair<int, int>, 20>> map{};
    map[1] = -1;
    map[2] = -2;
    map[3] = -3;
    map[4] = -4;
    map[5] = -5;
//    auto dd = std::make_pair(1,2);
    EXPECT_EQ(map[1], -1);
    EXPECT_EQ(map[2], -2);
    EXPECT_EQ(map[3], -3);
    EXPECT_EQ(map[4], -4);
    EXPECT_EQ(map[5], -5);
}

TEST(StackBasedPoolAllocator, DDDTest) {
    std::pair<int, int> old_arr[] =
            {
                    std::make_pair(1, 2),
                    std::make_pair(3, 2),
                    std::make_pair(4, 2),
            };

    int n = (sizeof(old_arr) /
             sizeof(old_arr[0]));

    std::map<int, int, std::greater<>, StackBasedPoolAllocator<std::pair<int, int>, 5>> New_Map(old_arr,
                                                                                                 old_arr + n);

}