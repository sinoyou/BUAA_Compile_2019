#include <cstdio>
#ifndef __DEBUG_H__
#define __DEBUG_H__

// 显示代码文件名称与行号的debug输出
#define DEBUG_PRINT(msg) fprintf(stdout, "line: %d, file : %s %s \n", __LINE__, __FILE__, msg); 

// 适用于GrammaticalParser语法解析的输出
#define RECUR_PRINT(head, level, rule)	{						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += " ";				\
	cout << space << head << " " << rule << endl;				\
}

// 适用于GrammaticalParser语法解析的输出
#define PARSE_PRINT(level, token, symbol){						\
	string space = "";											\
	for (int i = 0; i < level; i++) space += " ";				\
	cout << space << " @ " << token << " " << symbol << endl;				\
}

#endif // !__DEBUG_H__
	
