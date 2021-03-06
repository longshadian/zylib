#pragma once

#include <vector>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
//组合算法,计算
// char a[MAX_LENGTH]; //存储初始字符串
// char r[MAX_LENGTH]; //存储组合结果
// src-被组合集合 
// srclen-被组合集合的长度
// need-需要组合的长度
// buffer-组合过程所需缓存,长度至少为need
// out-最终结果
//----------------------------------
// 集合{ 0, 1, 2, 3, 4 } 取3个元素组合
// 0 1 2
// 0 1 3
// 0 1 4
// 0 2 3
// 0 2 4
// 0 3 4
// 1 2 3
// 1 2 4
// 1 3 4
// 2 3 4

template <typename T>
void combinational_detail(const T* src,
    size_t srclen,
    size_t need,
    T* buffer,
    std::vector<std::vector<T> >* out,
    size_t lenpos = 0,
    size_t needpos = 0)
{
    if (needpos == need) {
        out->push_back(std::vector<T>(buffer, buffer + need));
        return;
    }

    for (size_t i = lenpos; i < srclen; ++i) {
        if (srclen - i < need - needpos)
            return;
        buffer[needpos] = src[i];
        combinational_detail(src, srclen, need, buffer, out, i + 1, needpos + 1);
    }
}

template <typename T>
void combinational(const T* src,
    size_t srclen,
    size_t need,
    T* buffer,
    std::vector<std::vector<T> >* out)
{
    assert(need <= srclen);
    if (need == 0)
        return;
    combinational_detail(src, srclen, need, buffer, out, 0, 0);
}

//排列组合，对集合{1,2,3,4},输出
// 1
// 2
// 3
// 4
// 1 2
// 1 3
// 1 4
// 2 3
// 2 4
// 3 4
// 1 2 3
// 1 2 4
// 1 3 4
// 2 3 4
// 1 2 3 4

template <typename T>
void permutation(const T* src, size_t src_len, std::vector<std::vector<T>>* out)
{
    if (!src || src_len == 0)
        return;

    for (size_t i = 1; i <= src_len; ++i) {
        std::vector<T> buffer;
        buffer.resize(i);
        std::vector<std::vector<T>> ret;
        combinational(src, src_len, i, buffer.data(), &ret);
        for (size_t j = 0; j != ret.size(); ++j)
            out->push_back(ret[j]);
    }
}

//洗牌算法
/*
template <typename RAIterator>
void randomShuffle(RAIterator first, RAIterator last)
{
    typename std::iterator_traits<RAIterator>::difference_type n = (last - first);
    if (n <= 0)
        return;
    while (--n) {
        std::swap(first[n], first[rand() % (n + 1)]);
    }
}
*/
