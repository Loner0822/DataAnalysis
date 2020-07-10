#include <iostream>
#include "DatabaseUnit.h"

DatabaseUnit::DatabaseUnit(const std::string &host, const std::string &user, const std::string &psw, const std::string &table_name, const int &port) {
	// 连接mysql，数据库
	mysql_init(&Mysql);
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
		std::cout << "Connect Success" << std::endl;
	return true;
	}
}

bool DatabaseUnit::Query_Database(std::string sql_str) {
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
		printf("Couldn't get result from %s\n", mysql_error(&Mysql));
		return false;
	}
	return true;
}

bool DatabaseUnit::Insert_Database(std::string sql_str) {
	if (mysql_query(&Mysql, sql_str.c_str()))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&Mysql));
		return false;
	}
	else
	{
		printf("Insert success\n");
		return true;
	}
}

bool DatabaseUnit::Delete_Database(std::string sql_str) {
	if (mysql_query(&Mysql, sql_str.c_str()))        //执行SQL语句  
	{
		printf("Query failed (%s)\n", mysql_error(&Mysql));
		return false;
	}
	else
	{
		printf("Delete success\n");
		return true;
	}
}

void DatabaseUnit::Free_Database() {
	mysql_free_result(Res);  //释放一个结果集合使用的内存。
	mysql_close(&Mysql);	 //关闭一个服务器连接。
}