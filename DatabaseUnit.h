#include <mysql.h>
#include <string>

class DatabaseUnit {
public:
	MYSQL Mysql;
	MYSQL_RES *Res;
	std::string Host;
	std::string User;
	std::string PSW;
	std::string TableName;
	int Port;
	
	DatabaseUnit(const std::string& host, const std::string& user, const std::string& psw, const std::string& table_name, const int& port);

	//����false������ʧ�ܣ�����true�����ӳɹ�  
	bool Connect_Database();

	bool Query_Database(std::string sql_str);

	bool Insert_Database(std::string sql_str);

	bool Delete_Database(std::string sql_str);

	void Free_Database();
};