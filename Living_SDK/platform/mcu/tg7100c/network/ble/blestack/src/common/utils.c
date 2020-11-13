/*****************************************************************************************
*
* @file utils.c
*
* @brief entry
*
* Copyright (C) Bouffalo Lab 2019
*
* History: 2019-11 crealted by Lanlan Gong @ Shanghai
*
*****************************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int params_filter(char** params,uint32_t *r)
{	
	char *p ;
	uint32_t result=0;
	uint8_t base=0;
	
	p = *params;
	
    if((*p == '0') && ((*(p+1) == 'x') || (*(p+1) == 'X')) ){
		p = p + 2;
		base = 16;
		
	}else{
		base = 10;
	}
	
	while(*p){
		result *= base;
	    if(*p >= '0' && *p<='9')
			result += *p-'0';
		else if(base==10)
			return -1;
			
		if(base==16){
			if(*p >= 'a' && *p<='f')
				result += *p-'a' + 10;
			else if(*p >= 'A' && *p<='F')
				result += *p-'A' + 10;
		
		}
		p++;
	}
		
	*r = result;
	return 0;
	
}


void get_bytearray_from_string(char** params, uint8_t *result,int array_size)
{
    
    int i = 0;
    char rand[3];

    for(i=0; i < array_size; i++){
        strncpy(rand, (const char*)*params, 2);
        rand[2]='\0';
        result[i] = strtol(rand, NULL, 16);
        *params = *params + 2;
    }
}

void get_uint8_from_string(char** params, uint8_t *result)
{
	uint32_t p = 0;
	int state=0;
	
	state = params_filter(params,&p);
	if(!state){
		*result = p & 0xff;
	}else
		*result = 0;
}

void get_uint16_from_string(char** params, uint16_t *result)
{
	uint32_t p = 0;
	int state=0;
	
	state = params_filter(params,&p);
	if(!state){
		*result = p & 0xffff;
	}else
		*result = 0;
}

void get_uint32_from_string(char** params, uint32_t *result)
{
    uint32_t p = 0;
	int state=0;
	
	state = params_filter(params,&p);
	if(!state){
		*result = p;
	}else
		*result = 0;
}

void reverse_bytearray(uint8_t *src, uint8_t *result, int array_size)
{
    for(int i=0; i < array_size;i++){
        result[array_size - i -1] = src[i];
    }
}

unsigned int find_msb_set(uint32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x80000000;

    if (!data) {
        return 0;
    }
    while ((data & mask) == 0) {
        count += 1u;
        mask = mask >> 1u;
    }
    return (32 - count);
}

unsigned int find_lsb_set(uint32_t data)
{
    uint32_t count = 0;
    uint32_t mask = 0x00000001;

    if (!data) {
        return 0;
    }
    while ((data & mask) == 0) {
        count += 1u;
        mask = mask << 1u;
    }
    return (1 + count);
}
