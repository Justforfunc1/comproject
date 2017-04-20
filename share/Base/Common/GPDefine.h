/**********************************************************
 * \file GPDefine.h
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#ifndef _GPDefine_H
#define _GPDefine_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <queue>
#include <fstream>
#include <sstream>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <hash_map>

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef WIN32
#define __stdcall
#endif

#ifdef WIN32
#define  HASH_NAME    std
#else
#define  HASH_NAME    __gnu_cxx
#endif

typedef signed char			GPInt8;
typedef signed short		GPInt16;
typedef signed int			GPInt32;
typedef signed long long    GPInt64;

typedef unsigned char			GPUInt8;
typedef unsigned short			GPUInt16;
typedef unsigned int			GPUInt32;
typedef unsigned long long		GPUInt64;

typedef unsigned char	GPByte;
typedef char	GPChar;
typedef float	GPFloat;
typedef double	GPDouble;

typedef bool	GPBOOL;
typedef void	GPVOID;

typedef ::std::string				GPString;
typedef ::std::vector<GPBOOL>		GPBOOLVector;
typedef ::std::vector<GPChar>		GPCharVector;
typedef ::std::vector<GPInt16>		GPInt16Vector;
typedef ::std::vector<GPInt32>		GPInt32Vector;
typedef ::std::vector<GPInt64>		GPInt64Vector;
typedef ::std::vector<GPFloat>		GPFloatVector;
typedef ::std::vector<GPDouble>		GPDoubleVector;
typedef ::std::vector<GPString>		GPStringVector;


#define GP_SAFE_DELETE(_point_)   if(_point_ != NULL) { delete _point_; _point_ = NULL; }
#define GP_SAFE_DELETE_NOT_NULL(_point_)   if(_point_ != NULL) { delete _point_; }
#define GP_SAFE_DELETE_ARRAY(_point_array_) if(_point_array_ != NULL) {delete [] _point_array_; _point_array_ = NULL; }
#define GP_SAFE_DELETE_ARRAY_NOT_NULL(_point_array_) if(_point_array_ != NULL) {delete [] _point_array_; }

#define MAX_LENGTH_1024		1024
#define MAX_LENGTH_512		512
#define MAX_LENGTH_128		128
#define MAX_LENGTH_64		64
#define MAX_LENGTH_16		16

#define GP_ERROR			1
#define GP_SUCCESS			0

#endif

