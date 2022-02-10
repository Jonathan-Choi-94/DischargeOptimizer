// ver 1.6.1 Release?

#pragma once
#include "UdeEngine.h"
#include "UdeLogger.h"
#include "DataArchive.h"
#include "DataProcessor.h"
#include "PredPointCoupledModel.h"
#include "ModelTrainToolBox.h"
#include "OptimalDischargeComputer.h"
#include "time.h"
#include "string.h"

DataArchive* g_dataarchive = nullptr;
DataProcessor* g_dataprocessor = nullptr;
std::string GetFullLengthTime(std::string, std::string, std::string, std::string);
void GetInitConfigData(std::map<std::string, std::string>&);

int main(int argc, char* argv[])
{
	// Arranging default simulation informations
	std::string imt_sn = "5";
	std::string kdrum_imt_sn = "28";
	std::string kriver_imt_sn = "121";

	// Get arguments to update the serial numbers in DB
	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i" && atoi(argv[i + 1]) != false)
			imt_sn = std::string(argv[i + 1]);
		else if (std::string(argv[i]) == "-kd" && atoi(argv[i + 1]) != false)
			kdrum_imt_sn = std::string(argv[i + 1]);
		else if (std::string(argv[i]) == "-kr" && atoi(argv[i + 1]) != false)
			kriver_imt_sn = std::string(argv[i + 1]);
	}

	// Get current time 
	time_t curTime = time(NULL);
	struct tm* pLocal = localtime(&curTime);
	std::string current_year = std::to_string(pLocal->tm_year + 1900);
	std::string current_month = std::to_string(pLocal->tm_mon + 1);
	std::string current_day = std::to_string(pLocal->tm_mday);
	std::string current_hour = std::to_string(pLocal->tm_hour);
	std::string current_full_time = 
		GetFullLengthTime(current_year, current_month, current_day, current_hour);

	// Print running information
	std::cout << "DischargeOptimizer.exe" << std::endl;
	std::cout << "Current time:" << current_year << "." << current_month << "."
		<< current_day << ":" << current_hour << std::endl;
	std::cout << "imt_sn = " << imt_sn << std::endl;
	std::cout << "kdrum_imt_sn = " << kdrum_imt_sn << std::endl;
	std::cout << "kriver_imt_sn = " << kriver_imt_sn << std::endl;

	//====================================================================================
	
	std::string psql_dbname, psql_host, psql_port, psql_user, psql_password, PYTHONHOME, KriverOutFolder;
	std::map<std::string, std::string> init_config_data_map;
	GetInitConfigData(init_config_data_map);
	psql_dbname = init_config_data_map["psql_dbname"];
	psql_host = init_config_data_map["psql_host"];
	psql_port = init_config_data_map["psql_port"];
	psql_user = init_config_data_map["psql_user"];
	psql_password = init_config_data_map["psql_password"];
	PYTHONHOME = init_config_data_map["PYTHONHOME"];
	KriverOutFolder = init_config_data_map["KriverOutFolder"];

	// Define db_connector
	PostgreSQLConnector* db_connector = new PostgreSQLConnector();
	try
	{
		printf("\n 데이터 처리 단위 시작 \n");
		// Connect psql DB
		std::string dbconnect_info = "dbname=" + psql_dbname + 
			" host=" + psql_host + " port=" + psql_port +
			" user=" + psql_user + " password=" + psql_password;
		const char* dbconnect_query = dbconnect_info.c_str();
		db_connector->ConnectDB(dbconnect_query);
		
		// Write the initial data with program start time
		std::string initial_write_query = imt_sn + ", nextval('seq_optmz_imt_sn'), now()";
		db_connector->WriteData("th_ai_optmz_log", initial_write_query);
		printf("Writing initial data in DB has succeeded \n");

		// Generate data archive
		g_dataarchive = new DataArchive();
		g_dataarchive->imt_sn = imt_sn;
		g_dataarchive->PYTHONHOME = PYTHONHOME;

		// Generate data preprocessor 
		DataProcessor* g_dataprocessor = new DataProcessor();
		
		// Get the path of sj_riv.nc file
		std::vector<std::vector<std::string>> path_receiver;
		std::string get_query_condition = "imt_sn = " + imt_sn +
			" and kriver_imt_sn = " + kriver_imt_sn;
		path_receiver = db_connector->GetData("result_flpth", "th_kriver_imt_log", get_query_condition);
		if (path_receiver.at(0).empty())
			throw (std::runtime_error("Failed to get the K-River result file path"));

		// Read in&out data from DB, sj_riv.nc file
		path_receiver.at(0).at(0) = KriverOutFolder;
		g_dataprocessor->ReadKRiverOutputData(path_receiver.at(0).at(0));
		g_dataprocessor->ReadKRiverInputData(db_connector,
			kdrum_imt_sn, path_receiver.at(0).at(0));
		g_dataarchive->numof_dataset++;

		//=======================================================================================
		printf("\n 모델학습 단위 시작 \n");

		// Generate model-train toolbox
		ModelTrainToolBox* modeltraintoolbox = new ModelTrainToolBox();

		// NSE check for the whole model
		modeltraintoolbox->CheckFlowModelNSE(0); 
		modeltraintoolbox->CheckWholeModelNSE(0); 

		// Train Models 
		std::vector<int> train_dataset{ 0 };
		int train_interpolation = 1;
		if (!g_dataarchive->train_points.empty())
		{
			printf("Starts Model Training \n");
			modeltraintoolbox->TrainLevelModel(g_dataarchive->train_points, train_dataset, train_interpolation);
			printf("\n");
			 
			modeltraintoolbox->CheckFlowModelNSE(0);
			modeltraintoolbox->CheckWholeModelNSE(0);
		}

		delete modeltraintoolbox;
		
		//=======================================================================================
		printf("\n 방류량 최적화 단위 시작 \n");

		// Generate optimal discharge computer
		OptimalDischargeComputer* optdischargecomputer = new OptimalDischargeComputer();
		optdischargecomputer->Run();
		//optdischargecomputer->PrintOutToDB(db_connector);
		
		delete optdischargecomputer;
		//=======================================================================================
		// Engine Destruction
		UDEIF->Destroy();

		// Update DB with success finish time
		db_connector->UpdateData("th_ai_optmz_log", "end_dt = now()",
			"optmz_sn = currval('seq_optmz_imt_sn')");
	}
	catch (std::runtime_error& e)
	{
		// Update DB with error
		std::string error =  e.what();
		std::string query_witherror = "end_dt = now(), err_cn = '" + error + "'";
		if (db_connector->m_mainconnection != nullptr)
		{
			db_connector->UpdateData("th_ai_optmz_log", query_witherror,
				"optmz_sn = currval('seq_optmz_imt_sn')");
		}

		// Print cmd error log
		CLOG_PTR->critical("Exception: {}", e.what());
	}
	catch (std::out_of_range& e)
	{
		// Update DB with error
		std::string error = e.what();
		std::string query_witherror = "end_dt = now(), err_cn = '" + error + "'";
		db_connector->UpdateData("th_ai_optmz_log", query_witherror,
			"optmz_sn = currval('seq_optmz_imt_sn')");
		
		// Print cmd error log
		CLOG_PTR->error("Subscript out_of_range");
	}
	catch (...)
	{
		// Update DB with error
		std::string error = "Unknown exception";
		std::string query_witherror = "end_dt = now(), err_cn = '" + error + "'";
		db_connector->UpdateData("th_ai_optmz_log", query_witherror,
			"optmz_sn = currval('seq_optmz_imt_sn')");

		// Print cmd error log
		CLOG_PTR->critical("Unknown exception");
	}

	return 0;
}


std::string GetFullLengthTime(std::string current_year, std::string current_month,
	std::string current_day, std::string current_hour)
{
	std::string full_length_time;
	full_length_time.append(current_year);
	if (current_month.size() == 1)
	{
		full_length_time.append("0");
		full_length_time.append(current_month);
	}
	else
		full_length_time.append(current_month);
	if (current_day.size() == 1)
	{
		full_length_time.append("0");
		full_length_time.append(current_day);
	}
	else
		full_length_time.append(current_day);
	if (current_hour.size() == 1)
	{
		full_length_time.append("0");
		full_length_time.append(current_hour);
	}
	else
		full_length_time.append(current_hour);

	return full_length_time;
}

void GetInitConfigData(std::map<std::string, std::string>& init_config_data_map)
{
	std::string dbname, host, port, user, password, KRiverPath, PYTHONHOME, KriverOutFolder;
	std::string error_message{};

	char* getptr = new char[255];
	GetPrivateProfileStringA("PostgreSQLconfig", "dbname", "None", getptr, 255, ".\\Inidata.ini");
	dbname = getptr;
	GetPrivateProfileStringA("PostgreSQLconfig", "host", "None", getptr, 255, ".\\Inidata.ini");
	host = getptr;
	GetPrivateProfileStringA("PostgreSQLconfig", "port", "None", getptr, 255, ".\\Inidata.ini");
	port = getptr;
	GetPrivateProfileStringA("PostgreSQLconfig", "user", "None", getptr, 255, ".\\Inidata.ini");
	user = getptr;
	GetPrivateProfileStringA("PostgreSQLconfig", "password", "None", getptr, 255, ".\\Inidata.ini");
	password = getptr;
	GetPrivateProfileStringA("PYTHONHOME", "path", "None", getptr, 255, ".\\Inidata.ini");
	PYTHONHOME = getptr;
	GetPrivateProfileStringA("KriverOutFolder", "path", "None", getptr, 255, ".\\Inidata.ini");
	KriverOutFolder = getptr;

	init_config_data_map.insert(std::make_pair("psql_dbname", dbname));
	init_config_data_map.insert(std::make_pair("psql_host", host));
	init_config_data_map.insert(std::make_pair("psql_port", port));
	init_config_data_map.insert(std::make_pair("psql_user", user));
	init_config_data_map.insert(std::make_pair("psql_password", password));
	init_config_data_map.insert(std::make_pair("PYTHONHOME", PYTHONHOME));
	init_config_data_map.insert(std::make_pair("KriverOutFolder", KriverOutFolder));

	delete[] getptr;
}