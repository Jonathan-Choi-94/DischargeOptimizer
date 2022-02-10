#include "PostgreSQLConnector.h"

char* UTF8ToANSI(const char* pszCode)
{
	BSTR    bstrWide;
	char* pszAnsi;
	int     nLength;

	// Get nLength of the Wide Char buffer
	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1,
		NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	// Change UTF-8 to Unicode (UTF-16)
	MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

	// Get nLength of the multi byte buffer
	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];

	// Change from unicode to mult byte
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	return pszAnsi;
}


PostgreSQLConnector::PostgreSQLConnector()
{
	m_mainconnection = nullptr;
	m_work = nullptr;
	m_result = {};
}

PostgreSQLConnector::~PostgreSQLConnector()
{
	m_mainconnection->close();
	std::cout << "DB is disconnected and connector is destructed" << std::endl;
}

bool PostgreSQLConnector::ConnectDB(const char* DBinfo)
{
	m_mainconnection = new pqxx::connection(DBinfo);
	std::cout << "Connected to " << m_mainconnection->dbname() << '\n';

	return true;
}


bool PostgreSQLConnector::ReadData(std::string column, std::string table)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_read("");
	query_for_read.append("SELECT ");
	query_for_read.append(column);
	query_for_read.append(" FROM ");
	query_for_read.append(table);
	const char* query = query_for_read.c_str();

	m_result = m_work->exec(query);

	for (auto row : m_result)
	{
		for (auto column : row)
		{
			std::cout << UTF8ToANSI(column.c_str()) << '\t';
		}
		std::cout << '\n';
	}

	delete m_work;
	return true;
}

bool PostgreSQLConnector::WriteData(std::string table, std::string data)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_write("");
	query_for_write.append("INSERT INTO ");
	query_for_write.append(table);
	query_for_write.append(" VALUES( ");
	query_for_write.append(data);
	query_for_write.append(")");
	const char* query = query_for_write.c_str();

	m_work->exec(query);
	m_work->commit();

	delete m_work;

	return true;
}

bool PostgreSQLConnector::UpdateData(std::string table, std::string updatedata, std::string condition)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_write("");
	query_for_write.append("UPDATE ");
	query_for_write.append(table);
	query_for_write.append(" SET ");
	query_for_write.append(updatedata);
	query_for_write.append(" WHERE ");
	query_for_write.append(condition);
	const char* query = query_for_write.c_str();

	m_work->exec(query);
	m_work->commit();

	delete m_work;

	printf("Updating data in DB has succeeded \n");
	return true;
}

std::string PostgreSQLConnector::GetNextHour(std::string current_time, std::string plus_hour)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_read("");
	query_for_read.append("SELECT to_timestamp('");
	query_for_read.append(current_time);
	query_for_read.append("', 'YYYY.MM.DD.HH24') + '");
	query_for_read.append(plus_hour);
	query_for_read.append(" hours'");
	const char* query = query_for_read.c_str();

	m_result = m_work->exec(query);
	
	std::string next_hour;
	for (auto row : m_result)
	{
		next_hour = row.front().c_str();
	}

	delete m_work;
	return next_hour;
}

// Use this api only when getting columns in a double vector container
std::vector<std::vector<std::string>> PostgreSQLConnector::GetData(
	std::string column, std::string table, std::string condition)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_get("");
	query_for_get.append("SELECT ");
	query_for_get.append(column);
	query_for_get.append(" FROM ");
	query_for_get.append(table);
	query_for_get.append(" WHERE ");
	query_for_get.append(condition);
	const char* query = query_for_get.c_str();
	
	m_result = m_work->exec(query);

	int column_size = m_result[0].size();
	std::vector<std::vector<std::string>> data_acquired(column_size);
	for (auto row : m_result)
	{
		for (int i = 0; i < column_size; i++)
		{
			std::string temp_str = row[i].c_str();
			data_acquired.at(i).push_back(temp_str);
		}
	}

	delete m_work;

	return data_acquired;
}


// Use this api only when getting the initial water level in reservoir
std::map<std::string, std::vector<std::string>> PostgreSQLConnector::GetDataMap(
	std::string key_column, std::string value_column, std::string table, std::string condition)
{
	m_work = new pqxx::work(*m_mainconnection);

	std::string query_for_get("");
	query_for_get.append("SELECT ");
	query_for_get.append(key_column);
	query_for_get.append(",");
	query_for_get.append(value_column);
	query_for_get.append(" FROM ");
	query_for_get.append(table);
	query_for_get.append(" WHERE ");
	query_for_get.append(condition);
	const char* query = query_for_get.c_str();

	m_result = m_work->exec(query);

	int column_size;
	std::map<std::string, std::vector<std::string>> data_acquired;
	std::string temp_str;
	std::vector<std::string> temp_strvec;
	for (auto row : m_result)
	{
		column_size = row.size();
		temp_str = row[0].c_str();
		for (int i = 1; i < column_size; i++)
		{
			temp_strvec.push_back(row[i].c_str());	
		}
		data_acquired.insert(std::make_pair(temp_str, temp_strvec));
		temp_strvec.clear();
	}

	delete m_work;

	return data_acquired;
}


