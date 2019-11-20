#include "Quaternary.h"
#include "debug.h"
#include <stdio.h>
# define MAXBUF 200
static char buf[MAXBUF];

vector<string> PrintQuater(vector<Quaternary*>* QuaterList) {
	vector<string> PutList;
	for (auto it = QuaterList->begin(); it != QuaterList->end(); it++) {
		QuaterType qtype = (*it)->type;
		if (qtype == QuaterType::FuncDeclar)
			PutList.push_back("");
		PutList.push_back(PrintQuaterHandler(*it, qtype));
	}
	return PutList;
}

string PrintQuaterHandler(Quaternary* q, QuaterType qtype)
{
	string sentence;
	switch (qtype)
	{
	case Undefined:
		DEBUG_PRINT("[Error] Undefined QuaterType");
		break;
	case VarDeclar:sentence = PutVarDeclarQuater(q); break;
	case ConstDeclar:sentence = PutConstDeclarQuater(q); break;
	case FuncDeclar:sentence = PutFuncDeclarQuater(q); break;
	case FuncParaDeclar:sentence = PutFuncParaDeclarQuater(q); break;
	case FuncParaPush:sentence = PutFuncParaPushQuater(q); break;
	case FuncCall:sentence = PutFuncCallQuater(q); break;
	case AssignRet:sentence = PutAssignRetQuater(q); break;
	case FuncRet:sentence = PutFuncRetQuater(q); break;
	case Add:sentence = PutCalQuater(q); break;
	case Sub:sentence = PutCalQuater(q); break;
	case Mult:sentence = PutCalQuater(q); break;
	case Div:sentence = PutCalQuater(q); break;
	case Assign:sentence = PutAssignQuater(q); break;
	case EqlCmp:sentence = PutCmpQuater(q); break;
	case NeqCmp:sentence = PutCmpQuater(q); break;
	case GtCmp:sentence = PutCmpQuater(q); break;
	case GeqCmp:sentence = PutCmpQuater(q); break;
	case LtCmp:sentence = PutCmpQuater(q); break;
	case LeqCmp:sentence = PutCmpQuater(q); break;
	case Goto:sentence = PutGotoQuater(q); break;
	case Bnz:sentence = PutBnzQuater(q); break;
	case Bz:sentence = PutBzQuater(q); break;
	case SetLabel:sentence = PutSetLabelQuater(q); break;
	case ArrayQuery:sentence = PutArrayQueryQuater(q); break;
	case AssignArray:sentence = PutAssignArrayQuater(q); break;
	case Scan:sentence = PutScanQuater(q); break;
	case Print:sentence = PutPrintQuater(q); break;
	default:
		DEBUG_PRINT("[Error] Unkown QuaterType");
		break;
	}
	return sentence;
}

/*
 * VarDeclar
 * para int|char idenfr
*/
string PutVarDeclarQuater(Quaternary* Q) {
	string name = Q->Result->getname(false);
	string type = Q->Result->get_basictype();
	if (Q->Result->isArray) {
		int size = Q->Result->array_size;
		sprintf_s(buf, MAXBUF, "var %s %s[%d]", type.c_str(), name.c_str(), size);
	}
	else {
		sprintf_s(buf, MAXBUF, "var %s %s", type.c_str(), name.c_str());
	}
	return string(buf);
}

/*
 * ConstDeclar
 * const int|char idenfr = value
*/
string PutConstDeclarQuater(Quaternary* Q) {
	string name = Q->Result->getname(false);
	string type = Q->Result->get_basictype();
	int value = Q->Result->value;
	if (type == "int") {
		sprintf_s(buf, MAXBUF, "const int %s = %d", name.c_str(), value);
	}
	else {
		sprintf_s(buf, MAXBUF, "const char %s = '%c'", name.c_str(), value);
	}
	return string(buf);
}

/*
 * FuncDeclar
 * void|int|char func_name()
*/
string PutFuncDeclarQuater(Quaternary* Q) {
	string name = Q->Result->getname(false);
	string type = Q->Result->get_basictype();
	sprintf_s(buf, MAXBUF, "%s %s()", type.c_str(), name.c_str());
	return string(buf);
}

/*
 * FuncParaDeclar
 * para int|char idenfr
*/
string PutFuncParaDeclarQuater(Quaternary* Q) {
	string name = Q->Result->getname(false);
	string type = Q->Result->get_basictype();
	sprintf_s(buf, MAXBUF, "para %s %s", type.c_str(), name.c_str());
	return string(buf);
}

/*
 * FuncParaPush
 * push idenfr/temp
*/
string PutFuncParaPushQuater(Quaternary* Q) {
	string name = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "push %s", name.c_str());
	return string(buf);
}

/*
 * FuncCall
 * call func_head
*/
string PutFuncCallQuater(Quaternary* Q) {
	string name = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "call %s", name.c_str());
	return string(buf);
}

/*
 * AssignRet
 * idenfr = RET
*/
string PutAssignRetQuater(Quaternary* Q) {
	string name = Q->Result->getname();
	sprintf_s(buf, MAXBUF, "%s = RET", name.c_str());
	return string(buf);
}

/*
 * FuncRet
 * ret (idenfr/temp)
*/
string PutFuncRetQuater(Quaternary* Q) {
	string name = "";
	if (Q->OpA != NULL)
		name = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "ret %s", name.c_str());
	return string(buf);
}

/*
 * Cal
 * idenfr/temp = idenfr/temp CalOp idenfr/temp
*/
string PutCalQuater(Quaternary* Q) {
	string A = (Q->OpA == NULL) ? "" : Q->OpA->getname();
	string B = Q->OpB->getname();
	string result = Q->Result->getname();
	QuaterType type = Q->type;
	string CalOp = (type == QuaterType::Add) ? "+" :
		(type == QuaterType::Sub) ? "-" :
		(type == QuaterType::Mult) ? "*" :
		(type == QuaterType::Div) ? "/" : "unknown";
	sprintf_s(buf, MAXBUF, "%s = %s %s %s", result.c_str(), A.c_str(),CalOp.c_str() ,B.c_str());
	return string(buf);
}

/*
 * Assign
 * idenfr/temp = idenfr/temp
*/
string PutAssignQuater(Quaternary* Q) {
	string A = Q->OpA->getname();
	string result = Q->Result->getname();
	sprintf_s(buf, MAXBUF, "%s = %s", result.c_str(), A.c_str());
	return string(buf);
}

/*
 * AssignArray
 * idenfr[index] = idenfr/temp
*/
string PutAssignArrayQuater(Quaternary* Q) {
	string arr = Q->Result->getname();
	string index = Q->OpB->getname();
	string value = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "%s[%s] = %s", arr.c_str(), index.c_str(), value.c_str());
	return string(buf);
}

/*
 * EqlCMp
 * idenfr/temp = idenfr/temp CmoOP idenfr/temp
*/
string PutCmpQuater(Quaternary* Q) {
	string A = Q->OpA->getname();
	string B = Q->OpB->getname();
	string result = Q->Result->getname();
	QuaterType type = Q->type;
	string CmpOp = (type == QuaterType::EqlCmp) ? "==" :
		(type == QuaterType::NeqCmp) ? "!=" :
		(type == QuaterType::GtCmp) ? ">" :
		(type == QuaterType::GeqCmp) ? ">=" :
		(type == QuaterType::LtCmp) ? "<" :
		(type == QuaterType::LeqCmp) ? "<=" : "Unknown";
	sprintf_s(buf, MAXBUF, "%s = %s %s %s", result.c_str(), A.c_str(), CmpOp.c_str(), B.c_str());
	return string(buf);
}

/*
 * Goto
 * GOTO label
*/
string PutGotoQuater(Quaternary* Q) {
	string label = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "GOTO %s", label.c_str());
	return string(buf);
}

/*
 * Bnz
 * BNZ label, condition
*/
string PutBnzQuater(Quaternary* Q) {
	string label = Q->OpA->getname();
	string condition = Q->OpB->getname();
	sprintf_s(buf, MAXBUF, "BNZ %s %s", label.c_str(), condition.c_str());
	return string(buf);
}

/*
 * Bz
 * BZ label, condition
*/
string PutBzQuater(Quaternary* Q) {
	string label = Q->OpA->getname();
	string condition = Q->OpB->getname();
	sprintf_s(buf, MAXBUF, "BZ %s %s", label.c_str(), condition.c_str());
	return string(buf);
}

/*
 * SetLabel
 * label:
*/
string PutSetLabelQuater(Quaternary* Q) {
	string label = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "%s:", label.c_str());
	return string(buf);
}

/*
 * ArrayQuery
 * idenfr/temp = idenfr[index]
*/
string PutArrayQueryQuater(Quaternary* Q) {
	string arr = Q->OpA->getname();
	string index = Q->OpB->getname();
	string result = Q->Result->getname();
	sprintf_s(buf, MAXBUF, "%s = %s[%s]", result.c_str(), arr.c_str(), index.c_str());
	return string(buf);
}

/*
 * Scan
 * Scan idenfr
*/
string PutScanQuater(Quaternary* Q) {
	string idenfr = Q->Result->getname();
	sprintf_s(buf, MAXBUF, "SCAN %s", idenfr.c_str());
	return string(buf);
}

/*
 * Print
 * print v/c/t
*/
string PutPrintQuater(Quaternary* Q) {
	string name = Q->OpA->getname();
	sprintf_s(buf, MAXBUF, "PRINT %s", name.c_str());
	return string(buf);
}