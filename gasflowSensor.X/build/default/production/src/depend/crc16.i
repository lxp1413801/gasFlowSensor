
# 1 "src/depend/crc16.c"

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

# 10 "src/depend/crc16.h"
extern uint16_t crc_sum(uint8_t *puchMsg, uint16_t usDataLen);
extern uint8_t crc_verify(uint8_t* message, uint16_t n);
extern void crc_append( uint8_t* message, uint16_t n);

# 7 "src/depend/m_string.h"
extern void m_mem_set(uint8_t* buf,uint8_t x,uint16_t len);
extern uint16_t m_mem_cpy(uint8_t* d,uint8_t* s);
extern void m_mem_cpy_len(uint8_t* d,uint8_t* s,uint16_t len);
extern uint16_t m_str_match(uint8_t* b,uint8_t* c);
extern int32_t m_math_pow(int32_t x,int32_t y);
extern uint8_t m_str_cmp(uint8_t* d,uint8_t* s);
extern uint8_t m_str_cmp_len(uint8_t* d,uint8_t* s,uint16_t len);
extern uint16_t HEX8(uint8_t* hex,uint8_t x);
extern uint16_t m_str_b2h(uint8_t* h,uint8_t* b,uint16_t len);
extern uint16_t m_str_h2b(uint8_t* b,uint8_t* h,uint16_t len);


extern void m_int_2_str(uint8_t* buf,int32_t x,uint16_t len);
extern void m_int_2_str_ex(uint8_t* buf,int32_t x,uint16_t ssize);
extern uint16_t m_str_head_hide(uint8_t* str,uint16_t loc);

extern const uint8_t Bcd2HexTable[];
extern const uint8_t Hex2BcdTable[];

# 30
extern uint32_t swap_uint32(uint32_t x);
extern uint16_t swap_uint16(uint16_t x);




extern void int32_array_shift_right(int32_t *p32,uint16_t eoc);

# 116 "src/depend/crc16.c"
uint16_t crc_sum( uint8_t* buf, uint16_t len)
{
uint16_t crc=0xFFFF;
uint16_t i, j;
for ( j=0; j<len;j++){
crc=crc ^*buf++;
for ( i=0; i<8; i++){

if(crc&0x0001){
crc=crc>>1;
crc=crc^ 0xa001;
}else{
crc=crc>>1;
}
}

}
return ( crc);
}
uint8_t crc_verify( uint8_t* message, uint16_t n)
{
uint16_t expected,crccal;
m_mem_cpy_len((uint8_t*)(&expected),(message+n-2),2);
crccal=crc_sum(message,n - 2);
if(expected!=crccal)return 0;
return 1;
}
void crc_append(uint8_t* message, uint16_t n)
{
uint16_t crc;
crc = crc_sum(message, n);
message[n] = (uint8_t)(crc & 0xff);
message[n+1] = (uint8_t)((crc >> 8) & 0xff);
}

