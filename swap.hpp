// swap.hpp

#pragma once

namespace sb {

    template <class T>

    inline void swap(T& x, T& y)
    {
        T t;
        t = x;
        x = y;
        y = t;
    }

}
