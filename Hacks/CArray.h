/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 20014-2015 Nirran, phobos2077
 * Copyright (c) 20015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

#ifndef CARRAY_H
#define CARRAY_H

// C++ standard includes
#include <vector>

// int2ssl includes
#include "../Hacks/Types.h"

// Third party includes

template<typename A, typename B>
class CArray
{
    std::vector<A> _vector;
public:
    CArray()
    {
    }

    A& operator[](uint32_t offset)
    {
        return _vector[offset];
    }

    const A& operator[](uint32_t offset) const
    {
        return _vector[offset];
    }

    void RemoveAll()
    {
        while (!_vector.empty()) _vector.pop_back();
    }

    void Add(A value)
    {
        _vector.push_back(value);
    }

    bool IsEmpty()
    {
        return _vector.empty();
    }

    INT_PTR GetUpperBound()
    {
        if (_vector.size() == 0) return 0;
        return _vector.size() - 1;
    }

    void RemoveAt(uint32_t n, uint32_t count = 1)
    {
        _vector.erase(_vector.begin() + n, _vector.begin() + n + count);
    }

    uint32_t GetSize()
    {
        return _vector.size();
    }

    void SetSize(uint32_t n)
    {
        _vector.resize(n);
    }

    void InsertAt(uint32_t position, A value)
    {
        _vector.insert(_vector.begin() + position, value);
    }

    void Copy(CArray source)
    {
        _vector.clear();
        for (auto it = source._vector.begin(); it != source._vector.end(); ++it)
        {
            _vector.push_back(*it);
        }
    }
};

#endif // CARRAY_H
