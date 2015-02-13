#ifndef CMAP_H
#define CMAP_H

#include <map>
#include "../Hacks/Types.h"

template<typename A, typename B, typename C,typename D>
class CMap
{
    std::map<A, C> _map;

    public:
        CMap()
        {
        }

        ULONG GetSize() const
        {
            return _map.size();
        }

        void RemoveAll()
        {
            _map.clear();
        }

        void SetAt(ULONG offset, C value)
        {
            if (_map.find(offset) != _map.end())
            {
                _map.at(offset) = value;
            }

            _map.insert(std::pair<A, C>(offset, value));
        }

        bool Lookup(ULONG offset, C& value) const
        {
            auto it = _map.find(offset);
            if (it != _map.end())
            {
                value = it->second;
                return true;
            }
            return false;
        }

        void InitHashTable(ULONG n)
        {

        }
};

#endif // CMAP_H
