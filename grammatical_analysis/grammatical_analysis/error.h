#ifndef __ERROR_H__
#define __ERROR_H__

// ö�ٱ���
enum ErrorType {
	LexError,
	Redefine,
	Undefine,
	FuncParamNumUnmatch,
	FuncParamTypeUnmatch,
	ConditionError,
	VoidWithReturn,
	ReturnError,
	ArrayIndexWrong,
	ConstModify,
	MissSemicn,
	MissRightParent,
	MissRightBrack,
	MissWhile,
	ConstDefWrong
};

#endif // !1
