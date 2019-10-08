#include "GrammaticalParser.h"
/* 支持函数 */
void _error(const char* s);			// 错误输出处理
Token _gettoken_index(int index);	// 根据索引获取一个单词
void _gettoken();					// 获取一个单词
void _retract();					// 回退一个单词

/* 初始化函数 */
GrammaticalParser::GrammaticalParser(vector<Token>& token_list, vector<string>& output)
	:token_list(token_list), output_list(output){
	token = NULL;
	ptoken = 0;
}

/* 支持函数 */
void GrammaticalParser::_error(const char * s) {
	printf("Face an error when parsing grammar %s.\n", s);
}

Token GrammaticalParser::_gettoken_index(int index)
{
	return token_list[index];
}
void GrammaticalParser::_gettoken()
{
	ptoken++;
	token = &(token_list[ptoken]);
}
void GrammaticalParser::_retract()
{
	ptoken--;
	token = &(token_list[ptoken]);
}