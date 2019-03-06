#include "klib.h"
//#include "am.h"
//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
size_t strlen(const char *s) {
/*	const char *p=s;
	if(s==NULL)
		return -1;

	while(*p)
	{p++;
	}
	
//	_put((p-s));
  	return ((int)(p-s));*/
	size_t i=0;
	while(*(s+i)!='\0')
	{i++;
	}return i;
}
/*size_t strnlen(const char *s,size_t count)
{
	const char *p=s;
	if(s==NULL)
		return -1;
	size_t n;
	size_t cnt=count;
	for(n=0;n<cnt && *str;n++,str++)
		;
	return n;
}
*/
char *strcpy(char* dst,const char* src) {
	assert(dst!=NULL&&src!=NULL);
	char *ret=dst;
	int cnt=strlen(src)+1;
	if(dst>=src&&dst<=src+cnt-1)
	{
		dst=dst+cnt-1;
		src=src+cnt-1;
		while(cnt--)
		{
			*dst--=*src--;
		}
	}//in case of (strcpy(s+1,s);
	else
	{
		while(cnt--)
		{
			*dst++=*src++;
		}
		
	}
//	_put(*ret);
	return ret;
	// return NULL;
}

char* strncpy(char* dst, const char* src, size_t n) {
/*  char *s=dst;
	while(*src!='\0'&&0<n--)
	{
		*s++=*src++;
	
	}
	while(0<n--)
	{
		*s++='\0';
	}
//	_put(*dst);
	return dst;*/
	size_t i;
	for(i=0;i<n&&src[i]!='\0';++i)
	{dst[i]=src[i];
	}for(;i<n;i++)
	{
	dst[i]='\0';
	}return dst;
}

char* strcat(char* dst, const char* src) {
	char *s=NULL;
	for(s=dst;*s!='\0';++s)
	{;}
	for(;(*s=*src)!='\0';++s,++src)
	{;}
//	_put(dst);
	return dst;
}

int strcmp(const char* s1, const char* s2) {
/*  int flag=0;
  while(*s1==*s2)
  {
	if(*s1!='\0')
	{
		s1++;
		s2++;
	}
	else
		return 0;
  }

	flag=*s1-*s2;
//	_put(*s1);
//	_put(*s2);
	return flag;*/
	int len1=strlen(s1);
//	_put(*s2);
	int len2=strlen(s2);
	int len=0;
	if(len1<len2)len=len1;
	else len=len2;
	for(int i=0;i<=len;i++)
	{if(s1[i]<s2[i])return -1;
		else if(s1[i]>s2[i])return 1;

	}
return 0;

}


int strncmp(const char* s1, const char* s2, size_t n) {
  

  while(n)
  {
	if(*s1==*s2)
	{
		s1++;
		s2++;
	}
	else
	{
		return *s1-*s2;}
	n--;
  }

	return *s1-*s2;
}

void* memset(void* v,int c,size_t n) {
	char * s=(char*)v;
	const unsigned char uc=c;
	for(;n>0;++s,--n)
	{
	*s=uc;
	}
	return v;
	
	
	
	//  return NULL;
}

void* memcpy(void* out, const void* in, size_t n) {
	unsigned char*s=(unsigned char*)out;
	unsigned char*sin=(unsigned char*)in;
	while(n--)
	{
		*s=*sin;
		s++;
		sin++;
	}
	
	
	
	return out;
	
	
	// return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
	unsigned char* a=(unsigned char*)s1;
	unsigned char* b=(unsigned char*)s2;
/*	while(n)
	{
		if(*a==*b)
		{
			a++;
			b++;
		}
		else
		{
			return *a-*b;
		
		}
		n--;
	}
   
return *a-*b;
*/
	for(int i=0;i<n;i++)
	{	
		if(a[i]>b[i])
			return (a[i]-b[i]);
		else if(a[i]<b[i])
			return (a[i]-b[i]);



	}
return 0;

	
}

#endif
