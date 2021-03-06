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

#include "state.h"
#include <sys/stat.h>
#include <time.h>

extern void append_token(Pu *L, Token &t);
extern void parse_include(Pu *L, const Token &filename);
extern void parse_function(Pu *L, int token_from, FILE *pbcf, TokenList *tl);
extern Token get_token_from_file(Pu *L, TokenList *t);
extern bool check_complete(Pu *L);
extern void parse_function_body(Pu *L, 
                                int ldef, 
                                int token_from, 
                                Token &t, 
                                FILE *pbcf=0, 
                                TokenList *tl=0);

static void save_token(FILE *pbcf, TokenList &tokens)
{
    TokenList::iterator it = tokens.begin();
    TokenList::iterator ite = tokens.end();
    while (it != ite)
    {
        int ctp = it->type;
        fwrite(&ctp, sizeof(ctp), 1, pbcf);
        switch (it->type)
        {
        case INTEGER:
            fwrite(&it->literal_value->intVal(), sizeof(it->literal_value->intVal()), 1, pbcf);
            break;
        case NUM:
            fwrite(&it->literal_value->numVal(), sizeof(it->literal_value->numVal()), 1, pbcf);
            break;
        case OP:{
            char cop = (char)it->optype;
            fwrite(&cop, sizeof(cop), 1, pbcf);
                }break;
        case VAR:{
            char sl = (char)it->name->length();
            fwrite(&sl, sizeof(sl), 1, pbcf);
            fwrite(it->name->c_str(), sl, 1, pbcf);
                 }break;
        case STR:{
            size_t sl = it->literal_value->strVal().length();
            fwrite(&sl, sizeof(size_t), 1, pbcf);
            fwrite(it->literal_value->strVal().c_str(), it->literal_value->strVal().length(), 1, pbcf);
                 }break;
        default:
            break;
        }
		fwrite(&it->line, sizeof(it->line), 1, pbcf);
        ++it;
    }
}

void savebytecode(Pu *, const char *fname, TokenList &tokensave)
{
    FILE *pbcf = fopen(fname, "wb");
    time_t ti; 
    ti = time(nullptr);
    fwrite(&ti, sizeof(ti), 1, pbcf);
    save_token(pbcf, tokensave);
    fclose(pbcf);
}

void get_nextbytetoken(Pu *L, FILE *pbcf, Token &t)
{
    int type = UNKNOWN;
    bool needinclude = false;
    for (;;)
    {
        if (fread(&type, sizeof(char), 1, pbcf))
        {
            t.type = (PuType)type;
            t.line = 0;

            switch (type)
            {
            case INTEGER:{
				PU_INT number = 0;
				fread(&number, sizeof(number), 1, pbcf);
				t.literal_value = get_int_literal(L, number);
				} break;

            case NUM:{
				PU_NUMBER number = 0;
				fread(&number, sizeof(number), 1, pbcf);
				t.literal_value = get_num_literal(L, number);
				} break;

            case OP:{
                char cop = 0;
                fread(&cop, sizeof(cop), 1, pbcf);
                t.optype = (OperatorType)cop;
                } break;

            case VAR:{
                char l = 0;
                fread(&l,sizeof(l),1,pbcf);
                char *c = (char*)malloc(l + 1);
                memset(c,0,l+1);
                fread(c, 1, l, pbcf);
                t.name = InsertStrPool(L, c);
                free(c);
                } break;

            case STR:{
                size_t l = 0;
                fread(&l, sizeof(l), 1, pbcf);
                char *c = (char*)malloc(l + 1);                
                fread(c, 1, l, pbcf);
                c[l] = 0;
				t.literal_value = get_str_literal(L, c);
				free(c);
                } break;
            }
			t.filename = L->current_filename.back();
			fread(&t.line, sizeof(t.line), 1, pbcf);
            append_token(L,t);
        }

        if (type != KW_INCLUDE)
        {
            break;
        }
        else
        {
            needinclude = true;
        }
    }
    
	if (needinclude)
	{
		parse_include(L, t);
	}
	else if (type == KW_FUNCTION)
	{
		parse_function(L, FROM_BYTECODE, pbcf, 0);
	}
}

static void get_token_from_bytecode(Pu *L, FILE *pbcf)
{
    for (;;)
    {
        Token t;
        t.type = UNKNOWN;
        get_nextbytetoken(L,pbcf, t);
        if (t.type == FINISH)
            break;
    }
}

PUAPI void pu_getbytecodename(char *bytecodename, 
                              const char *csource_code_name)
{
    size_t i = strlen(csource_code_name) - 1;
    for ( ; i != 0; --i)
    {
        if (csource_code_name[i] == '.')
        {
            break;
        }
    }
    
    memcpy(bytecodename, csource_code_name, i);
    memcpy(bytecodename + i, ".puc", 5);
}


PUAPI int pu_loadbytecode(Pu *L, const char *fname)
{
    FILE *pbcf = fopen(fname, "rb");
    if (!pbcf)
        return 0;

    time_t ti; 
    fread(&ti, sizeof(ti), 1, pbcf);

    struct stat buf;
    char sfn[1024] = {0};
    strncpy(sfn, fname, strlen(fname)-1);
    if (stat( sfn ,&buf) != 0)
    {
        fclose(pbcf);
        return 0;
    }
    else if (buf.st_mtime > ti)
    {
        fclose(pbcf);
        return 0;
    }
	L->current_filename.push_back(InsertStrPool(L, fname));
    get_token_from_bytecode(L, pbcf);
	L->current_filename.pop_back();
    fclose(pbcf);
    return 1;
}

PUAPI PURESULT pu_load(Pu *L, const char *sname)
{
    size_t sname_index_max = strlen(sname)-1;
    for ( ; sname_index_max != 0; --sname_index_max)
    {
        if (sname[sname_index_max] == '.')
            break;
    }

    if (strcmp(&(sname[sname_index_max]), ".puc") == 0)
    {
        return (PURESULT)pu_loadbytecode(L, sname);
    }
    
    L->source.pf = fopen(sname, "r");
    L->source.type = Pusource::ST_FILE;
    if (L->source.pf == nullptr)
    {
        return PU_FAILED;
    }

	L->current_filename.push_back(InsertStrPool(L, sname));

    if (L->tokens.size() > 0 && L->tokens.back().type == FINISH)
    {
        if (L->cur_token == (int)L->tokens.size() - 1)
        {
            L->cur_token--;
        }
        L->tokens.pop_back();
    }

    TokenList t;
    Token temp;
    do
    {
        temp = get_token_from_file(L, &t);
        CHECKTOKENERROR PU_FAILED;
    }while(temp.type != FINISH);
    fclose(L->source.pf);
    L->source.pf = nullptr;

    if (!check_complete(L))
    {
        return PU_FAILED;
    }
    
    char bname[1024]={0};
    if (sname_index_max + 1 > sizeof(bname))
    {
        return PU_FAILED;
    }
    pu_getbytecodename(bname, sname);
    savebytecode(L, bname, t);
    t.clear();
    L->cur_token = 0;
    L->current_filename.pop_back();
    L->jumpstack.release();
    return PU_SUCCESS;
}


PUAPI PURESULT pu_loadbuff(Pu *L, const char *str)
{
    if (str == nullptr)
    {
        return PU_FAILED;
    }

    const char *buffname = "<stream buffer>";
	L->current_filename.push_back(InsertStrPool(L, buffname));
    L->source.type = Pusource::ST_BUFFER;
    L->source.str.buff = str;
    L->source.str.pos = 0;
    
    Token temp = get_token_from_file(L,nullptr);
     while(L->tokens[L->tokens.size()-1].type != FINISH) {        
        if (L->funstack.size() > 0 && L->funstack.back() >= 1)// 如果有函数未结束，继续获取函数内容
            parse_function_body(L,L->uncomdef.top(), 
            FROM_SOURCECODE, temp, 
            nullptr,nullptr);
        else
            temp = get_token_from_file(L,nullptr);
        CHECKTOKENERROR PU_FAILED;
    }
    L->source.str.buff = nullptr;
    L->source.str.pos = 0;
    L->cur_token = 0;
    L->current_filename.pop_back();
    return PU_SUCCESS;
}


PUAPI void pu_makebytecode(Pu *L, const char *sname)
{
    pu_load(L, sname);
    L->cur_token = 0;
    char codename[1024]={0};
    pu_getbytecodename(codename, sname);
    TokenList t;
    do
    {    
        NEXT_TOKEN;
        t.push_back(TOKEN);
    }while(TOKEN.type != FINISH);
    savebytecode(L,codename,t);
}
