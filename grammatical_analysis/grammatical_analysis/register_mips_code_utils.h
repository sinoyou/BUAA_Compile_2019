#ifndef __REG_MIPS_CODE_UTILS_H__
#define __REG_MIPS_CODE_UTILS_H__

#include "Block.h"
#include "Quaternary.h"
class RegMipsFunction;

void reg_mips_string(vector<string>* list, string str);

// general
void reg_mips_move(vector<string>* list, string src, string tar);

// # comment
/* None Instr Mips Code */
void reg_mips_comment(vector<string>* list, string comment);
// label: .space size 
void reg_mips_space_alloc(vector<string>* list, string lable, int size);
// label: .asciiz strcon
void reg_mips_asciiz(vector<string>* list, string label, string strcon);
// label: 
void reg_mips_label(vector<string>* list, string label);

/* Memory */
// addi $sp, $sp, size
void reg_mips_sp_move(vector<string>* list, int offset);
// lw target, offset(sp) | lw target, glb_data_label | la target, string_label | li target, const
void reg_mips_load(vector<string>* list, string target, SymbolItem* item, RegMipsFunction* func);
// lw target, offset(sp)
void reg_mips_load(vector<string>* list, string target, int offset);
// sw target, offset(sp) | sw target, label 
void reg_mips_save(vector<string>* list, string target, SymbolItem* item, RegMipsFunction* func);
// sw target, offset(sp)
void reg_mips_save(vector<string>* list, string target, int offset);

/* Branch */
// jr target
void reg_mips_jr(vector<string>* list, string label);
void reg_mips_jal(vector<string>* list, string label);
void reg_mips_j(vector<string>* list, string label);
void reg_mips_bnz(vector<string>* list, string lable, string condition);
void reg_mips_bz(vector<string>* list, string lable, string condition);
void reg_mips_branch(vector<string>* list, string label, string cmpA, string cmpB, QuaterType cmp_type);

/* Archimetic */
void reg_mips_calc(vector<string>* list, string A, string B, string Result, QuaterType type);
void reg_mips_cali(vector<string>* list, string A, int immediate, string Result, QuaterType type);
void reg_mips_cmp(vector<string>* list, string A, string B, string Result, QuaterType type);
void reg_mips_li(vector<string>* list, int value, string Result);

/* IO */
// scan : int|char
void reg_mips_scan(vector<string>* list, string target, BasicType type);
// print : int|char|string
void reg_mips_print(vector<string>* list, string source, BasicType type);

/* Array */
void reg_mips_array_assign(vector<string>* list, string source, SymbolItem* arr, string index, string temp, RegMipsFunction* func);
void reg_mips_array_query(vector<string>* list, string target, SymbolItem* arr, string index, string temp, RegMipsFunction* func);


#endif // !_MIPS_CODE_UTILS_H__
