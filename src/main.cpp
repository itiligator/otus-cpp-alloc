#include <map>
#include "PoolAllocator.h"
#include <iostream>

constexpr int factorial(int n){
    return (n < 2) ? 1 : (n * factorial(n-1));
}

int main(int, char **) {
    // map with default allocator
    std::map<int, int> map_std_alloc{};
    for (int i=1; i < 11; i++){
        map_std_alloc[i] = factorial(i);
    }

    // map with custom allocator
    std::map<int, int, std::greater<>, LocalPoolAllocator<std::pair<const int, int>, 20>> map_custom_alloc{};
    for (int i=1; i < 11; i++){
        map_custom_alloc[i] = factorial(i);
    }

    auto map_custom_alloc_copy = std::move(map_custom_alloc);

//    // print key-value pairs stored in the map with custom allocator
//    for (int i=1; i < 11; i++){
//        std::cout << i << " " << map_custom_alloc[i] << std::endl;
//    }

    // print key-value pairs stored in the map with custom allocator
    for (int i=1; i < 11; i++){
        std::cout << i << " " << map_custom_alloc_copy[i] << std::endl;
    }

//

	return 0;
}
