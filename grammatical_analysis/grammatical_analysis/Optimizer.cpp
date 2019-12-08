#include "Optimizer.h"
vector<Quaternary*> Optimizer::get_optimized_quaters() {
	return optimized_code;
}

void Optimizer::dumps() {
	ofstream f;
	f.open(optimizer_name + "_optimized.txt", ios::out | ios::trunc);
	vector<string> quater_out_list = PrintQuater(&optimized_code);
	for (auto it = quater_out_list.begin(); it != quater_out_list.end(); it++) {
		f << *it << endl;
	}
	f.close();
}