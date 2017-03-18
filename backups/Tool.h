/**********************************************************
 *  \file Tool.h
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
**********************************************************/
#ifndef _Tool_H
#define _Tool_H
#include "common.h"

class Tool {

public:
	static int mkDir(char *path);
	static int mv(char *oldpath, char *newpath);
	static long parseTime(const char *format, const char *buf);
	static long parseDateTime(const char *format, char* buf);
	static string getWeek();
};

#endif
