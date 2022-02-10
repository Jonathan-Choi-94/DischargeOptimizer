#pragma once
#include "HEinsSim.h"
#ifdef _LICENSE_VER_
#include "LicenseManager.h"
#endif // _LICENSE_VER_

class SimLicenseManager
{
private:
	bool license_loc_init;
	bool license_checker;
	char license_loc[100];
public:
	SimLicenseManager();
	~SimLicenseManager();

	void SetLicenseFileLoc(std::string& license_file_loc);
	bool IsAvail();

private:
	void CheckLicenseFile();
	bool CheckLicenseAvail() const;
};

