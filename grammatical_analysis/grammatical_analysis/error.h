#ifndef __ERROR_H__
#define __ERROR_H__

// ц╤╬ы╠Да©
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
