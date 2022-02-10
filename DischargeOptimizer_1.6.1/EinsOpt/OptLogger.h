#pragma once
#include "spdlog/spdlog.h"
#include "EinsOpt.h"

class UDEOPT_EXT_CLASS OptLogger
{
public:
	static OptLogger* GetPtr();

	//File logger for optimizer 
	spdlog::logger* GetOptLogger(void) const { return opt_logger_.get(); }
	//Console logger for optimizer 
	spdlog::logger* GetOptConsoleLogger(void) const { return opt_console_logger_.get(); }

	//Start the logger for engine debugging 
	void StartEngDebugLogger() const;
	//Start the logger for optimization
	void StartOptLogger();
	//Stop the simulation logger 
	//void StopSimLogger() const;
	//relase all logger
	void Release();
	
private:
	OptLogger();
	static OptLogger* ptr_instance_;

	//std::shared_ptr<spdlog::logger> sim_console_logger_;
	std::shared_ptr<spdlog::logger> eng_logger_;
	//std::shared_ptr<spdlog::logger> model_logger_;
	std::shared_ptr<spdlog::logger> opt_logger_;
	std::shared_ptr<spdlog::logger> opt_console_logger_;
};

/**
 * \brief : console logger for model and simulation
 */
//File logger for optimizer 
#define OPT_LOG_PTR OptLogger::GetPtr()->GetOptLogger()
//Console logger for optimizer 
#define OPT_CON_PTR OptLogger::GetPtr()->GetOptConsoleLogger()
