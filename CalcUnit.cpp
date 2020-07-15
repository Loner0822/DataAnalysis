#include "CalcUnit.h"	

CalcUnit::CalcUnit() {
	this->Calc_Id = 0;
	this->Const_str = "";
	this->Const_Nums.clear();
}

std::vector<int> Get_Bit_From_Input(int value, int &pos, int &len) {
	std::vector<int> res;
	res.clear();
	while (value) {
		res.push_back(value & 1);
		value >>= 1;
	}
	while (pos + 1 < len)
		pos += 8;
	while (res.size() <= pos) {
		res.push_back(0);
	}
	return res;
}

double CalcUnit::Calculate_Result(double input, const int &bit_len) {
	double res = 0;
	std::vector<int> bit_vec;
	bool have_val;
	int num_len, num_val, pos, e;
	double const_v, x, u, r, val1, val2;
	switch (Calc_Id) {
	case 7:				// 补码
		pos = Const_Nums[0], num_len = Const_Nums[1];
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		if (bit_vec[pos] == 0) {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + bit_vec[i];
			}
		}
		else {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + (1 - bit_vec[i]);
			}
			res = - res - 1;
		}
		break;
	case 8:				// 多项式计算
		num_len = int(Const_Nums[1] + 0.5);
		for (int i = Const_Nums.size() - 1; i >= Const_Nums.size() - num_len; --i) {
			res = res * input + Const_Nums[i];
		}
		break;
	case 9:				// 缓存
		res = input;
		break;
	case 10:			// 提取所需位 十进制显示
		pos = Const_Nums[0], num_len = Const_Nums[1];
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		for (int i = pos; i > pos - num_len; --i) {
			res = res * 2 + bit_vec[i];
		}
		break;			// A[0]*input+A[1]
	case 11:
		res = Const_Nums[0] * input + Const_Nums[1];
		break;
	case 12:			// 补码
		pos = bit_len - 1, num_len = bit_len;
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		if (bit_vec[pos] == 0) {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + bit_vec[i];
			}
		}
		else {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + (1 - bit_vec[i]);
			}
			res = -res - 1;
		}
		break;
	case 13:			// IEEE 浮点 32位
		pos = bit_len - 1, num_len = bit_len;
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		e = 0;
		for (int i = pos - 1; i >= pos - 8; --i) {
			e = e * 2 + bit_vec[i];
		}
		have_val = 0;
		for (int i = 0; i <= pos - 9; ++i) {
			res = (res + bit_vec[i]) / 2.0;
			if (bit_vec[i])
				have_val = 1;
		}
		switch (e)
		{
		case 0:
			res = res * pow(2, -128);
			break;
		case 255:
			if (have_val) {
				res = NAN;
			}
			else {
				res = INFINITY;
			}
			break;
		default:
			e -= 127;
			res = (res + 1) * pow(2, e);
			break;
		}
		if (bit_vec[pos] != 0) {
			res = -res;
		}
		break;
	case 14:			// 补码 无符号
		pos = bit_len - 1, num_len = bit_len;
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		for (int i = pos; i > pos - num_len; --i) {
			res = res * 2 + bit_vec[i];
		}
		break;
	case 20:			// 补码 非符号位/2^(15) 再平方
		pos = bit_len - 1, num_len = bit_len;
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		if (bit_vec[pos] == 0) {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + bit_vec[i];
			}
			res = pow(res * pow(2, -15), 2);
		}
		else {
			for (int i = pos - 1; i > pos - num_len; --i) {
				res = res * 2 + (1 - bit_vec[i]);
			}
			res = -res - 1;
			res = -pow((-res) * pow(2, -15), 2);
		}

		break;
	case 22:			// 积日 积秒
		num_val = int(input + 0.5);
		pos = int(Const_Nums[0] + 0.5);
		if (pos == 1) {
			res = int(num_val / (3600 * 24));
		}
		else {
			res = num_val % (3600 * 24);
		}
		break;
	case 23:			// input*A[0]^A[1]
		res = input * pow(Const_Nums[0], Const_Nums[1]);
		break;
	case 24:			// 不用
		res = input;
		break;
	case 26:			// A[0]*ln(input) + A[1]
		res = Const_Nums[0] * log(input) + Const_Nums[1];
		break;
	case 28:			// 多项式计算	
		input = input * 3.3 / 255.0;
		for (int i = 0; i < Const_Nums.size(); ++i) {
			res = res * input + Const_Nums[i];
		}
		break;
	case 29:			// val1:A[0]~A[1]位 val2:A[2]~A[3]位 val1/10^(val2-A[4])
		pos = Const_Nums[0], num_len = Const_Nums[1];
		val1 = val2 = 0.0;
		bit_vec = Get_Bit_From_Input(int(input + 0.5), pos, num_len);
		for (int i = pos; i > pos - num_len; --i) {
			val1 = val1 * 2 + bit_vec[i];
		}
		pos = Const_Nums[2], num_len = Const_Nums[3];
		for (int i = pos; i > pos - num_len; --i) {
			val2 = val2 * 2 + bit_vec[i];
		}
		res = val1 / pow(10, val2 - Const_Nums[4]);
		break;
	case 50:			// 附录四计算
		const_v = Const_Nums[Const_Nums.size() - 1];
		u = (input + 1) * 5 / 4096;
		x = u * 10000 / (const_v - u);
		res = (-172843.44828) / (-4622.53337 + sqrt(21367814.75677 - (-345686.89656 * (-6.01188 - log(x))))) - 273.15;
		break;
	case 51:			// 附录四计算(带参数)
		const_v = Const_Nums[Const_Nums.size() - 1];
		u = (input + 1) * 5 / 4096;
		x = u * 10000 / (const_v - u);
		res = (-Const_Nums[2]) / (-Const_Nums[3] + sqrt(Const_Nums[4] - (-Const_Nums[5] * (-Const_Nums[6]-log(x))))) - Const_Nums[7];
		break;
	case 52:			// 不用
		res = input;
		break;
	case 53:			// 多种公式
		switch (int(Const_Nums[0] + 0.5))
		{
		case 1:
			r = ((2.55 * input) / (4096.0 - input)) * 1000;
			res = -0.002174 * pow(r, 2) + 2.135 * r - 288.8;
			break;
		case 3:
			r = ((5.1 * input) / (4096.0 - input)) * 1000;
			res = -0.002174 * pow(r, 2) + 2.135 * r - 288.8;
			break;
		case 4:
			r = ((5.1 * input) / (4096.0 - input)) * 1000;
			res = 515.8 * pow(r, -0.1353) - 137.9;
			break;
		case 5:
			r = ((10 * input) / (4096.0 - input)) * 1000;
			res = 703.4 * pow(r, -0.1353) - 137.9;
			break;
		case 10:
			u = 5.0 * input / 256;
			r = log(5100 * u / (5 - u));
			res = 2 * 1195.569 / (sqrt(2998.869 * 2998.869 - 4 * 1195.569 * (-3.15209 - r)) - 2998.869) - 273.15;
			break;
		default:
			res = input;
			break;
		}
		break;
	default:
		res = input;
		break;
	}
	return res;
}