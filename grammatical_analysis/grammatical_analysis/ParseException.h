#ifndef __PARSE_EXCEPTION_H__
#define __PARSE_EXCEPTION_H__

#include<exception>		
#include<iostream>

using namespace std;

// ������������
enum ParseExceptionType
{
	Unknown,
	WrongToken,							// ����ľ��
	UnknownBranch,						// ����FISRT�����޷�Ԥ���ķ�֧
	Unexpected,							// �����ܵ�����ķ�
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

