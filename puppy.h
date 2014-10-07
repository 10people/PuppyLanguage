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
	puppy��һ��С�����Խ�������֧�����еĳ���������縳ֵ��ѭ���������жϵ�
	��֧�ֽ�ʹ��c/c++��д�ĺ������뵽�ű���
*/

#ifndef __Pu_H__
#define __Pu_H__


typedef long long	PU_INT;
typedef double		PU_FLOAT;
typedef PU_FLOAT	PU_NUMBER;

typedef struct Pu Pu;
typedef struct __pu_value __pu_value;
typedef __pu_value* pu_value;

// �����ýű����õĺ�������
typedef void (*ScriptFunc)(Pu*, int argnum, pu_value*);

// �������
typedef enum PURESULT{
	PU_FAILED = -1,// ʧ��
	PU_SUCCESS = 0// �ɹ�
}PURESULT;

// ֵ����
typedef enum PUVALUETYPE{
	NIL = 11,	// ��
	NUM = 16,	// ����
	STR,		// �ַ���
	ARRAY,		// ����
    MAP,        // �ֵ�
	CORO,		// Э�̶���
	FILEHANDLE,	// �ļ����
	BOOLEANT,		// ����
	FUN,			// ����
    CPTR = 27       // ָ��
}PUVALUETYPE;

// ֵ���ɽű�ϵͳ�����ģ������û�����pu_new_value������
typedef enum PUVALUECREATEDBY{
	PU_SYSTEM,// �ű�ϵͳ
	PU_USER// �û�
}PUVALUECREATEDBY;// �޸��ɽű�ϵͳ������ֵ�ᵼ��ʧ��


#ifdef __cplusplus
extern "C"{
#endif

    typedef void * (*pumalloc)(size_t _Size); 
    typedef void   (*pufree)(void * _Memory); 

/**
*
*	��ý������汾
*	����ֵ��
*	�������汾
*	
*/
const char *pu_version();

/**
*
*	����һ���ű�����������
*	����ֵ��
*	Pu ָ��
*	
*/
Pu *pu_open();


/**
*
*	�ر�һ���ű�����������
*	������	
*	Pu *L �ű�����������
*
*/
void pu_close(Pu *L);


/**
*
*	���ؽű�
*	������	
*	Pu *L �ű�����������
*	const char *fname �ű��ļ���
*/
PURESULT pu_load(Pu *L, const char *fname);

/*
 *	�����ַ���
 */
PURESULT pu_loadbuff(Pu *L, const char *str);

/**
*
*	ִ�нű�
*	������	
*	Pu *L �ű�����������
*/
void pu_run(Pu *L);


/**
*
*	����Դ�������ֽ���
*	������
*	Pu *L �ű�����������
*	const char *fname Դ�����ļ���
*
*/
void pu_makebytecode(Pu *L, const char *fname);

/**
*
*	���ýű���������ֵ
*	������	
*	Pu *L �ű�����������
*	pu_value *v ����ֵ
*
*/
void pu_set_return_value(Pu *L, pu_value v);
pu_value pu_get_return_value(Pu *L);

/**
*
*	ע��һ�������ýű�����
*	������	
*	Pu *L �ű�����������
*	const char *funcname �ڽű��еĺ�������
*	ScriptFunc func ����ָ��
*	int argnum ��������
*
*/
void pu_reg_func(Pu *L, const char *funcname, ScriptFunc pfunc);

/**
*
*	��ֵת�����ַ���
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	���أ�
*	ֵ��������ַ���
*
*/
const char *pu_str(pu_value v);

/**
*
*	��ֵת��������
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	���أ�
*	ֵ�����������
*
*/
PU_NUMBER pu_num(pu_value v);

/**
*
*	��ֵת����ָ��
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	���أ�
*	ֵ�������ָ��
*
*/
void *pu_ptr(pu_value v);

/**
*
*	��������±���ָ���ֵ
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	int idx �����±�
*	���أ�
*	�����Ա
*
*/
pu_value pu_arr(pu_value v, int idx);

/**
*
*	���ֵ������
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	���أ�
*	����
*
*/
PUVALUETYPE pu_type(pu_value v);

/**
*
*	��ֵ����Ϊ�ַ���
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	const char *str Ҫ���õ��ַ���
*
*/
PURESULT pu_set_str(pu_value v, const char *str);

/**
*
*	��ֵ����Ϊ����
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	int num Ҫ���õ�����
*
*/
PURESULT pu_set_num(pu_value v, PU_NUMBER num);

/**
*
*	��ֵ����Ϊָ��
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	void *ptr Ҫ���õ�ָ��
*
*/
PURESULT pu_set_ptr(pu_value v, void *ptr);

/**
*
*	��ֵ����Ϊ����
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*	const int num Ҫ���õ�����
*
*/
PURESULT pu_set_arr(pu_value varr, int idx, pu_value v);

// ֵ���ɽű�ϵͳ�����ģ������û�����pu_new_value������
PUVALUECREATEDBY pu_value_created_by(pu_value v);

// �������������ֵ
PURESULT pu_push_arr(pu_value varr, pu_value v);


// ����������ɾ��ֵ
PURESULT pu_pop_arr(pu_value varr);

// �õ�ֵ�ĳ���
int pu_len(pu_value v);


/**
*
*	�����µ�ֵ
*	������	
*	Pu *L �ű�����������
*	���أ�
*	ֵ����
*
*/
pu_value pu_new_value(Pu *L);

/**
*
*	ɾ��һ��ֵ
*	������	
*	const pu_value v ��ű�������ֵ���͵Ķ���
*
*/
PURESULT pu_del_value(pu_value v);

/**
*
*	���ýű��еĺ���
*	������	
*	Pu *L �ű�����������
*	const char *function_name ������
*	const pu_value varr ��������
*
*/
pu_value pu_call(Pu *L, const char *function_name, pu_value varr);

/**
*
*	��ýű��е�ȫ�ֱ���
*	������	
*	Pu *L �ű�����������
*	const char *varname ������
*	����ֵ��
*	�ű��еı���
*
*/
pu_value pu_global(Pu *L, const char *varname);


/*
 *	����ִ��һ���ַ���
 */
int pu_eval(Pu *L, const char *str);


/*
 *	�õ�һ��ֵ���ַ�����ʽ
 */
void pu_val2str(Pu *L, pu_value *v, char *buff, int buffsize);

/*
 * �����Զ���malloc����
 */
pumalloc pu_set_malloc(pumalloc fun_malloc);

/*
 * �����Զ���free����
 */
pufree pu_set_free(pufree fun_free);


#ifdef __cplusplus
}
#endif

#endif


