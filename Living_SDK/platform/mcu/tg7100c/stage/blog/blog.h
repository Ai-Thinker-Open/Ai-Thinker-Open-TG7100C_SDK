
#ifndef __BLOG_H__
#define __BLOG_H__

#include <utils_log.h>

#ifdef TG7100C_BLOG_PRINT

#define blog_trace  printf//log_trace
#define blog_debug  printf//log_debug
#define blog_info   printf//log_info
#define blog_warn   printf//log_warn
#define blog_error  printf//log_error
//#define blog_assert log_assert
#define blog_print  printf
#define blog_buf(...)

#else

#define blog_trace(...)  //log_trace
#define blog_debug(...)  //log_debug
#define blog_info(...)   //log_info
#define blog_warn(...)   //log_warn
#define blog_error(...)  //log_error
#define blog_print(...)  
#define blog_buf(...)

#endif

#endif

