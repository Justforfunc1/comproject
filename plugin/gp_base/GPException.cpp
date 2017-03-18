/**********************************************************
 * \file GPException.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 
 ** \author Allen.L
**********************************************************/

#include <cstdlib>
#include "gp_base/GPException.h"

namespace GPBase {

GPException::GPException() :
		_file(0), _line(0), _desc(NULL) {
}

GPException::GPException(const GPChar* file, GPInt32 line) :
		_file(file), _line(line), _desc(NULL) {
}

GPException::GPException(const GPChar* file, GPInt32 line, const GPChar* desc) :
		_file(file), _line(line), _desc(desc) {
}

GPException::~GPException() throw () {
}

const GPChar* GPException::_name = "GPBase::GPException";

GPString GPException::name() const {
	return _name;
}

GPVOID GPException::print(std::ostream& out) const {
	if (_file && _line > 0) {
		out << _file << ':' << _line << ": ";
		if (_desc) {
			out << _desc << ": ";
		}
	}
	out << name();
}

const GPChar* GPException::what() const throw () {
	try {
		if (_str.empty()) {
			std::stringstream s;
			print(s);
			_str = s.str();
		}
		return _str.c_str();
	} catch (...) {
	}
	return "";
}

GPException* GPException::clone() const {
	return new GPException(*this);
}

GPVOID GPException::_throw() const {
	throw *this;
}

const GPChar* GPException::file() const {
	return _file;
}

GPInt32 GPException::line() const {
	return _line;
}

std::ostream& operator <<(std::ostream& out, const GPException& ex) {
	ex.print(out);
	return out;
}

NullHandleException::NullHandleException(const GPChar* file, GPInt32 line) :
		GPException(file, line) {
	abort();
}

NullHandleException::~NullHandleException() throw () {
}

const GPChar* NullHandleException::_name = "NullHandleException";

GPString NullHandleException::name() const {
	return _name;
}

GPException* NullHandleException::clone() const {
	return new NullHandleException(*this);
}

GPVOID NullHandleException::_throw() const {
	throw *this;
}

IllegalArgumentException::IllegalArgumentException(const GPChar* file, GPInt32 line) :
		GPException(file, line) {
}

IllegalArgumentException::IllegalArgumentException(const GPChar* file, GPInt32 line, const GPString& r) :
		GPException(file, line), _reason(r) {
}

IllegalArgumentException::~IllegalArgumentException() throw () {
}

const GPChar* IllegalArgumentException::_name = "IllegalArgumentException";

GPString IllegalArgumentException::name() const {
	return _name;
}

GPVOID IllegalArgumentException::print(std::ostream& out) const {
	GPException::print(out);
	out << ": " << _reason;
}

GPException* IllegalArgumentException::clone() const {
	return new IllegalArgumentException(*this);
}

GPVOID IllegalArgumentException::_throw() const {
	throw *this;
}

GPString IllegalArgumentException::reason() const {
	return _reason;
}

}

