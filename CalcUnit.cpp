#include "CalcUnit.h"

CalcUnit::CalcUnit() {
	this->Calc_Id = 0;
	this->Const_str = "";
	this->Const_Nums.clear();
}

double Ca

double CalcUnit::Calculate_Result(double input) {
	double res = 0;
	std::vector<int> bit_vec;
	bit_vec.clear();
	switch (Calc_Id) {
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		int tmp = int(input + 0.5);
		while (tmp) {
			bit_vec.push_back(tmp & 1);
			tmp >>= 1;
		}
		
		break;
	case 11:
		break;
	case 12:
		break;
	case 13:
		break;
	case 14:
		break;
	case 20:
		break;
	case 22:
		break;
	case 23:
		break;
	case 24:
		break;
	case 26:
		break;
	case 28:
		break;
	case 29:
		break;
	case 50:
		break;
	case 51:
		break;
	case 52:
		break;
	case 53:
		break;
	default:
		res = input;
		break;
	}
	return res;
}