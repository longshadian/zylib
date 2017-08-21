#pragma once

#include <vector>
#include <cassert>

//////////////////////////////////////////////////////////////////////////
//����㷨,����
// char a[MAX_LENGTH]; //�洢��ʼ�ַ���
// char r[MAX_LENGTH]; //�洢��Ͻ��
// src-����ϼ��� 
// srclen-����ϼ��ϵĳ���
// need-��Ҫ��ϵĳ���
// buffer-��Ϲ������軺��,��������Ϊneed
// out-���ս��
//----------------------------------
// ����{ 0, 1, 2, 3, 4 } ȡ3��Ԫ�����
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

//������ϣ��Լ���{1,2,3,4},���
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

//ϴ���㷨
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