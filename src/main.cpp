#include <map>
#include "allocator.h"
#include <iostream>

constexpr unsigned factorial(unsigned n){
    return (n < 2) ? 1 : (n * factorial(n-1));
}

int main(int, char **) {
    // map with default allocator
    std::map<int, int> map_std_alloc{};
    for (int i=1; i < 11; i++){
        map_std_alloc[i] = factorial(i);
    }

    // map with custom allocator
    std::map<int, int, std::greater<>, StackBasedPoolAllocator<std::pair<int, int>, 10>> map_custom_alloc{};
    for (int i=1; i < 11; i++){
        map_custom_alloc[i] = factorial(i);
    }

    // print key-value pairs stored in the map with custom allocator
    for (int i=1; i < 11; i++){
        std::cout << i << " " << map_custom_alloc[i] << std::endl;
    }

	return 0;
}
