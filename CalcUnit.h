#include <string>
#include <vector>
#include <cmath>
#include <ctime>

class CalcUnit {
public:
	int Calc_Id;
	std::string Const_str;
	std::vector <double> Const_Nums;

	CalcUnit();

	double Calculate_Result(double input, const int &bit_len);
};
