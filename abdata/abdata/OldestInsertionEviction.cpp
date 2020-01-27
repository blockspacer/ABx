/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "stdafx.h"
#include "EvictionStrategy.h"
#include <multi_index_container.hpp>
#include <multi_index/ordered_index.hpp>

using namespace multi_index;

IO::DataKey OldestInsertionEviction::NextEviction()
{
    DataItemContainer::nth_index<1>::type& rankIndex = dataItems_.get<1>();

    auto first = rankIndex.begin();
    const IO::DataKey& retVal = first->key;
    dataItems_.erase(retVal);
    return retVal;
}

void OldestInsertionEviction::AddKey(const IO::DataKey& key)
{
    dataItems_.insert(DataItem(key, GetNextRank()));
}

void OldestInsertionEviction::RefreshKey(const IO::DataKey& key)
{
    uint64_t next = GetNextRank();
    auto keyItr = dataItems_.find(key);
    dataItems_.modify(keyItr, [next](DataItem& d)
    {
        d.rank = next;
    });
}

void OldestInsertionEviction::DeleteKey(const IO::DataKey& key)
{
    auto keyItr = dataItems_.find(key);
    if (keyItr != dataItems_.end())
    {
        dataItems_.erase(keyItr);
    }
}

void OldestInsertionEviction::Clear()
{
    dataItems_.clear();
    currentRank_ = 0;
}
