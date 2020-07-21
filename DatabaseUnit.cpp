﻿#include <iostream>
#include "DatabaseUnit.h"

DatabaseUnit::DatabaseUnit(const std::string& host, const std::string& user, const std::string& psw, const std::string& table_name, const int& port) {
	// 连接mysql，数据库
	mysql_init(&Mysql);
	//mysql_options(&Mysql, MYSQL_SET_CHARSET_NAME, "gbk");
	this->Host = host;
	this->User = user;
	this->PSW = psw;
	this->TableName = table_name;
	this->Port = port;
	this->Res = NULL;
}

bool DatabaseUnit::Connect_Database() {
	// 中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去
	if (!(mysql_real_connect(&Mysql, Host.c_str(), User.c_str(), PSW.c_str(), TableName.c_str(), Port, NULL, 0))) {
		std::cout << "Error Connecting To Database:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else {
		//mysql_query(&Mysql, "SET NAMES GB2312");
		std::cout << "Connect Success" << std::endl;
		return true;
	}
}

bool DatabaseUnit::CreateTable_Database(std::string sql_str) {
	//sql_str = G2U(sql_str.c_str());
	if (mysql_query(&Mysql, sql_str.c_str())) {
		std::cout << "Query Failed:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else {
		std::cout << "Create Table Success" << std::endl;
	}
}

bool DatabaseUnit::Query_Database(std::string sql_str) {
	//sql_str = G2U(sql_str.c_str());
	// 返回0 查询成功，返回1查询失败 
	if (mysql_query(&Mysql, sql_str.c_str())) {
		std::cout << "Query Failed:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else {
		std::cout << "Query Success" << std::endl;
	}

	//获得sql语句结束后返回的结果集  
	if (!(this->Res = mysql_store_result(&Mysql))) {
		std::cout << "Couldn't get result from " << mysql_error(&Mysql) << std::endl;
		return false;
	}
	return true;
}

bool DatabaseUnit::Insert_Database(std::string sql_str) {
	//sql_str = G2U(sql_str.c_str());
	if (mysql_query(&Mysql, sql_str.c_str()))        //执行SQL语句  
	{
		std::cout << "Query Failed:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else
	{
		std::cout << "Insert Success" << std::endl;
		return true;
	}
}

bool DatabaseUnit::Delete_Database(std::string sql_str) {
	//sql_str = G2U(sql_str.c_str());
	if (mysql_query(&Mysql, sql_str.c_str()))        //执行SQL语句  
	{
		std::cout << "Query Failed:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else
	{
		std::cout << "Delete Success" << std::endl;
		return true;
	}
}

void DatabaseUnit::Free_Database() {
	mysql_free_result(Res);  //释放一个结果集合使用的内存。
	mysql_close(&Mysql);	 //关闭一个服务器连接。
}
