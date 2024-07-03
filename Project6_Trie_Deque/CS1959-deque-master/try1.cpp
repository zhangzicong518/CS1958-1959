#include <bits/stdc++.h>

#include "deque.hpp"

int main()
{
    sjtu::deque<int> q;
    q.push_front(1);
    const sjtu::deque<int> cq(q);
    sjtu::deque<int>::const_iterator it_q;
    it_q = cq.cbegin();
    try
    {
        const int num = (*it_q);
    }
    catch (...)
    {
        std::cerr << "yes" << '\n';
    }
    return 0;
}