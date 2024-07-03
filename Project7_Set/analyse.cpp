#include <bits/stdc++.h>

// #define Treap

#ifdef RBTree
#include "src.hpp"
#endif

#ifdef fhq
#include "src_fhq.hpp"
#endif

#ifdef Treap
#include "src_treap.hpp"
#endif

class Timer
{
private:
    double dfnStart, dfnEnd;

public:
    void init()
    {
        dfnEnd = dfnStart = clock();
    }
    void stop()
    {
        dfnEnd = clock();
    }
    double getTime()
    {
        return 1.0 * (dfnEnd - dfnStart) / CLOCKS_PER_SEC;
    }
} timer;

const int maxn = 105;
const int maxm = 1e5;

// ESet<int> s[maxn];
std::set<int> s[maxn];

std::vector<int> v, opreat;

void init()
{

    for (int i = 1; i <= maxm; i++)
    {
        v.push_back(i);
        opreat.push_back(i);
    }
    std::shuffle(v.begin(), v.end(), std::default_random_engine(19970104));
    std::shuffle(opreat.begin(), opreat.end(), std::default_random_engine(20010101));
}

void emplace_test()
{
    std::cout << "test 1: emplace test    " << std::endl;
    timer.init();
    for (int i = 0; i < maxm; i++)
        s[0].emplace(v[i]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void find_test()
{
    std::cout << "test 2: find test    " << std::endl;
    timer.init();
    for (int i = 0; i < maxm; i++)
        auto it = s[0].find(opreat[i]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void copy_construct_test()
{
    std::cout << "test 3: copy_construct test    " << std::endl;
    timer.init();
    for (int i = 1; i < maxn; i++)
        s[i] = s[i - 1];
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void clear_test()
{
    std::cout << "test 4: clear test    " << std::endl;
    timer.init();
    for (int i = 1; i < maxn; i++)
        s[i].clear();
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void move_construct_test()
{
    std::cout << "test 5: move_construct test    " << std::endl;
    timer.init();
    s[1] = s[0];
    for (int i = 2; i < maxn; i++)
        s[i] = std::move(s[i - 1]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void range_test()
{
    std::cout << "test 6: range test    " << std::endl;
    timer.init();
    // for (int i = 1; i < maxm; i += 2)
    //     auto num = s[0].range(opreat[i - 1], opreat[i]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void bound_find_test()
{
    std::cout << "test 7: bound_find test    " << std::endl;
    timer.init();
    for (int i = 0; i < maxm; i++)
        auto it = s[0].lower_bound(opreat[i]);
    for (int i = maxm - 1; i >= 0; i--)
        auto it = s[0].lower_bound(opreat[i]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void iterator_test()
{
    std::cout << "test 8: iterator test    " << std::endl;
    // std::cout << "debug" << std::endl;
    timer.init();
    auto it = s[0].begin();
    int pos = 0, num = 0;
    for (int i = 100; i <= 200; i++)
    {
        while (pos > opreat[i])
        {
            it--;
            pos--;
            num++;
            if (num % 100 == 0)
            {
                s[0].erase(opreat[i]);
                s[0].emplace(opreat[i]);
            }
        }
        while (pos < opreat[i])
        {
            it++;
            pos++;
            num++;
            if (num % 100 == 0)
            {
                s[0].erase(opreat[i]);
                s[0].emplace(opreat[i]);
            }
        }
    }
    // std::cout << "debug" << std::endl;
    std::cout << "total move: " << num << std::endl;
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

void erase_test()
{
    std::cout << "test 9: erase test    " << std::endl;
    timer.init();
    for (int i = 0; i < maxm; i++)
        s[0].erase(opreat[i]);
    timer.stop();
    std::cout << "time : " << timer.getTime() << std::endl;
}

int main()
{
    freopen("set_result.txt", "w", stdout);
    init();
    emplace_test();
    find_test();
    copy_construct_test();
    move_construct_test();
    range_test();
    bound_find_test();
    iterator_test();
    erase_test();
    return 0;
}