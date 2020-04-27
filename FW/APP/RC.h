#ifndef __RC_H__
#define __RC_H__
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#ifdef __cplusplus
extern "C"
{
#endif

//通过名称获取资源大小
size_t RCGetSize(const char * name);

//通过名称获取资源指针
const unsigned char * RCGetHandle(const char * name);

#ifdef __cplusplus
};
#endif
#endif // __RC_H__
