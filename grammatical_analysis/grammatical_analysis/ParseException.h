#ifndef __PARSE_EXCEPTION_H__
#define __PARSE_EXCEPTION_H__

#include<exception>		
#include<iostream>

using namespace std;

// 解析错误类型
enum ParseExceptionType
{
	Unknown,
	WrongToken,							// 错误的句柄
	UnknownBranch,						// 根据FISRT集合无法预估的分支
	Unexpected,							// 不可能到达的文法
};

class ParseException : public exception
{
public:
	ParseExceptionType errorType;
	string des;

	ParseException(ParseExceptionType errorType, string description);
	const char* what() const throw();
};



#endif // !__ERROR_H__

