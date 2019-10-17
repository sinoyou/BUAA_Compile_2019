#include <cstdio>
#ifndef __DEBUG_H__
#define __DEBUG_H__

// ��ʾ�����ļ��������кŵ�debug���
#define DEBUG_PRINT(msg) fprintf(stdout, "line: %d, file : %s %s \n", __LINE__, __FILE__, msg); 

// ������GrammaticalParser�﷨���������
#define RECUR_PRINT(head, level, rule)	{						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += " ";				\
	cout << space << head << " " << rule << endl;				\
}

// ������GrammaticalParser�﷨���������
#define PARSE_PRINT(level, token, symbol){						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += " ";				\
	cout << space << " @ " << token << " " << symbol << endl;				\
}

#endif // !__DEBUG_H__
	
