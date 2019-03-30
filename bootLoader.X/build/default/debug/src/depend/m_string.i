
# 1 "src/depend/m_string.c"

# 13 "C:\Program Files (x86)\Microchip\xc8\v2.00\pic\include\c90\stdint.h"
typedef signed char int8_t;

# 20
typedef signed int int16_t;

# 28
typedef __int24 int24_t;

# 36
typedef signed long int int32_t;

# 52
typedef unsigned char uint8_t;

# 58
typedef unsigned int uint16_t;

# 65
typedef __uint24 uint24_t;

# 72
typedef unsigned long int uint32_t;

# 88
typedef signed char int_least8_t;

# 96
typedef signed int int_least16_t;

# 109
typedef __int24 int_least24_t;

# 118
typedef signed long int int_least32_t;

# 136
typedef unsigned char uint_least8_t;

# 143
typedef unsigned int uint_least16_t;

# 154
typedef __uint24 uint_least24_t;

# 162
typedef unsigned long int uint_least32_t;

# 181
typedef signed char int_fast8_t;

# 188
typedef signed int int_fast16_t;

# 200
typedef __int24 int_fast24_t;

# 208
typedef signed long int int_fast32_t;

# 224
typedef unsigned char uint_fast8_t;

# 230
typedef unsigned int uint_fast16_t;

# 240
typedef __uint24 uint_fast24_t;

# 247
typedef unsigned long int uint_fast32_t;

# 268
typedef int32_t intmax_t;

# 282
typedef uint32_t uintmax_t;

# 289
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;

# 7 "src/depend/m_string.h"
extern void m_mem_set(uint8_t* buf,uint8_t x,uint16_t len);
extern void m_mem_cpy(uint8_t* d,uint8_t* s);
extern void m_mem_cpy_len(uint8_t* d,uint8_t* s,uint16_t len);
extern uint16_t m_str_match(uint8_t* b,uint8_t* c);
extern int32_t m_math_pow(int32_t x,int32_t y);
extern uint8_t m_str_cmp(uint8_t* d,uint8_t* s);
extern uint8_t m_str_cmp_len(uint8_t* d,uint8_t* s,uint16_t len);
extern uint16_t HEX8(uint8_t* hex,uint8_t x);
extern uint16_t m_str_b2h(uint8_t* h,uint8_t* b,uint16_t len);
extern uint16_t m_str_h2b(uint8_t* b,uint8_t* h,uint16_t len);


extern void m_int_2_str(uint8_t* buf,int32_t x,uint16_t len);
extern uint16_t m_str_head_hide(uint8_t* str,uint16_t loc);

extern const uint8_t Bcd2HexTable[];
extern const uint8_t Hex2BcdTable[];

# 29
extern uint32_t swap_uint32(uint32_t x);
extern uint16_t swap_uint16(uint16_t x);




extern void int32_array_shift_right(int32_t *p32,uint16_t eoc);

# 3 "src/depend/m_string.c"
const uint8_t HexTable[]="0123456789ABCDEF";

# 32
void m_mem_set(uint8_t* buf,uint8_t x,uint16_t len)
{
while(len--)
{
*buf++=x;
}
}
void m_mem_cpy(uint8_t* d,uint8_t* s)
{
while(*s!='\0')
{
*d++=*s++;
}
*d='\0';
}
void m_mem_cpy_len(uint8_t* d,uint8_t* s,uint16_t len)
{

while(len)
{
*d++=*s++;
len--;
}
}
uint8_t m_str_cmp(uint8_t* d,uint8_t* s)
{
uint16_t ret=1;
while(*d!='\0' && *s!='\0')
{
if(*d != *s)
{
ret=0;
break;
}
s++;d++;
}
return ret;
}
uint8_t m_str_cmp_len(uint8_t* d,uint8_t* s,uint16_t len)
{
uint16_t ret=1;
while(len--){
if(*d != *s){
ret=0;
break;
}
s++;d++;
}
return ret;
}
uint16_t m_str_match(uint8_t* b,uint8_t* c)
{
uint8_t *bb,*cc;
uint16_t ret=0;
while(*b!='\0' )
{
cc=c;
bb=b++;
ret=0;
while(*bb!='\0' && *cc!='\0')
{
if(*bb!=*cc)
{
ret=0;
break;
}
bb++;
cc++;
}
if(*cc=='\0')
{


return 1;
}
}
return ret;
}

# 113
int32_t m_math_pow(int32_t x,int32_t y)
{
int32_t ret=1;
if(y==0)return 1;
while(y--)
{
ret*=x;
}
return ret;
}

# 127
void m_int_2_str(uint8_t* buf,int32_t x,uint16_t len)
{
int32_t t;
if(!len)return;

t=m_math_pow(10,len);
if(x>=t-1)x=t-1;
while(len--)
{
t=x%10;

*buf-- = ('0'+(uint8_t)t);
x/=10;
}
}

# 146
uint16_t m_str_head_hide(uint8_t* str,uint16_t loc)
{
uint16_t ret=0;
while(loc--)
{
if(*str=='0')
{
*str++=' ';
ret++;
}
else
{
break;
}
}
return ret;
}

# 166
uint16_t HEX8(uint8_t* hex,uint8_t x)
{
*hex++=HexTable[(x&0xf0)>>4];
*hex++=HexTable[(x&0x0f)];
hex='\0';
return 3;
}

# 177
uint16_t m_str_b2h(uint8_t* h,uint8_t* b,uint16_t len)
{
uint16_t i=0;
uint8_t t8;
for(i=0;i<len;i++)
{
t8=*b++;
*h++=HexTable[(t8&0xf0)>>4];
*h++=HexTable[(t8&0x0f)];
}
return (len<<1);
}

# 197
uint8_t _chr2hex(uint8_t c){
uint8_t h=0;
if(c>='0' && c <='9')h|=(c-'0');
else if(c>='A' && c<='F')h|=(c-'A'+0x0a);
else if(c>='a' && c<='f')h|=(c-'a'+0x0a);
else h|=0;
return h;
}

# 208
uint16_t m_str_h2b(uint8_t* b,uint8_t* h,uint16_t len)
{
uint16_t i=len;
uint8_t bb,s;
while(i>=2)
{
bb=0;
s=*h++;

bb=_chr2hex(s);
bb<<=4;
s=*h++;
bb= bb+_chr2hex(s);
*b++=bb;
i-=2;
}
return (len>>1);
}

uint32_t swap_uint32(uint32_t x)
{
uint32_t ret;
uint8_t *d,*s;
s=(uint8_t*)&x;
d=(uint8_t*)&ret;
d[0]=s[3];
d[1]=s[2];
d[2]=s[1];
d[3]=s[0];
return ret;
}
uint16_t swap_uint16(uint16_t x)
{
uint16_t ret;
uint8_t *d,*s;
s=(uint8_t*)&x;
d=(uint8_t*)&ret;
d[0]=s[1];
d[1]=s[0];
return ret;
}

void int32_array_shift_right(int32_t *p32,uint16_t eoc)
{
while(eoc>=1){
p32[eoc]=p32[eoc-1];
eoc--;
}
}

