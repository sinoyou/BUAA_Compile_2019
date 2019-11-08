#include "ParseException.h"

/*
 * Initial Function
**/
ParseException::ParseException(ParseExceptionType errorType, string des){
	__super::exception("Parse Exception");
	this->errorType = errorType;
	this->des = des;
}

const char * ParseException::what() const throw() {
	return (des).c_str();
}