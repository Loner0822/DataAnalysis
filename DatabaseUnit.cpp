#include <iostream>
#include "DatabaseUnit.h"

DatabaseUnit::DatabaseUnit(const std::string &host, const std::string &user, const std::string &psw, const std::string &table_name, const int &port) {
	// ����mysql�����ݿ�
	mysql_init(&Mysql);
	this->Host = host;
	this->User = user;
	this->PSW = psw;
	this->TableName = table_name;
	this->Port = port;
	this->Res = NULL;
}

bool DatabaseUnit::Connect_Database() {
	// �м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ
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
	// ����0 ��ѯ�ɹ�������1��ѯʧ�� 
	if (mysql_query(&Mysql, sql_str.c_str())) {
		std::cout << "Query Failed:" << mysql_error(&Mysql) << std::endl;
		return false;
	}
	else {
		std::cout << "Query Success" << std::endl;
	}
	
	//���sql�������󷵻صĽ����  
	if (!(this->Res = mysql_store_result(&Mysql))) {
		printf("Couldn't get result from %s\n", mysql_error(&Mysql));
		return false;
	}
	return true;
}

bool DatabaseUnit::Insert_Database(std::string sql_str) {
	if (mysql_query(&Mysql, sql_str.c_str()))        //ִ��SQL���  
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
	if (mysql_query(&Mysql, sql_str.c_str()))        //ִ��SQL���  
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
	mysql_free_result(Res);  //�ͷ�һ���������ʹ�õ��ڴ档
	mysql_close(&Mysql);	 //�ر�һ�����������ӡ�
}