#ifndef CARRAY_H
#define CARRAY_H

#include "../Hacks/Types.h"
#include <vector>

template<typename A, typename B>
class CArray
{
    std::vector<A> _vector;
public:
    CArray()
    {
    }

    A& operator[](ULONG offset)
    {
        return _vector[offset];
    }

    const A& operator[](ULONG offset) const
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

    void RemoveAt(ULONG n, ULONG count = 1)
    {
        _vector.erase(_vector.begin() + n, _vector.begin() + n + count);
    }

    ULONG GetSize()
    {
        return _vector.size();
    }

    void SetSize(ULONG n)
    {
        _vector.resize(n);
    }

    void InsertAt(ULONG position, A value)
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
