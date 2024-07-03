#ifndef PARSER
#define PARSER

// parser of myscheme

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include <map>
#include <cstring>
#include <iostream>
#include <cstring>
#define mp make_pair
using std ::pair;
using std ::string;
using std ::vector;

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Expr Syntax ::parse(Assoc &env)
{
    Expr result = ptr->parse(env);
    return result;
}

Expr Number ::parse(Assoc &env)
{
    Expr result = new Fixnum(n);
    return result;
}

Expr Identifier ::parse(Assoc &env)
{
    Expr result = new Var(s);
    return result;
}

// can't be written as True type because True type does not have a show function
Expr TrueSyntax ::parse(Assoc &env)
{
    Expr result = new Var("#t");
    return result;
}

Expr FalseSyntax ::parse(Assoc &env)
{
    Expr result = new Var("#f");
    return result;
}

// std ::vector<Syntax> stxs;
Expr List ::parse(Assoc &env)
{
    size_t len = stxs.size();
    string s;
    if (len == 0)
    {
        Expr result = new Var("");
        return result;
    }
    for (size_t i = 0; i < len; i++)
    {
        auto stx = stxs[i];
        if (dynamic_cast<List *>(stx.get())) // list
        {
            auto value = stx->parse(env)->eval(env);
            if (dynamic_cast<Symbol *>(value.get()))
            {
                s = (*dynamic_cast<Symbol *>(value.get())).s;
                goto cond;
            }
        }
        else if (dynamic_cast<Identifier *>(stx.get())) // Identifier
        {
            s = (*dynamic_cast<Identifier *>(stx.get())).s;
        cond:
            if (s == "quote")
            {
                auto para1 = stxs[++i];
                Expr result = new Quote(para1);
                return result;
            }
            else if (s == "if")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                auto para3 = stxs[++i].parse(env);
                Expr result = new If(para1, para2, para3);
                return result;
            }
            else if (s == "+")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Plus(para1, para2);
                return result;
            }
            else if (s == "-")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Minus(para1, para2);
                return result;
            }
            else if (s == "*")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Mult(para1, para2);
                return result;
            }
            else if (s == "cons")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Cons(para1, para2);
                return result;
            }
            else if (s == "car")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new Car(para1);
                return result;
            }
            else if (s == "cdr")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new Cdr(para1);
                return result;
            }
            else if (s == "pair?")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new IsPair(para1);
                return result;
            }
            else if (s == "boolean?")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new IsBoolean(para1);
                return result;
            }
            else if (s == "null?")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new IsNull(para1);
                return result;
            }
            else if (s == "fixnum?")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new IsFixnum(para1);
                return result;
            }
            else if (s == "procedure?")
            {
                auto para1 = stxs[++i].parse(env);
                Expr result = new IsProcedure(para1);
                return result;
            }
            else if (s == "lambda")
            {
                auto vec = (*dynamic_cast<List *>(stxs[++i].get())).stxs;
                std::vector<std::string> para1;
                for (auto stx : vec)
                    para1.push_back((*dynamic_cast<Identifier *>(stx.get())).s);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Lambda(para1, para2);
                return result;
            }
            else if (s == "<=")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new LessEq(para1, para2);
                return result;
            }
            else if (s == "<")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Less(para1, para2);
                return result;
            }
            else if (s == ">=")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new GreaterEq(para1, para2);
                return result;
            }
            else if (s == ">")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Greater(para1, para2);
                return result;
            }
            else if (s == "=")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new Equal(para1, para2);
                return result;
            }
            else if (s == "begin")
            {
                if (len == 1)
                    return new Var("");
                for (int j = 1; j + 2 <= len; j++)
                {
                    // auto tmp_expr = vec[i].parse(env);
                    // auto tmp_value = tmp_expr->eval(env);
                    stxs[j].parse(env);
                }
                auto result = stxs[len - 1].parse(env);
                return result;
            }
            else if (s == "let")
            {
                auto vec = (*dynamic_cast<List *>(stxs[++i].get())).stxs;
                auto para2 = stxs[++i].parse(env);
                std::vector<std::pair<std::string, Expr>> para1;
                for (auto stx : vec)
                {
                    Syntax tmp = (*dynamic_cast<List *>(stx.get())).stxs[0];
                    auto fir = (*dynamic_cast<Identifier *>(tmp.get())).s;
                    auto sec = (*dynamic_cast<List *>(stx.get())).stxs[1].parse(env);
                    para1.push_back(std::make_pair(fir, sec));
                }
                Expr result = new Let(para1, para2);
                return result;
            }
            else if (s == "eq?")
            {
                auto para1 = stxs[++i].parse(env);
                auto para2 = stxs[++i].parse(env);
                Expr result = new IsEq(para1, para2);
                return result;
            }
            else if (s == "letrec")
            {
                auto vec = (*dynamic_cast<List *>(stxs[++i].get())).stxs;
                std::vector<std::pair<std::string, Expr>> para1;
                for (auto stx : vec)
                {
                    Syntax tmp = (*dynamic_cast<List *>(stx.get())).stxs[0];
                    auto fir = (*dynamic_cast<Identifier *>(tmp.get())).s;
                    auto sec = (*dynamic_cast<List *>(stx.get())).stxs[1].parse(env);
                    para1.push_back(std::make_pair(fir, sec));
                }
                auto para2 = stxs[++i].parse(env);
                Expr result = new Letrec(para1, para2);
                return result;
            }
            else if (s == "not")
            {
                auto para1 = stxs[++i].parse(env);
                auto result = new Not(para1);
                return result;
            }
            else if (s == "void")
            {
                Expr result = new MakeVoid();
                return result;
            }
            else if (s == "exit")
            {
                Expr result = new Exit();
                return result;
            }
            else
            {
                if (i == len - 1)
                {
                    std::vector<Expr> vec;
                    Expr result = new Apply(new Var(s), vec);
                    return result;
                }
                else
                {
                    for (auto i = env; i.get() != nullptr; i = i->next)
                    {
                        if (i->x == s)
                        {
                            if (!dynamic_cast<Closure *>(i->v.get()))
                                return new Var(s);
                        }
                    }
                    // func calling
                    std::vector<Expr> vec;
                    for (int j = i + 1; j < len; j++)
                        vec.push_back(stxs[j].parse(env));
                    Expr result = new Apply(stx.parse(env), vec);
                    return result;
                }
            }
        }
    }
}

#endif
