#ifndef __MIPS_CODE_UTILS_H__
#define __MIPS_CODE_UTILS_H__

#include "Block.h"
#include "Quaternary.h"
class MipsFunction;

void mips_string(vector<string>* list, string str);

// # comment
/* None Instr Mips Code */
void mips_comment(vector<string>* list, string comment);
// label: .space size 
void mips_space_alloc(vector<string>* list, string lable, int size);
// label: .asciiz strcon
void mips_asciiz(vector<string>* list, string label, string strcon);
// label: 
void mips_label(vector<string>* list, string label);

/* Memory */
// addi $sp, $sp, size
void mips_sp_move(vector<string>* list, int offset);
// lw target, offset(sp) | lw target, glb_data_label | la target, string_label | li target, const
void mips_load(vector<string>* list, string target, SymbolItem* item, MipsFunction* func);
// lw target, offset(sp)
void mips_load(vector<string>* list, string target, int offset);
// sw target, offset(sp) | sw target, label 
void mips_save(vector<string>* list, string target, SymbolItem* item, MipsFunction* func);
// sw target, offset(sp)
void mips_save(vector<string>* list, string target, int offset);

/* Branch */
// jr target
void mips_jr(vector<string>* list, string label);
void mips_jal(vector<string>* list, string label);
void mips_j(vector<string>* list, string label);
void mips_bnz(vector<string>* list, string lable, string condition);
void mips_bz(vector<string>* list, string lable, string condition);

/* Archimetic */
void mips_calc(vector<string>* list, string A, string B, string Result, QuaterType type);
void mips_cali(vector<string>* list, string A,int immediate, string Result, QuaterType type);
void mips_cmp(vector<string>* list, string A, string B, string Result, QuaterType type);

/* IO */
// scan : int|char
void mips_scan(vector<string>* list, string target, BasicType type);
// print : int|char|string
void mips_print(vector<string>* list, string source, BasicType type);

/* Array */
void mips_array_assign(vector<string>* list, string source, SymbolItem* arr, string index, MipsFunction* func);
void mips_array_query(vector<string>* list, string target, SymbolItem* arr, string index, MipsFunction* func);

/* ------------------------------------------ 非代码类 --------------------------------------- */
/* 给定四元式，求其属于的块 */
void quater_block_check(Quaternary* q);

/* 根据函数获取该函数下的所有四元式 */
vector<Quaternary*> get_quaternary_by_function(SymbolItem* func, vector<Quaternary*>* list);

/* 给定四元式，返回其所有的SymbolItem */
map<string, SymbolItem*> get_items_by_quater(Quaternary* q);


#endif // !_MIPS_CODE_UTILS_H__
