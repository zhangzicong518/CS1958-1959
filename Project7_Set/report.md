# 简介：

本次大作业中，除了任务一要求的使用红黑树为底层实现set以外，我另外选择了fhq和treap两种平衡树为底层实现set的功能。

然后对于基于红黑树，fhq，treap实现的和 std::set 共计四种set的各项功能进行了测试和性能分析

# 测试数据：

~~~cpp
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
~~~
对于 **emplace  find range lower_bound upper_bound erase** 以及 拷贝构造 移动构造 迭代器移动等操作，分别构建了测试点，测试数据均为随机生成，且不同数据结构使用的随机测试数据相同。

对于 拷贝构造 移动构造 进行了 **100** 次操作

对于 迭代器移动 进行了约 **3e6** 次操作

对于 emplace erase 等功能进行了 **1e5** 次操作

# 测试结果：

|       | emplace | find | 拷贝构造 | 移动构造 | range | lower_bound upper_bound | iterator | erase |
| :---  |    :----:   |   :---: |  :----:   |  :----:   |  :----:   |  :----:   |  :----:   |  :----:   |
| 红黑树 |   137    | 9   | 3637| 11838 | 6 | 12 | 81 | 139 |
| fhq |   170    | 39   | 3662 | 10575 | 22 | 69 | 597 | 93 |
| treap |   182    | 20   | 3640 | 10501 | 17 | 43 | 280 | 89 |
| std::set |   138    | 23   | 3595 | 24168 | 无 | 116 | 32 | 310 |

tip：

1. 上述测试结果时间单位为 ms

2. std::set在树形不改变的情况下，对于迭代器的移动有优化。静态树形下3e6级别的移动用时不超过1ms。因此为了避免该情况，在 iterator 测试中添加了每100次移动后会执行插入删除操作来改变树形。

# 性能分析

对于红黑树，fhq，treap实现的set来说，几乎在每一种操作的效率上都符合，红黑树优于treap，treap优于fhq。特别是在迭代器的移动上，红黑树的迭代器移动可以通过回溯父亲的方式解决，一般移动次数不超过logn，而fhq和treap每次都需要分裂整棵树或者从根节点开始遍历几乎每次都需要logn次操作。

另一方面，std::set在大部分操作下表现出的效率和自己实现的红黑树差不多。但是在拷贝构造上，因为stl本身对set的实现比较复杂，比较重视拷贝时的内存安全等原因，std::set的拷贝构造函数显著慢于自己实现的set。在lower_bound, upper_bound, erase操作上也显著慢于自己实现的set。但是在迭代器的移动上却大幅优于自己实现的set。

总的来说红黑树还是最适合实现set的数据结构，在各种操作上都表现的很好，但是码量比较大。fhq实现的set很慢，但实现方式比较简单，码量很小。