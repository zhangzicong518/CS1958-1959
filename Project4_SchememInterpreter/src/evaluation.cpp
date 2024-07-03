#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Value Let::eval(Assoc &env) // let expression
{
    Assoc new_env = empty();
    for (auto i = env; i.get() != nullptr; i = i->next)
        new_env = extend(i->x, i->v, new_env);
    for (auto pii : bind)
    {
        auto value = find(pii.first, new_env);
        if (value.get() != nullptr)
            modify(pii.first, pii.second->eval(env), new_env);
        else
            new_env = extend(pii.first, pii.second->eval(env), new_env);
    }
    auto result = body->eval(new_env);
    if (dynamic_cast<Symbol *>(result.get()))
    {
        std::string name = (*dynamic_cast<Symbol *>(result.get())).s;
        auto tar = find(name, new_env);
        if (tar.get() != nullptr)
        {
            if (dynamic_cast<Closure *>(tar.get()))
                env = new_env;
            else
                return tar;
        }
    }
    return result;
}

Value Lambda::eval(Assoc &env) // lambda expression
{
    return ClosureV(x, e, env);
}

Value Apply::eval(Assoc &e) // for function calling
{
    std::string name = (*dynamic_cast<Symbol *>(rator->eval(e).get())).s;
    auto tar = find(name, e);
    if (tar.get() == nullptr || !dynamic_cast<Closure *>(tar.get()))
    {
        throw(RuntimeError("RE"));
    }
    std::vector<std::string> para = (*dynamic_cast<Closure *>(tar.get())).parameters;
    if (para.size() != rand.size())
    {
        if (rand.size() == 0)
        {
            auto tmp = dynamic_cast<Closure *>(tar.get());
            auto result = tmp->e->eval(e);
            return result;
        }
        else
            throw(RuntimeError("RE"));
    }

    // debug
    // std::cout << "func name " << name << std::endl;

    int len = (int)para.size();
    Assoc new_env = empty();
    for (auto i = e; i.get() != nullptr; i = i->next)
        new_env = extend(i->x, i->v, new_env);
    for (int i = 0; i < len; i++)
    {
        // debug
        // auto value = rand[i]->eval(e);
        // std::cout << para[i] << std::endl;
        // value->show(std::cout);
        // std::cout << std::endl;

        new_env = extend(para[i], rand[i]->eval(e), new_env);
    }
    auto result = dynamic_cast<Closure *>(tar.get())->e->eval(new_env);
    return result;
}
// parse all the expr tp value
// link value to parameters
// eval in new env

Value Letrec::eval(Assoc &env) // letrec expression
{
    // init env1 insert all var
    Assoc env1 = empty();
    for (auto i = env; i.get() != nullptr; i = i->next)
        env1 = extend(i->x, i->v, env1);
    for (auto pii : bind)
    {
        auto value = find(pii.first, env1);
        if (value.get() != nullptr)
            modify(pii.first, Value(nullptr), env1);
        else
            env1 = extend(pii.first, Value(nullptr), env1);
        // env1 = extend(pii.first, Value(nullptr), env1);
    }

    // init env2 eval all var
    Assoc env2 = empty();
    for (auto i = env1; i.get() != nullptr; i = i->next)
        env2 = extend(i->x, i->v, env2);
    for (auto pii : bind)
        modify(pii.first, pii.second->eval(env1), env2);
    // update env of closure
    for (auto pii : bind)
    {
        if (dynamic_cast<Closure *>(pii.second.get()))
            modify(pii.first, pii.second->eval(env2), env2);
    }

    auto result = body->eval(env2);
    if (dynamic_cast<Symbol *>(result.get()))
    {
        std::string name = (*dynamic_cast<Symbol *>(result.get())).s;
        auto tar = find(name, env2);
        if (tar.get() != nullptr)
        {
            if (dynamic_cast<Closure *>(tar.get()))
                env = env2;
            else
                return tar;
        }
    }
    return result;
}

// evaluation of variable
Value Var::eval(Assoc &e)
{
    int len = x.length();
    if (len == 0)
        return NullV();
    auto value = find(x, e);
    if (value.get() != nullptr && !dynamic_cast<Closure *>(value.get()))
        return value;
    if (x == "#f")
        return BooleanV(false);
    else if (x == "#t")
        return BooleanV(true);
    else
        return SymbolV(x);
}

Value Fixnum::eval(Assoc &e) // evaluation of a fixnum
{
    return IntegerV(n);
}

Value If::eval(Assoc &e) // if expression
{
    auto judge = cond->eval(e);
    if (dynamic_cast<Boolean *>(judge.get()) && (*dynamic_cast<Boolean *>(judge.get())).b == false)
    {
        auto result = alter->eval(e);
        return result;
    }
    else
    {
        auto result = conseq->eval(e);
        return result;
    }
}

Value True::eval(Assoc &e)
{
} // evaluation of #t

Value False::eval(Assoc &e) {} // evaluation of #f

Value Begin::eval(Assoc &e) {} // begin expression

Value Quote::eval(Assoc &e) // quote expression
{
    if (dynamic_cast<List *>(s.get()))
    {
        List *ptr = dynamic_cast<List *>(s.get());
        size_t num = (*ptr).stxs.size();
        if (num == 0)
            return NullV();
        else if (num == 1)
        {
            // RE
        }
        Value pair = PairV((new Quote((*ptr).stxs[num - 1]))->eval(e), NullV());
        for (int i = num - 2; i >= 0; i--)
        {
            auto expr_tmp = new Quote((*ptr).stxs[i]);
            pair = PairV(expr_tmp->eval(e), pair);
        }
        return pair;
    }
    else
    {
        auto value1 = s.parse(e)->eval(e);
        return value1;
    }
}

Value MakeVoid::eval(Assoc &e) // (void)
{
    return VoidV();
}

Value Exit::eval(Assoc &e) // (exit)
{
    return TerminateV();
}

Value Binary::eval(Assoc &e) // evaluation of two-operators primitive
{
    auto para1 = rand1->eval(e);
    auto para2 = rand2->eval(e);
    if (e_type == E_PLUS)
    {
        auto value = (new Plus(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_MINUS)
    {
        auto value = (new Minus(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_MUL)
    {
        auto value = (new Mult(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_CONS)
    {
        auto value = (new Cons(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_LE)
    {
        auto value = (new LessEq(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_LT)
    {
        auto value = (new Less(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_GE)
    {
        auto value = (new GreaterEq(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_GT)
    {
        auto value = (new Greater(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_EQ) //=
    {
        auto value = (new Equal(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
    else if (e_type == E_EQQ) // eq?
    {
        auto value = (new IsEq(rand1, rand2))->evalRator(para1, para2);
        return value;
    }
}

Value Unary::eval(Assoc &e) // evaluation of single-operator primitive
{
    auto para = rand->eval(e);
    if (e_type == E_CAR)
    {
        auto value = (new Car(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_CDR)
    {
        auto value = (new Cdr(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_PAIRQ)
    {
        auto value = (new IsPair(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_BOOLQ)
    {
        auto value = (new IsBoolean(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_NULLQ)
    {
        auto value = (new IsNull(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_INTQ)
    {
        auto value = (new IsFixnum(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_PROCQ)
    {
        auto value = (new IsProcedure(rand))->evalRator(para);
        return value;
    }
    else if (e_type == E_NOT)
    {
        auto value = (new Not(rand))->evalRator(para);
        return value;
    }
}

Value Mult::evalRator(const Value &rand1, const Value &rand2) // *
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    return IntegerV(para1 * para2);
}

Value Plus::evalRator(const Value &rand1, const Value &rand2) // +
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    return IntegerV(para1 + para2);
}

Value Minus::evalRator(const Value &rand1, const Value &rand2) // -
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    return IntegerV(para1 - para2);
}

Value Less::evalRator(const Value &rand1, const Value &rand2) // <
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    if (para1 < para2)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value LessEq::evalRator(const Value &rand1, const Value &rand2) // <=
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    if (para1 <= para2)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value Equal::evalRator(const Value &rand1, const Value &rand2) // =
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    if (para1 == para2)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) // >=
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    if (para1 >= para2)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value Greater::evalRator(const Value &rand1, const Value &rand2) // >
{
    int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
    int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
    if (para1 > para2)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value IsEq::evalRator(const Value &rand1, const Value &rand2) // eq?
{
    if (dynamic_cast<Integer *>(rand1.get()) && dynamic_cast<Integer *>(rand1.get()))
    {
        int para1 = (*dynamic_cast<Integer *>(rand1.get())).n;
        int para2 = (*dynamic_cast<Integer *>(rand2.get())).n;
        if (para1 == para2)
            return BooleanV(true);
        else
            return BooleanV(false);
    }
    else if (dynamic_cast<Boolean *>(rand1.get()) && dynamic_cast<Boolean *>(rand1.get()))
    {
        bool para1 = (*dynamic_cast<Boolean *>(rand1.get())).b;
        bool para2 = (*dynamic_cast<Boolean *>(rand2.get())).b;
        if (para1 == para2)
            return BooleanV(true);
        else
            return BooleanV(false);
    }
    else if (dynamic_cast<Symbol *>(rand1.get()) && dynamic_cast<Symbol *>(rand1.get()))
    {
        std::string para1 = (*dynamic_cast<Symbol *>(rand1.get())).s;
        std::string para2 = (*dynamic_cast<Symbol *>(rand2.get())).s;
        if (para1 == para2)
            return BooleanV(true);
        else
            return BooleanV(false);
    }
    else
    {
        if (rand1.get() == rand2.get())
            return BooleanV(true);
        else
            return BooleanV(false);
    }
}

Value Cons::evalRator(const Value &rand1, const Value &rand2) // cons
{
    return PairV(rand1, rand2);
}

Value IsBoolean::evalRator(const Value &rand) // boolean?
{
    if (dynamic_cast<Boolean *>(rand.get()))
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value IsFixnum::evalRator(const Value &rand) // fixnum?
{
    if (dynamic_cast<Integer *>(rand.get()))
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value IsSymbol::evalRator(const Value &rand) {} // symbol?

Value IsNull::evalRator(const Value &rand) // null?
{
    if (dynamic_cast<Null *>(rand.get()))
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value IsPair::evalRator(const Value &rand) // pair?
{
    if (dynamic_cast<Pair *>(rand.get()))
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value IsProcedure::evalRator(const Value &rand) // procedure?
{
    if (dynamic_cast<Closure *>(rand.get()))
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value Not::evalRator(const Value &rand) // not
{
    if (dynamic_cast<Boolean *>(rand.get()) && (*dynamic_cast<Boolean *>(rand.get())).b == false)
        return BooleanV(true);
    else
        return BooleanV(false);
}

Value Car::evalRator(const Value &rand) // car
{
    auto value = (*dynamic_cast<Pair *>(rand.get())).car;
    return value;
}

Value Cdr::evalRator(const Value &rand) // cdr
{
    auto value = (*dynamic_cast<Pair *>(rand.get())).cdr;
    return value;
}
