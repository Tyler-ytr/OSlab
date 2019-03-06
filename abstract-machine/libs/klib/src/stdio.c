#include "klib.h"
#include <stdarg.h>

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define ZEROPAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define LEFT 16
#define SPECIAL 32
#define LARGE 64
 int _div(long* n,unsigned base)
	 {
		      int __res; 
			           __res = ((unsigned long) *n) % (unsigned) base; 
					            *n = ((unsigned long) *n) / (unsigned) base; 
								         return __res;
										  }

#define do_div(n,base) _div(&n,base)
size_t strnlen(const char *s,size_t count)
	  {
		        
				      if(s==NULL)
						            return -1;
					       size_t n;
						         size_t cnt=count;
								       for(n=0;n<cnt && *s;n++,s++)
										            ;
									       return n;
										     }      

void myputc(char c,char** s_h)
{
//	if(*s_h==0)_putc(c);
//	else
//	{
	**s_h=c;
	(*s_h)++;
//	}
}

static inline int isdigit(int ch)
{
	return (ch>='0')&&(ch<='9');
}



static int skip_atoi(const char **s)
{
	int i=0;
	while(isdigit(**s))
	{
	
	i=i*10+*((*s)++)-'0';
	
	}return i;

}


static char* getnumber(char *str,long num,int base,int size,int precision,int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;
	sign=0;
	if(type&LARGE)
		digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if(type&LEFT)
		type&=~ZEROPAD;
	if(base<2||base>36)
	return 0 ;	
	c=(type&ZEROPAD)?'0':' ';
	sign=0;


	if(type&SIGN)
	{
		if(num<0)
		{
			sign='-';
			num=-num;
			size--;
		}
		else if(type&PLUS)
		{
			sign='+';
			size--;
		}
		else if(type&SPACE)
		{
			sign=' ';
			size--;
		}
	}
		
	if(type&SPECIAL)
	{
		if(base==16)
			size-=2;
		else if(base==8)
			size--;
	
	}

	i=0;
	if(num==0)
	{
	tmp[i++]='0';
	}
	else
	{
		while(num!=0)
		{
		
		tmp[i++]=digits[do_div(num,base)];
		
		}	
	
	}

	if(i>precision)
		precision=i;
	size-=precision;
	if(!(type&(ZEROPAD+LEFT)))
		while(size-->0)
		{//		*str++=' ';
		myputc(' ',&str);
		}
	if(sign&&sign!=0)
	{//	*str++=sign;
		myputc(sign,&str);
	}
	if(type&SPECIAL)
	{
		if(base==8)
		{//	*str++=0;
		myputc('0',&str);
		
		}
		else if(base==16)
		{
		//	*str++='0';
		//	*str++=digits[33];
		myputc('0',&str);
		myputc(digits[33],&str);
		}
	
	
	}

	if(!(type&LEFT))
	{while(size-->0)
		{//	*str++=c;
		myputc(c,&str);
		}}
		while(i<precision--){
//		*str++='0';
		myputc('0',&str);
	}

		while(i-->0)
		{//	*str++=tmp[i];
		myputc(tmp[i],&str);
		
		}
		while(size-->0)
		{	//	*str++=' ';
		myputc(tmp[i],&str);
		
		}

	return str;


}
/*void myputc(char c,char** s_h)
{
	if(*s_h==0)_putc(c);
	else
	{
	**s_h=c;
	(*s_h)++;
	}
}*/
int vprintf(char *out,const char* fmt,va_list ap)
{
	const char*format=fmt;//format==fmt;
	char *str=out;
//	char **s_p=&str;
const	 char *s;
	int len,i;
	unsigned long num;
	int flags=0;
	int precision;
	int qualifier;
	int base;
	int field_width=-1;
	for(str=out;*format;++format)
	{
		if(*format!='%')
		{ 
//			*str++=*format;
			

	myputc(*format,&str);

			continue;
		
 		} 
			
		flags=0;
		repeat:
		format++;
		switch (*format)
			{	
			case '-':flags|=LEFT;
					 goto repeat;
			case '+':flags|=PLUS;
					 goto repeat;
			case ' ':
					 flags|=SPACE;
					 goto repeat;
			case '#':flags|=SPECIAL;
					 goto repeat;
			case '0':flags|=ZEROPAD;
					 goto repeat;
 			}
		//change process flags
			
		//get field width
		//wait to do
		field_width=-1;
		if(isdigit(*format))
 		{//	Log("digit has not been set");
			field_width=skip_atoi(&format);}
		else if(*format=='*'){
		//	Log("* is not set");
		//	assert(0);
		format++;			
 				}
		
			precision=-1;
			if(*format=='.')
 			{
		//		Log(". is not set");
			//	assert(0);
			format++;	
				if (isdigit(*format))
			 	{//	Log(". is not set");
					precision=skip_atoi(&format);	}
				else if(*format=='*')
				{
			
				//	Log("* is not set");
					format++;
					precision=va_arg(ap,int);
			
				}
				if(precision<0)
					precision=0;
			}
			//get the conversion qualifier
			qualifier=-1;
			if(*format=='h'||*format=='l'||*format=='L')
			{
				qualifier=*format;
				++format;
			
			}
	base=10;
			switch(*format){	
			case 'c': 
				//	Log("I am in c");
				assert(0);
			case 'e':
				//	Log("I am in e");assert(0);
			case 'E':
				//	Log("I am in E");assert(0);
			case 'f':
				//	Log("I am in f");assert(0);
			case 'g':
				//	Log("I am in g");assert(0);
			case 'G':
				//	Log("I am in G");assert(0);
			case 'o':
				//	Log("I am in o");
				assert(0);
			case 's':
					s=va_arg(ap,char *);
					len=strnlen(s,precision);
			//	char*k=s;	
					if(!(flags&LEFT))
						while(len<field_width--)
						{//	*str++=' ';
							myputc(' ',&str);
						}	

					for(i=0;i<len;++i)
					{//*str++=*s++;
					myputc(*s++,&str);
			

					}
					while(len<field_width--)
					{//		*str++=' ';
						myputc(' ',&str);
					}
					continue;
			case 'x':
					base=16;break;
			case 'X':
					flags|=LARGE;
					base=16;break;
			case 'p':
				//	assert(0);

				//	Log("I am in p");assert(0);
					if(field_width==-1)
					{	field_width=2*sizeof(void*);
					flags|=ZEROPAD;
					}
			str=getnumber(str,(unsigned long)va_arg(ap,void*),16,field_width,precision,flags);
		continue;				
			case 'n':
					if(qualifier=='1')
					{
					long *ip=va_arg(ap,long *);
					*ip=(str-out);
					}else
					{
					int *ip=va_arg(ap,int *);
					*ip=(str-out);
					}//maybe wrong when it is printf
			case '%':
				//	Log("I am in %");
				//	assert(0);
					myputc('%',&str);continue;
			case 'd':
			case 'i':
					flags|=SIGN;
			case 'u':break;
			default:
			//		*str++='%';
				myputc('%',&str);
				   if(*format)
				   { 
					  // *str++=*format;
					myputc(*format,&str);
				   
				   }
				   else
				--format;
			continue;	   ;
			}
			if(qualifier=='l')
				num=va_arg(ap,unsigned long);
			else if(qualifier=='h')
			{
				num=va_arg(ap,int);
				if(flags&SIGN)
				num=(short)num;	
			}
			if(flags&SIGN)
				num=va_arg(ap,int);
			else
				num=va_arg(ap,unsigned int);

			str=getnumber(str,num,base,field_width,precision,flags);
	}
//	if(*str!='0')
	*str='\0';
		
		return (str-out);

}





int printf(const char *fmt, ...) {
//Log("I am in printf")
//only work for buf<200,maybe out
char buf[200];
	va_list ap;
	va_start(ap,fmt);
int ret=vprintf(buf,fmt,ap);
if(ret<0)return ret;
	;
for(int i=0;buf[i]!='\0';i++)
	_putc(buf[i]);
return ret;
va_end(ap);

	return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
//Log("I am in vsprintf");
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int ret=vprintf(out,fmt,ap);
	va_end(ap);
	return ret;
	
//	return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
//Log("I am in snprintf");
  return 0;
}

#endif
