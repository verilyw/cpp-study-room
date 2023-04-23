#include <iostream>
#include "math.h"


int main() {
    int res;

    res = math::add(1, 2);
    std::cout << "math::add(1, 2) = " << res << '\n';

    res = math::sub(4, 2);
    std::cout << "math::sub(4, 2) = " << res << '\n';

    res = math::mul(3, 2);
    std::cout << "math::mul(3, 2) = " << res << '\n';

    return 0;
}