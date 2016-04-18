/*
    Copyright (c) 2009 Zhang li

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/*
    Author zhang li
    Email zlvbvbzl@gmail.com
*/

#ifndef __VM_DEF_H__
#define __VM_DEF_H__

#include "config.h"

struct Pu;
struct Token;
struct __pu_var;
typedef __pu_var* pu_var; 
struct FuncPos;

#define PUAPI extern "C"

// �����ýű����õĺ�������
typedef void (*ScriptFunc)(Pu*, int arg_num, pu_var*);

// ������ص�
typedef void (*ErrHandle)(int err, const char *err_str);

// ��������Ļص�
typedef void (*OutputHandle)(const char *str);

#define QUIT_SCRIPT            {L->isquit=true; L->cur_token = L->tokens.size() - 1;NEXT_TOKEN}
#define TOKEN                (*(L->token))
#define NEXT_TOKEN            {L->token = &(L->tokens[L->cur_token++]);}
#define NEXT_TOKEN_N(n)        assert(n!=0);{L->token = &(L->tokens[L->cur_token+n-1]);L->cur_token+=n;}
#define PREV_TOKEN            {L->cur_token-=2;NEXT_TOKEN;}
#define PREV_TOKEN_N(n)        {L->cur_token-=1+n;NEXT_TOKEN;}

// �������
typedef enum PURESULT{
    PU_FAILED = -1,// ʧ��
    PU_SUCCESS = 0// �ɹ�
}PURESULT;

#ifdef _MSC_VER
#define PU_SNPRINTF(dest, size, fmt, ...) _snprintf(dest, size, fmt, __VA_ARGS__)
#else
#define PU_SNPRINTF(dest, size, fmt, ...) snprintf(dest, size, fmt, __VA_ARGS__)
#endif

#define MAKE_TEMP_VALUE(p) \
    if (L->tempool.size() > 0)\
    {\
        p = L->tempool.back();\
        L->tempool.pop_back();\
    }\
    else\
    {\
        p = new __pu_var(L);\
    }    \
    L->tempvals.push_back(p);

#define CHECK_EXP(v) \
    if (!v || v->type() == UNKNOWN)\
    {\
        error(L, 7);\
        return;\
    }
    
#define CHECK_EXP_RETURN(v, r) \
    if (!v || v->type() == UNKNOWN)\
    {\
        error(L, 7);\
        return r;\
    }

#define CHECK_EXP_RETURNERR(v) \
    if (!v || v->type() == UNKNOWN)\
    {\
        error(L, 7);\
        __pu_var *_r = nullptr;\
        MAKE_TEMP_VALUE(_r);\
        return _r;\
    }
#endif
