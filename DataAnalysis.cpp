#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <iomanip>
//#include "windows.h"
#include "Frame.h"
#include "tinyxml2.h"
#include "IniReader.h" 
#include "DatabaseUnit.h"
#include "OperatorUnit.h"
#include "WebSocketpp.h"
#include "Calc_Func_DLL.h"
#define CALCFUNCDLL_API __declspec(dllexport)

#include <SimpleAmqpClient/SimpleAmqpClient.h>


using namespace AmqpClient;

const int MaxByte = 0x12f;
double CONST_5V_VOLT;

// 读取2进制文件(压缩成字符类型)
void Get_Byte_From_File(std::string& res) {
	res = "";
	std::ifstream fin("02星S.dat", std::ios::in | std::ios::binary);
	if (!fin.is_open()) {
		std::cout << "False" << std::endl;
		return;
	}
	fin.seekg(0, std::ios::end);
	int fsize = fin.tellg();

	fin.seekg(0, std::ios::beg);

	int ReadLen = MaxByte;
	char szin[MaxByte];
	memset(szin, 0, sizeof szin);

	if (fsize <= MaxByte)
		ReadLen = fsize;

	while (fin.read(szin, ReadLen)) {
		std::string tmp(szin, sizeof(szin) / MaxByte * ReadLen);
		res += tmp;
		fsize -= MaxByte;
		if (fsize <= MaxByte)
			ReadLen = fsize;
		if (fsize < 0)
			break;
	}
	fin.close();
}

// 2进制(压缩成字符类型)转16进制
std::string Bin_To_Hex(const std::string& strBin) {
	std::string strHex;
	strHex.resize(strBin.size() * 2);
	for (size_t i = 0; i < strBin.size(); i++) {
		uint8_t cTemp = strBin[i];
		for (size_t j = 0; j < 2; j++) {
			uint8_t cCur = (cTemp & 0x0f);
			if (cCur < 10)
				cCur += '0';
			else
				cCur += ('A' - 10);
			strHex[2 * i + 1 - j] = cCur;
			cTemp >>= 4;
		}
	}
	return strHex;
}

// 按帧存储数据包
void Save_Data_By_Frame(std::vector<Frame>& Frame_Data, const std::string& file_string) {
	int Frame_Num = file_string.size() / FrameLength;
	for (int i = 0; i < Frame_Num; ++i) {
		Frame tmp(file_string.substr(i * FrameLength, FrameLength));
		Frame_Data.push_back(tmp);
	}
}

// 检查是否为2位16进制字符
bool Check_Is_Hex(std::string str) {
	if (str.size() != 2)
		return false;
	for (int i = 0; i < 2; ++i) {
		str[i] = toupper(str[i]);
		if (!('0' <= str[i] && str[i] <= '9') && !('A' <= str[i] && str[i] <= 'F'))
			return false;
	}
	return true;
}

// 字符串分割
std::vector<std::string> My_Split(const std::string& s, const std::string& seperator) {
	std::vector<std::string> result;
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

// 获取今日日期
std::string Get_Now_Date_String(int type = 0, time_t now = time(0)) {
	tm ltm;
	localtime_s(&ltm, &now);
	std::string year, month, day;
	year = std::to_string(1900 + ltm.tm_year);
	while (year.size() < 4) {
		year = "0" + year;
	}
	month = std::to_string(1 + ltm.tm_mon);
	while (month.size() < 2) {
		month = "0" + month;
	}
	day = std::to_string(ltm.tm_mday);
	while (day.size() < 2) {
		day = "0" + day;
	}
	if (type == 0)
		return year + month + day;
	else
		return year + "." + month + "." + day;
}

// 获取当前时间
std::string Get_Now_Time_String(int type = 0, time_t now = time(0)) {
	tm ltm;
	localtime_s(&ltm, &now);
	std::string year, month, day;
	year = std::to_string(1900 + ltm.tm_year);
	while (year.size() < 4) {
		year = "0" + year;
	}
	month = std::to_string(1 + ltm.tm_mon);
	while (month.size() < 2) {
		month = "0" + month;
	}
	day = std::to_string(ltm.tm_mday);
	while (day.size() < 2) {
		day = "0" + day;
	}

	std::string hour, minute, second;
	hour = std::to_string(ltm.tm_hour);
	while (hour.size() < 2) {
		hour = "0" + hour;
	}
	minute = std::to_string(ltm.tm_min);
	while (minute.size() < 2) {
		minute = "0" + minute;
	}
	second = std::to_string(ltm.tm_sec);
	while (second.size() < 2) {
		second = "0" + second;
	}
	if (type == 0)
		return year + "." + month + "." + day + " " + hour + ":" + minute + ":" + second;
	else
		return hour + ":" + minute + ":" + second;
}

// 按csv文件建立Para_Method_Map
void Build_Para_Method_Map(std::map<std::string, std::string>& res_map, std::string& str) {
	std::string::iterator it;
	for (it = str.begin(); it < str.end(); ++it) {
		if (*it == '\n') {
			str.erase(it);
			it--;
		}
		else if (*it == '\r') {
			str.erase(it);
			it--;
		}
		else if (*it == '\t') {
			str.erase(it);
			it--;
		}
		else if (*it == ',') {
			str.erase(it);
			it--;
		}
	}
	std::vector<std::string> para_index_method = My_Split(str, "\"");
	for (int i = 0; i < (int)para_index_method.size(); i += 2) {
		res_map[para_index_method[i]] = para_index_method[i + 1];
	}
}

// 按csv文件建立Star_Map
void Build_Star_Map(std::map<std::string, std::string>& res_map, std::string& str) {
	std::string::iterator it;
	for (it = str.begin(); it < str.end(); ++it) {
		if (*it == '\n') {
			str.erase(it);
			it--;
		}
		else if (*it == '\r') {
			str.erase(it);
			it--;
		}
		else if (*it == '\t') {
			str.erase(it);
			it--;
		}
		else if (*it == ',') {
			str.erase(it);
			it--;
		}
	}
	std::vector<std::string> star_msg = My_Split(str, "\"");
	for (int i = 0; i < (int)star_msg.size(); i += 2) {
		res_map[star_msg[i]] = star_msg[i + 1];
	}
}

// 常数字符串转换成数组 
std::vector<double> Const_Str_To_Vector(std::string str) {
	std::vector<double> res;
	std::vector<std::string> res_str;
	res_str = My_Split(str, ",");
	double tmp;
	for (int i = 0; i < (int)res_str.size(); ++i) {
		tmp = atof(res_str[i].c_str());
		res.push_back(tmp);
	}
	return res;
}

// 按XML中FormatDesc建map
void Build_XML_FormatDesc_Map(std::map<std::string, std::string>& res_map, tinyxml2::XMLElement* root) {
	std::string ID, Condition;
	tinyxml2::XMLElement* surface = root->FirstChildElement("FormatDesc"), * item;
	for (item = surface->FirstChildElement(); item != NULL; item = item->NextSiblingElement()) {
		ID = item->FindAttribute("id")->Value();
		Condition = item->FindAttribute("condition")->Value();
		Condition = Condition.substr(Condition.length() - 3, 2);
		if (Check_Is_Hex(Condition) && res_map.count(Condition) == 0) {
			res_map[Condition] = ID;
		}
	}
}

// 按XML中Format建map
void Build_XML_Format_Map(std::map<std::string, std::vector<std::pair<int, std::string>>>& res_map, tinyxml2::XMLElement* root) {
	tinyxml2::XMLElement* surface = root->FirstChildElement("FormatDesc"), * item, * tmp;
	std::string Elem_Name, Elem_ID;
	std::string ParaIndex, VirtualBand;
	std::vector<std::pair<int, std::string>> vec;
	for (item = surface->NextSiblingElement(); item != NULL; item = item->NextSiblingElement()) {
		vec.clear();
		Elem_Name = item->Name();
		if (Elem_Name != "Format")
			break;
		Elem_ID = item->FindAttribute("ID")->Value();
		for (tmp = item->FirstChildElement(); tmp != NULL; tmp = tmp->NextSiblingElement()) {
			ParaIndex = tmp->FindAttribute("paraIndex")->Value();
			VirtualBand = tmp->FindAttribute("virtualBand")->Value();
			vec.push_back(std::make_pair(std::stoi(ParaIndex), VirtualBand));
		}
		res_map[Elem_ID] = vec;
	}
}

// 按XML中Calculate建map
void Build_XML_Para_Calc_Map(std::map<std::string, std::vector<CalcUnit>>& res_map, tinyxml2::XMLElement* root) {
	tinyxml2::XMLElement* surface = root->FirstChildElement("Calculate"), * item, * tmp;
	std::string para_index;
	CalcUnit elem;
	std::vector<int> check_num;
	std::vector<CalcUnit> vec;
	for (item = surface->FirstChildElement(); item != NULL; item = item->NextSiblingElement()) {
		vec.clear();
		para_index = item->Attribute("paraIndex");
		for (tmp = item->FirstChildElement(); tmp != NULL; tmp = tmp->NextSiblingElement()) {
			elem.Calc_Id = std::stoi(tmp->Attribute("id"));
			check_num.push_back(elem.Calc_Id);
			elem.Const_str = tmp->Attribute("const");
			elem.Const_Nums = Const_Str_To_Vector(elem.Const_str);
			vec.push_back(elem);
		}
		res_map[para_index] = vec;
	}
}

// VirtualBand转换为数据串
std::string VirtualBand_To_DataString(std::string virtual_band, const Package& pkg) {
	std::vector<int> res;
	res.clear();
	int tmp, num1, num2;
	if (virtual_band.find('-') != virtual_band.npos) {
		tmp = num1 = num2 = 0;
		while ('0' <= virtual_band[tmp] && virtual_band[tmp] <= '9')
			num1 = num1 * 10 + virtual_band[tmp] - '0', ++tmp;
		while (virtual_band[tmp] < '0' || virtual_band[tmp] > '9')
			++tmp;
		while ('0' <= virtual_band[tmp] && virtual_band[tmp] <= '9')
			num2 = num2 * 10 + virtual_band[tmp] - '0', ++tmp;
		for (int i = num1; i <= num2; ++i)
			res.push_back(i);
	}
	else if (virtual_band.find(',') != virtual_band.npos) {
		tmp = num1 = num2 = 0;
		while ('0' <= virtual_band[tmp] && virtual_band[tmp] <= '9')
			num1 = num1 * 10 + virtual_band[tmp] - '0', ++tmp;
		while (virtual_band[tmp] < '0' || virtual_band[tmp] > '9')
			++tmp;
		while ('0' <= virtual_band[tmp] && virtual_band[tmp] <= '9')
			num2 = num2 * 10 + virtual_band[tmp] - '0', ++tmp;
		res.push_back(num1);
		res.push_back(num2);
	}
	else {
		tmp = num1 = 0;
		while ('0' <= virtual_band[tmp] && virtual_band[tmp] <= '9')
			num1 = num1 * 10 + virtual_band[tmp] - '0', ++tmp;
		res.push_back(num1);
	}
	std::string pkg_data, res_str;
	pkg_data = pkg.Package_Leader + pkg.Package_Sub_Leader + pkg.Package_Data;
	for (int i = 0; i < (int)res.size(); ++i) {
		res_str += pkg_data.substr((res[i] - 1) * 2, 2);
	}
	return res_str;
}

// 计算结果
std::string Calculate_To_DataString(const std::string& input, std::vector<CalcUnit>& calc_list) {
	double res = std::stoi(input, nullptr, 16);
	for (int i = 0; i < (int)calc_list.size(); ++i) {
		if (calc_list[i].Calc_Id == 50 || calc_list[i].Calc_Id == 51) {
			calc_list[i].Const_Nums.push_back(CONST_5V_VOLT);
		}
		res = calc_list[i].Calculate_Result(res, input.size() * 8);
	}
	return std::to_string(res);
}

// 处理显示结果
std::string Calculate_To_DataShow(std::string& res_str, const OperatorUnit& op, const std::string& code) {
	std::string show_str = "";
	std::vector<std::string> vec_type = My_Split(op.Type, ",");
	std::vector<std::string> vec_method;
	std::vector<int> vec_digit;
	int type_int = std::stoi(vec_type[0]);
	int num_zero = 6;
	if (vec_type.size() > 1) {
		num_zero = std::stoi(vec_type[1]);
	}
	std::ostringstream num_out;
	double res_double = std::stof(res_str);
	int res_int = int(res_double + 0.5);
	// 时间 2000.1.1 12:00 开始计时 10957 * 3600 * 24 + 12 * 3600 为 1970.1.1 00:00 到 2000.1.1 00:00 秒数
	time_t t = res_int + 10957 * 3600 * 24 + 12 * 3600;
	switch (type_int)
	{
	case 0:
		res_str = std::to_string(res_int);
		if (op.Para_Method != "") {
			vec_method = My_Split(op.Para_Method, ":;");
			for (int i = 0; i < (int)vec_method.size(); i += 2) {
				if (std::stoi(vec_method[i]) == res_int) {
					return vec_method[i + 1];
				}
			}
			return res_str + u8"(暂无翻译)";
		}
		else {
			return res_str + u8"(暂无翻译)";
		}
	case 1:
		res_str = std::to_string(res_int);
		return res_str;
	case 2:
		res_str = std::to_string(res_int);
		while (res_int) {
			vec_digit.push_back(res_int & 15);
			res_int >>= 4;
		}
		for (int i = 0; i < (int)vec_digit.size(); ++i) {
			if (0 <= vec_digit[i] && vec_digit[i] <= 9) {
				show_str = std::to_string(vec_digit[i]) + show_str;
			}
			else {
				show_str = std::string(1, vec_digit[i] - 10 + 'A') + show_str;
			}
		}
		while (show_str.size() < code.size())
			show_str = "0" + show_str;
		return show_str;
	case 3:
		res_str = std::to_string(res_int);
		while (res_int) {
			vec_digit.push_back(res_int & 1);
			res_int >>= 1;
		}
		for (int i = 0; i < (int)vec_digit.size(); ++i) {
			show_str = std::to_string(vec_digit[i]) + show_str;
		}
		while (show_str.size() < code.size() * 4)
			show_str = "0" + show_str;
		return show_str;
	case 4:
		if (vec_type.size() > 1) {
			num_out << std::setprecision(num_zero) << res_double;
			res_str = num_out.str();
		}
		return res_str;
	case 5:
		return res_str;
	case 6:
		res_str = std::to_string(res_int);
		return Get_Now_Time_String(0, t);
	case 7:
		res_str = std::to_string(res_int);
		return Get_Now_Date_String(1, t);
	case 8:
		res_str = std::to_string(res_int);
		return Get_Now_Time_String(1, t);
	default:
		return res_str;
	}
	return res_str;
}

int main() {
	std::string inifile = "Config.ini";
	std::string filename = "";
	INIReader reader(inifile);
	if (reader.ParseError() != 0) {
		std::cout << "Can't load '" + inifile + "'" << std::endl;
	}


	// 读取CSV 获取Para_Method
	filename = reader.Get("File", "Translate_Method", "");
	std::ifstream fin(filename, std::ios::in);
	std::string line_str, file_str;
	while (std::getline(fin, line_str)) {
		file_str += line_str;
	}

	std::map<std::string, std::string> Para_Method_Map;
	Build_Para_Method_Map(Para_Method_Map, file_str);
	file_str.clear();
	fin.close();

	// 读取CSV 获取卫星信息
	filename = reader.Get("File", "Star_Num", "");
	fin.open(filename, std::ios::in);
	while (std::getline(fin, line_str)) {
		file_str += line_str;
	}

	std::map<std::string, std::string> Star_Map;
	Build_Star_Map(Star_Map, file_str);

	// 连接MySQL
	std::string host = reader.Get("Database", "Host", "localhost");
	std::string user = reader.Get("Database", "User", "root");
	std::string password = reader.Get("Database", "Password", "");
	std::string dbname = reader.Get("Database", "DBName", "");
	int port = reader.GetInteger("Database", "Port", 3306);
	DatabaseUnit DB_Para(host, user, password, dbname, port);
	if (!DB_Para.Connect_Database()) {
		return 0;
	}

	// 读取数据库
	std::map<std::string, OperatorUnit> Para_Data;
	std::string str_sql = "select sys_name, param_code, param_name, my_type, param_index from t_tm_parameters";
	DB_Para.Query_Database(str_sql);
	MYSQL_ROW row;
	OperatorUnit tmp_opunit;
	while (row = mysql_fetch_row(DB_Para.Res)) {
		tmp_opunit.Sys_Name = row[0];
		tmp_opunit.Para_Code = row[1];
		tmp_opunit.Para_Name = row[2];
		tmp_opunit.Type = row[3];
		tmp_opunit.Para_Index = row[4];
		tmp_opunit.Para_Method = Para_Method_Map[row[1]];
		Para_Data[tmp_opunit.Para_Index] = tmp_opunit;
	}
	mysql_free_result(DB_Para.Res);

	// 读取&存储XML解析文件
	tinyxml2::XMLDocument TelProcess;
	filename = reader.Get("File", "Param_Link", "");
	TelProcess.LoadFile(filename.c_str());
	tinyxml2::XMLElement* root = TelProcess.RootElement();

	std::map<std::string, std::string> FormatDesc_Map;
	Build_XML_FormatDesc_Map(FormatDesc_Map, root);

	std::map<std::string, std::vector<std::pair<int, std::string>>> Format_Map;
	Build_XML_Format_Map(Format_Map, root);

	std::map<std::string, std::vector<CalcUnit>> Para_Calc_Map;
	Build_XML_Para_Calc_Map(Para_Calc_Map, root);

	//// 读取包数据 按帧存储的数据包
	//std::string File_String;
	//Get_Byte_From_File(File_String);
	//File_String = Bin_To_Hex(File_String);
	//if (File_String.size() % FrameLength != 0) {
	//    std::cout << "Data Error" << std::endl;
	//    return 0;
	//}
	//std::vector<Frame> Frame_Data;
	//Frame_Data.clear();
	//Save_Data_By_Frame(Frame_Data, File_String);
	//File_String.clear();

	std::string Frame_String;
	Frame Frame_Data;
	std::string star_num;
	std::string Id_Code, Format_Code;
	std::string para_idx;
	std::string now_time;
	std::string pkg_id;
	std::string pkg_code;
	std::string pkg_res;
	std::string pkg_show;
	std::string queue_name = reader.Get("RabbitMQ", "Queuename", "");
	//std::string star_val;
	Package now;
	OperatorUnit tmp_op;
	std::vector <std::pair<int, std::string>> Para_Code;
	host = reader.Get("RabbitMQ", "Host", "localhost");
	port = reader.GetInteger("RabbitMQ", "Port", 5672);
	user = reader.Get("RabbitMQ", "Username", "");
	password = reader.Get("RabbitMQ", "Password", "");
	std::string vhost = reader.Get("RabbitMQ", "Vhost", "/");
	Channel::ptr_t channel = Channel::Create(host, port, user, password, vhost);
	channel->DeclareQueue(queue_name, false, true, false, false);
	std::string consumer_tag = channel->BasicConsume(queue_name);

	// 表名
	std::string now_date = Get_Now_Date_String();
	std::string table_name = "analysis_result_S_" + now_date;

	// websocket
	websocket_endpoint endpoint;
	std::string wsUrl = reader.Get("WebSocket", "Url", "ws://localhost");
	int ws_id = endpoint.connect(wsUrl);
	connection_metadata::ptr metedata = endpoint.get_metadata(ws_id);
	std::string send_msg;

	while (1) {
		Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
		Frame_String = envelope->Message()->Body();
		if (Frame_String == "exit")
			break;

		Frame_String = Bin_To_Hex(Frame_String);
		Frame_Data = Frame(Frame_String);
		star_num = Frame_Data.Frame_Leader.substr(0, 3);

		if (Star_Map.count(star_num) > 0) {
			star_num = Star_Map[star_num];
		}
		else {
			star_num = u8"未识别";
		}
		str_sql = "create table if not exists " + table_name + "_" + star_num + " like analysis_result_S";
		DB_Para.CreateTable_Database(str_sql);

		for (int j = 0; j < 3; ++j) {
			now = Frame_Data.Data[j];
			Id_Code = now.Package_Leader.substr(2, 2);
			Format_Code = FormatDesc_Map[Id_Code];
			if (Format_Map.count(Format_Code))
				Para_Code = Format_Map[Format_Code];
			else
				continue;
			for (int k = 0; k < (int)Para_Code.size(); ++k) {
				para_idx = std::to_string(Para_Code[k].first);
				tmp_op = Para_Data[para_idx];
				now_time = Get_Now_Time_String();
				pkg_id = tmp_op.Sys_Name.substr(0, 4);
				pkg_code = VirtualBand_To_DataString(Para_Code[k].second, now);
				pkg_res = Calculate_To_DataString(pkg_code, Para_Calc_Map[para_idx]);
				pkg_show = Calculate_To_DataShow(pkg_res, tmp_op, pkg_code);
				if (para_idx == "1101") {
					CONST_5V_VOLT = std::stof(pkg_res);
				}
				str_sql = "insert into " + table_name + "_" + star_num + " (Create_time, Star_num, Package_id, Parameter_index, Parameter_name, Code, Value, Value_show) values ('";
				str_sql += now_time + "', '";
				str_sql += star_num + "', '";
				str_sql += pkg_id + "', '";
				str_sql += para_idx + "', '";
				str_sql += tmp_op.Para_Name + "', '";
				str_sql += pkg_code + "', '";
				str_sql += pkg_res + "', '";
				str_sql += pkg_show + "');";
				//std::cout << str_sql << std::endl;
				send_msg = str_sql;
				endpoint.send(ws_id, send_msg);
				DB_Para.Insert_Database(str_sql);
			}
		}
	}
	return 0;
}
