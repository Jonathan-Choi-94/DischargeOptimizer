#pragma once
#include "HEinsSim.h"
#include "spdlog/spdlog.h"

class UDESIM_EXT_CLASS UdeLogger
{
public:
	static UdeLogger* GetPtr();

	//Console logger for simulation 
	spdlog::logger* GetSimConsole(void) const { return sim_console_logger_.get(); }
	//File logger for simulation engine
	spdlog::logger* GetEngineLogger(void) const { return eng_logger_.get(); }
	//File logger for model 
	spdlog::logger* GetModelLogger(void) const { return model_logger_.get(); }
	//File logger for optimizer 
	//spdlog::logger* GetOptLogger(void) const { return opt_logger_.get(); }
	//Console logger for optimizer 
	//spdlog::logger* GetOptConsoleLogger(void) const { return opt_console_logger_.get(); }

	//Start the logger for engine debugging 
	void StartEngDebugLogger() const;
	//Start the logger for optimization
	//void StartOptLogger();
	//Stop the simulation logger 
	void StopSimLogger() const;
	//relase all logger
	void Release();
	//Start simulation out_most_model_name
	void StartSimLogger(const std::string& out_most_model_name);

private:
	UdeLogger();
	static UdeLogger* ptr_instance_;

	std::shared_ptr<spdlog::logger> sim_console_logger_;
	std::shared_ptr<spdlog::logger> eng_logger_;
	std::shared_ptr<spdlog::logger> model_logger_;
	//std::shared_ptr<spdlog::logger> opt_logger_;
	//std::shared_ptr<spdlog::logger> opt_console_logger_;
};

/**
 * \brief : console logger for model and simulation 
 */
#define CLOG_PTR UdeLogger::GetPtr()->GetSimConsole()
//File logger for simulation engine
#define ELOG_PTR UdeLogger::GetPtr()->GetEngineLogger()
//File logger for model 
#define MLOG_PTR UdeLogger::GetPtr()->GetModelLogger()
//File logger for optimizer 
//#define OPT_LOG_PTR UdeLogger::GetPtr()->GetOptLogger()
//Console logger for optimizer 
//#define OPT_CON_PTR UdeLogger::GetPtr()->GetOptConsoleLogger()
