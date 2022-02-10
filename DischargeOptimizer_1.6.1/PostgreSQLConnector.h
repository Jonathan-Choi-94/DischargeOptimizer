#pragma once
#include<pqxx/pqxx>
#include<iostream>
#include<string>
#include<wtypes.h>
#include<oleauto.h>
#include<tuple>


class PostgreSQLConnector
{
public:
	PostgreSQLConnector();
	virtual ~PostgreSQLConnector();

	bool ConnectDB(const char* DBinfo);
	bool ReadData(std::string column, std::string table);
	bool WriteData(std::string table, std::string data);
	bool UpdateData(std::string table, std::string updatedata, std::string condition);
	std::string GetNextHour(std::string current_time, std::string plus_hour);
	std::vector<std::vector<std::string>> GetData(
		std::string column, std::string table, std::string condition);
	std::map<std::string, std::vector<std::string>> GetDataMap(
		std::string key_column, std::string value_column, std::string table, std::string condition);

	pqxx::connection* m_mainconnection;
	pqxx::work* m_work;
	pqxx::result m_result;

};