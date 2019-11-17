#include "Quaternary.h"

Quaternary::Quaternary(QuaterType type,SymbolItem* A, SymbolItem* B, SymbolItem* Result) {
	this->type = type;
	this->OpA = A;
	this->OpB = B;
	this->Result = Result;
}