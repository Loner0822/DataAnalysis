#include <string>
#include <vector>

class CalcUnit {
public:
	int Calc_Id;
	std::string Const_str;
	std::vector <double> Const_Nums;

	CalcUnit();

	double Calculate_Input(std::string hex_input) {
		
	}

	double Calculate_Result(double input);
};
