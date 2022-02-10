#pragma once
#include <Python.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <DataArchive.h>

typedef std::vector<std::vector<double>> doublematrix;

class PythonNCreader
{
public:
	PythonNCreader();
	virtual ~PythonNCreader();

	bool StationInfoRead(const char* nc_filename, std::vector<double>&);
	bool OutputRead(std::string nc_filename, std::vector<doublematrix>&);
	bool Parser(PyObject* pModule, const char* type_parser, doublematrix&);

	PyObject* pName, * pModule, * pFunc, * pArg, * pValue, * ptemp, * objectsRepresentation;;
	char* nc_filename_;
};