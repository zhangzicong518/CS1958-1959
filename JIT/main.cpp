#include <iostream>
#include "asmjit/asmjit.h"
#include <asmjit/x86.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <random>

using namespace asmjit;

#define N 50000000

typedef bool (*Func)(const char *str);

void generateRandomString(size_t length, char *c, int len, std::mt19937 gen, char *ret)
{
    std::uniform_int_distribution<> dis(0, 127);
    int ret_len = length + len;
    if (dis(gen) < 80)
        for (int i = 0; i < len; ++i)
        {
            ret[i] = c[i];
        }
    else
        for (int i = 0; i < len; ++i)
        {
            ret[i] = (char)dis(gen);
        }

    for (int i = len; i < ret_len; ++i)
    {
        ret[i] += (char)dis(gen);
    }
}

char *c[N];
char mode[100];
int mode_len;

int baseline()
{
    int ans = 0;
    for (int j = 0; j < N; j++)
    {
        if (mode_len > 20)
            continue;
        bool flg = true;
        for (int i = 0; i < mode_len; i++)
        {
            if (c[j][i] != mode[i])
            {
                flg = false;
                break;
            }
        }
        if (flg)
            ans++;
    }
    return ans;
}

void generate()
{
    std::cin >> mode;
    mode_len = strlen(mode);

    const int len = 10;
    const int nthread = 9;
    std::thread t[nthread];
    std::random_device rd;
    for (int i = 0; i < nthread; i++)
    {
        std::mt19937 gen(rd() + i);
        t[i] = std::thread([i, gen] { // lanbda表达式 表示函数，也可单独作为一个函数写出来（类似于匿名函数）
            for (int j = i; j < N; j += nthread)
            {
                c[j] = new char[len + mode_len + 4];
                generateRandomString(len, mode, mode_len, gen, c[j]);
            }
        });
    }
    for (auto &tt : t)
    {
        tt.join(); // 等待所有线程执行完毕
    }
    std::cout << "finish genmerate" << std::endl;
}

int main(int argc, char *argv[])
{
    generate();

    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment(),
              rt.cpuFeatures());
    x86::Compiler cc(&code);

    FuncNode *funcNode = cc.addFunc(FuncSignature::build<bool, const char *>());
    x86::Gp str_ptr = cc.newGpq("str_ptr");
    x86::Gp mode_ptr = cc.newGpq("mode_ptr");
    x86::Gp vRet_t = cc.newGpd();
    x86::Gp vRet_f = cc.newGpd();
    x86::Gp index = cc.newGpd("index");
    x86::Gp pattern_len = cc.newGpd("pattern_len");

    funcNode->setArg(0, str_ptr);
    cc.mov(mode_ptr, mode);
    cc.mov(vRet_t, (bool)(true));
    cc.mov(vRet_f, (bool)(false));
    cc.mov(pattern_len, mode_len);
    cc.mov(index, 0);

    Label matched_label = cc.newLabel();
    Label end_label = cc.newLabel();

    cc.test(pattern_len, pattern_len);
    cc.jz(end_label);

    cc.bind(matched_label);

    x86::Gp value_model = cc.newGpb("value_model");
    cc.mov(value_model, x86::byte_ptr(mode_ptr));
    x86::Gp value_str = cc.newGpb("value_str");
    cc.mov(value_str, x86::byte_ptr(str_ptr));
    cc.cmp(value_model, value_str);
    cc.jne(end_label);

    cc.add(str_ptr, 1);
    cc.add(mode_ptr, 1);
    cc.inc(index);
    cc.cmp(index, pattern_len);
    cc.jl(matched_label);

    cc.ret(vRet_t);
    cc.bind(end_label);
    cc.ret(vRet_f);

    cc.endFunc();
    cc.finalize();

    Func fn;
    Error err = rt.add(&fn, &code);
    if (err)
        return 1;

    std::cout << "start matching" << std::endl;

    auto start1 = std::chrono::high_resolution_clock::now();

    int num = 0;
    for (int i = 0; i < N; ++i)
    {
        // std::cout << c[i] << std::endl;
        if (fn(c[i]))
        {
            // std::cout << mode << std::endl;
            num++;
        }
    }

    auto stop1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1).count();

    std::cout << " JIT : Elapsed time: " << duration1 << " ms\n";

    auto start2 = std::chrono::high_resolution_clock::now();

    int ans = baseline();

    auto stop2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2).count();

    std::cout << " baseline : Elapsed time: " << duration2 << " ms\n";

    std::cout << ans << " " << num << std::endl;

    rt.release(fn);

    for (int i = 0; i < N; ++i)
        delete c[i];

    return 0;
}
