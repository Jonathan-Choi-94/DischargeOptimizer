#include "PythonNCreader.h"

PythonNCreader::PythonNCreader() 
{
	std::string set_env = "PYTHONHOME=";
	set_env.append(g_dataarchive->PYTHONHOME);
	int index_success = putenv(set_env.c_str());

	Py_Initialize();
	pName = PyUnicode_FromString("NetCDFReader");
	pModule = PyImport_Import(pName);
	pFunc = nullptr;
	pArg = nullptr;
	pValue = nullptr;
	ptemp = nullptr;
	objectsRepresentation = nullptr;
}

PythonNCreader::~PythonNCreader() 
{
	Py_Finalize();
}

bool PythonNCreader::OutputRead(std::string nc_filename, std::vector<doublematrix>& dataset)
{
	nc_filename_ = (char*) nc_filename.c_str();
	doublematrix Q, WL;
	pName = PyUnicode_FromString("NetCDFReader");
	pModule = PyImport_Import(pName);
	if (pName != nullptr && pModule != nullptr)
	{
		if (!Parser(pModule, "Q_parser", Q))
			return false;
		if (!Parser(pModule, "WL_parser", WL))
			return false;

		dataset.push_back(Q);
		dataset.push_back(WL);
		return true;
	}
	else
	{
		return false;
	}
}

bool PythonNCreader::Parser(PyObject* pModule, const char* type_parser, doublematrix& data)
{
	// 실행할 함수를 PyObject에 전달한다. 
	pFunc = PyObject_GetAttrString(pModule, type_parser);
	// 문자열 nc_filename를 담은 매개변수를 만든다.
	pArg = Py_BuildValue("(z)", nc_filename_);
	// pFunc에 매개변수를 전달해서 실행한다. 
	pValue = PyObject_CallObject(pFunc, pArg);

	if (pValue != nullptr)
	{
		int count = (int)PyList_Size(pValue);
		int count_time = (int)PyList_Size(PyList_GetItem(pValue, 0));

		doublematrix arr(count);
		const char* return_val;

		int j = 0;
		for (int i = 0; i < count; i++)
		{
			std::vector<double> vec_temp;
			for (int j = 0; j < count_time; j++)
			{
				ptemp = PyList_GetItem(PyList_GetItem(pValue, i), j);
				objectsRepresentation = PyObject_Str(ptemp);
				PyObject* str = PyUnicode_AsEncodedString(objectsRepresentation, "utf-8", "~E~");
				return_val = PyBytes_AsString(str);
				vec_temp.push_back((float)strtod(return_val, NULL));
			}

			arr.at(i) = vec_temp;
		}

		data = arr;
		return true;
	}
	else
	{
		return false;
	}
}