/*
 *
 * Author: whm
 ***********************************************************/

#ifndef _STR_EXCEPTION_H
#define _STR_EXCEPTION_H

#include <string>

using namespace std;

class StrException {
private:
	string err_msg;
	int err_code;
public:
	StrException(const int code, const char *format, ...) {
		char msg[4096];
		va_list args;
		va_start(args, format);
		vsprintf(msg, format, args);
		va_end(args);

		err_msg = msg;
		err_code = code;
	}

	string getMsg() {
		return err_msg;
	}

	int getCode() {
		return err_code;
	}

};

#endif
