#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include<iomanip>
#include "Frame.h"
#include "tinyxml2.h"
#include "DatabaseUnit.h"
#include "OperatorUnit.h"
#include "CalcUnit.h"
#pragma comment(lib,"libmysql.lib")

const int MaxByte = 0xff;
double CONST_5V_VOLT;

// 读取2进制文件(压缩成字符类型)
void Get_Byte_From_File(std::string &res) {
    res = "";
    std::ifstream fin("01星S.dat", std::ios::in | std::ios::binary);
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
std::string Bin_To_Hex(const std::string &strBin) {
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
void Save_Data_By_Frame(std::vector<Frame> &Frame_Data, const std::string &file_string) {
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

// 按csv文件建立Para_Method Map
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
    for (int i = 0; i < para_index_method.size() / 2; ++i) {
        res_map[para_index_method[i * 2]] = para_index_method[i * 2 + 1];
    }
}

// 常数字符串转换成数组 
std::vector<double> Const_Str_To_Vector(std::string str) {
    std::vector<double> res;
    std::vector<std::string> res_str;
    res_str = My_Split(str, ",");
    double tmp;
    for (int i = 0; i < res_str.size(); ++i) {
        tmp = atof(res_str[i].c_str());
        res.push_back(tmp);
    }
    return res;
}

// 按XML中FormatDesc建map
void Build_XML_FormatDesc_Map(std::map<std::string, std::string> &res_map, tinyxml2::XMLElement *root) {
    std::string ID, Condition;
    tinyxml2::XMLElement* surface = root->FirstChildElement("FormatDesc"), *item;
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
void Build_XML_Format_Map(std::map<std::string, std::vector<std::pair<int, std::string>>> &res_map, tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* surface = root->FirstChildElement("FormatDesc"), *item, *tmp;
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
    tinyxml2::XMLElement* surface = root->FirstChildElement("Calculate"), *item, *tmp;
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
    for (int i = 0; i < res.size(); ++i) {
        res_str += pkg_data.substr((res[i] - 1) * 2, 2);
    }
    return res_str;
}

std::string Calculate_To_DataString(const std::string &input, std::vector<CalcUnit> &calc_list) {
    double res = std::stoi(input, nullptr, 16);
    for (int i = 0; i < calc_list.size(); ++i) {
        if (calc_list[i].Calc_Id == 50 || calc_list[i].Calc_Id == 51) {
            calc_list[i].Const_Nums.push_back(CONST_5V_VOLT);
        }
        res = calc_list[i].Calculate_Result(res);
    }
    return std::to_string(res);
}

int main() {

    // 读取CSV 获取Para_Method
    std::ifstream fin("t_tm_translate.csv", std::ios::in);
    std::string line_str, file_str;
    while (std::getline(fin, line_str)) {
        file_str += line_str;
    }
    
    std::map<std::string, std::string> Para_Method_Map;
    Build_Para_Method_Map(Para_Method_Map, file_str);
    file_str.clear();

    // 连接MySQL
    std::string host = "localhost";
    std::string user = "root";
    std::string password = "0822";
    std::string tablename = "workspace";
    int port = 3306;
    DatabaseUnit DB_Para(host, user, password, tablename, 3306);
    if (!DB_Para.Connect_Database()) {
        return 0;
    }

    // 存储数据库
    std::map<std::string, OperatorUnit> Para_Data;
    DB_Para.Query_Database("select sys_name, param_code, param_name, param_index from t_tm_parameters");
    MYSQL_ROW row;
    OperatorUnit tmp_opunit;
    while (row = mysql_fetch_row(DB_Para.Res)) {
        tmp_opunit.Sys_Name = row[0];
        tmp_opunit.Para_Code = row[1];
        tmp_opunit.Para_Name = row[2];
        tmp_opunit.Para_Index = row[3];
        tmp_opunit.Para_Method = Para_Method_Map[row[1]];
        Para_Data[tmp_opunit.Para_Index] = tmp_opunit;
    }
    mysql_free_result(DB_Para.Res);

    // 读取包数据
    std::string File_String;
    Get_Byte_From_File(File_String);
    File_String = Bin_To_Hex(File_String);
    if (File_String.size() % FrameLength != 0) {
        std::cout << "Data Error" << std::endl;
        return 0;
    }

    // 读取&存储XML解析文件
    tinyxml2::XMLDocument TelProcess;
    TelProcess.LoadFile("TelemetryProcess.xml");
    tinyxml2::XMLElement* root = TelProcess.RootElement();

    std::map<std::string, std::string> FormatDesc_Map;
    Build_XML_FormatDesc_Map(FormatDesc_Map, root);

    std::map<std::string, std::vector<std::pair<int, std::string>>> Format_Map;
    Build_XML_Format_Map(Format_Map, root);

    std::map<std::string, std::vector<CalcUnit>> Para_Calc_Map;
    Build_XML_Para_Calc_Map(Para_Calc_Map, root);

    // 按帧存储的数据包
    std::vector<Frame> Frame_Data;
    Frame_Data.clear();
    Save_Data_By_Frame(Frame_Data, File_String);
    File_String.clear();

    std::string Id_Code, Format_Code;
    std::vector <std::pair<int, std::string>> Para_Code;
    std::string Para_Index;
    OperatorUnit tmp_op;
    //std::vector <CalcUnit> 
    DB_Para.Delete_Database("delete from my_out");
    for (int i = 0; i < 4/*Frame_Data.size()*/; ++i) {
        for (int j = 0; j < 3; ++j) {
            Package now = Frame_Data[i].Data[j];
            Id_Code = now.Package_Leader.substr(2, 2);
            Format_Code = FormatDesc_Map[Id_Code];
            if (Format_Map.count(Format_Code))
                Para_Code = Format_Map[Format_Code];
            else
                continue;
            for (int k = 0; k < Para_Code.size(); ++k) {
                std::string para_idx = std::to_string(Para_Code[k].first);
                tmp_op = Para_Data[para_idx];
                std::string str_sql = "insert into my_out (para_index, sys_name, para_code, para_name, para_method, virtual_band, pkg_value, pkg_result) values ('";
                str_sql += para_idx + "', '";
                str_sql += tmp_op.Sys_Name + "', '";
                str_sql += tmp_op.Para_Code + "', '";
                str_sql += tmp_op.Para_Name + "', '";
                str_sql += tmp_op.Para_Method + "', '";
                str_sql += Para_Code[k].second + "', '";
                std::string vir_band = VirtualBand_To_DataString(Para_Code[k].second, now);
                str_sql += vir_band + "', '";
                std::string pkg_res = Calculate_To_DataString(vir_band, Para_Calc_Map[para_idx]);
                str_sql += pkg_res + "');";
                DB_Para.Insert_Database(str_sql);
                if (para_idx == "1101") {
                    CONST_5V_VOLT = std::stof(pkg_res);
                }
            }
        }
    }
    
    return 0;
}
