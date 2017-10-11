#include <iostream>
#include <unordered_map>
#include <Python.h>
#include <vector>

#define SmartPy(variableName,Object) PyPointer variableName=PyPointer(Object,"variableName")
#define SmartPyFunc(resultName,functionObject,arguments) PyPointer resultName=PyPointer(PyObject_CallObject(functionObject,arguments),"resultName"); 
#define SmartPyAttr(resultObject,container,handle)	PyPointer resultObject=PyPointer(PyObject_GetAttrString(container,handle),"resultObject");
class PyPointer{
	private:
		std::string name;
		PyObject* physicalObject;
	public:
		PyObject* get(){return physicalObject;}
		PyObject* operator()(){return physicalObject; }
		PyPointer(PyObject* obj,std::string uniqueName){
			static PyObject* main = PyImport_AddModule("__main__");
			name = uniqueName;
			physicalObject = obj;
			PyObject_SetAttrString(main, name.c_str(), physicalObject);
		};
		~PyPointer(){
			Py_XDECREF(physicalObject);
		};
};

class ReturnType{
	public:
		PyObject*error;
		PyObject*result;
		ReturnType(PyObject*e,PyObject*r){
			error = e;
			result= r;
		}
		~ReturnType(){
			static PyObject* main = PyImport_AddModule("__main__");
			PyObject_SetAttrString(main, "tok1", error);
			PyObject_SetAttrString(main, "tok2", result);
			Py_XDECREF(error);
			Py_XDECREF(result);
		}
};

class CPyWrapper{
	public: 
		static PyObject* GetFunction(std::string moduleName,std::string functionName){
			static std::unordered_map<std::string,PyObject*> moduleMap = std::unordered_map<std::string,PyObject*>();
			static std::unordered_map<std::string,PyObject*> functionMap = std::unordered_map<std::string,PyObject*>();
			if(moduleMap.find(moduleName) == moduleMap.end())
				moduleMap.insert({moduleName,PyImport_Import(PyString_FromString(moduleName.c_str()))});
			if(functionMap.find(moduleName+"."+functionName) == functionMap.end())
				functionMap.insert({
					moduleName+"."+functionName,
					PyObject_GetAttrString((moduleMap.find(moduleName))->second,functionName.c_str())
			});
			return functionMap.find(moduleName+"."+functionName)->second;
		}
};

ReturnType CallFunction(std::string moduleName, std::string functionName, PyObject* args){
	static PyObject* main = PyImport_AddModule("__main__");
	static PyObject* wrapperFunction = CPyWrapper::GetFunction("CPyWrapper.wrapper","executeFunction");
	SmartPy(collectibleTuple,args);
	PyObject* innerFunction = CPyWrapper::GetFunction(moduleName,functionName);
	SmartPy(nestedTuple,PyTuple_Pack(2,CPyWrapper::GetFunction("CPyWrapper.wrapper","tostring"),args));
	SmartPyFunc(functionResult,CPyWrapper::GetFunction("CPyWrapper.wrapper","executeFunction"),nestedTuple());
	for(int i = 0; i < PyTuple_Size(args); i++){
		PyObject_SetAttrString(main, "temp", PyTuple_GetItem(args,i));
		Py_XDECREF(PyTuple_GetItem(args,i));
	}
	return ReturnType(PyObject_GetAttrString(functionResult(),"error"),PyObject_GetAttrString(functionResult(),"result"));
}

int main(){
	/// Initialize python interpreter
    Py_Initialize();
	
    std::string resultString;
    for(int i = 0; i < 1000000; i++){
		ReturnType functionResult = CallFunction(
			"CPyWrapper.wrapper",
			"tostring",
			PyTuple_Pack(2,
				PyString_FromString("1.1"),
				PyString_FromString("2.1")
			)
		);
		resultString = PyString_AsString(functionResult.result);
    }
    std::cout << resultString << std::endl;
	/// Close the python interpreter
    Py_Finalize();
    return 0;
}
