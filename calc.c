#include <stm32f10x.h>
#include <stdlib.h>

#include <stdio.h>

#include "stack.h"
#include "usart.h"

#define CMD_SIN		1
#define CMD_COS		1
#define CMD_TAN		1
#define CMD_SQRT	1
#define CMD_LOG		1
#define CMD_ANS		1

#define CMD_DOUBLE	1
#define CMD_POW		1

u8 priority(char ch)
{
	if(
		(ch=='*')||
		(ch=='/')
		)
		return 2;
	else if(
		(ch=='+')||
		(ch=='-')
		)
		return 1;
	else
		return 0;
}


u8 is_alpha(char ch)
{
	if(
		(ch==CMD_SIN)||
		(ch==CMD_COS)||
		(ch==CMD_TAN)||
		(ch==CMD_SQRT)||
		(ch==CMD_LOG)||
		(ch==CMD_ANS)
		)
		return 1;
	else
		return 0;
}


u8 is_postalpha(char ch)
{
	if(
		(ch==CMD_DOUBLE)
		)
		return 1;
	else
		return 0;
}


u8 is_op(char ch)
{
	if(
		(ch=='+')||
		(ch=='-')||
		(ch=='*')||
		(ch=='/')
		//(ch==CMD_POW)
		)
		return 1;
	else
		return 0;
}

u8 is_digit(char ch)
{
	if(
		(ch=='1')||
		(ch=='2')||
		(ch=='3')||
		(ch=='4')||
		(ch=='5')||
		(ch=='6')||
		(ch=='7')||
		(ch=='8')||
		(ch=='9')||
		(ch=='0')||
		(ch=='.')||
		(ch=='i')
		)
		return 1;
	else
		return 0;
}

void Infix2Postfix(char *infix,char *postfix)
{
	struct stack_char sym;
	empty_stack_char(&sym);
	
	while(*infix)
	{
		while(*infix==' ')
			infix++;
		if(is_digit(*infix))
		{
			while(is_digit(*infix))
			{
				*postfix=*infix;
				postfix++;
				infix++;
			}
			*postfix=' ';
			postfix++;
		}
		if(*infix=='(')
		{
			push_char(&sym,*infix);
			infix++;
		}
		if(*infix==')')
		{
			char tmp=pop_char(&sym);
			while(tmp!='(')
			{
				*postfix=tmp;
				postfix++;
				*postfix=' ';
				postfix++;
				tmp=pop_char(&sym);
			}
			infix++;
		}
		if(is_op(*infix))
		{
			if(is_stack_empty_char(&sym))
			{
				push_char(&sym,*infix);
			}
			else
			{
				char tmp=pop_char(&sym);
				while((priority(tmp)>=priority(*infix))&&!is_stack_empty_char(&sym))
				{
					*postfix=tmp;
					postfix++;
					*postfix=' ';
					postfix++;
					tmp=pop_char(&sym);
				}
				push_char(&sym,tmp);
				push_char(&sym,*infix);
			}
			infix++;
		}
	}
	while(!is_stack_empty_char(&sym))
	{
		char a=pop_char(&sym);
		*postfix=a;
		postfix++;
		*postfix=' ';
		postfix++;
	}
	*postfix='\0';
}


#define NUMBER_LENGTH_BUFFER	20
float EvalPostfix(char *postfix)
{
	struct stack_float real,imag;
	empty_stack_float(&real);
	empty_stack_float(&imag);
	
	while(*postfix)
	{
		while(*postfix==' ')
			postfix++;
		if(is_digit(*postfix))
		{
			char buff[NUMBER_LENGTH_BUFFER];
			char *buffptr=buff;
			u8 is_real;
			float value;
			while(is_digit(*postfix))
			{
				*buffptr=*postfix;
				postfix++;
				buffptr++;
			}
			buffptr--;
			if(*buffptr=='i')
			{
				is_real=0;
			}
			else
			{
				is_real=1;
				buffptr++;
			}
			*buffptr='\0';
			value=strtod(buff,NULL);
			if(is_real)
			{
				push_float(&real,value);
				push_float(&imag,0);
			}
			else
			{
				push_float(&real,0);
				push_float(&imag,value);
			}
		}
		else
			postfix++;
	}
	
	{
		char buf[20];
		float value;
		while(!is_stack_empty_float(&real))
		{
			value=pop_float(&real);
			sprintf(buf,"%f",value);
			USART_Send_Str(USART1,buf);
			USART_Send_Str(USART1,"\t");
			value=pop_float(&imag);
			sprintf(buf,"%f",value);
			USART_Send_Str(USART1,buf);
			USART_Send_Str(USART1,"\r\n");
		}
	}
	return 0;
}


