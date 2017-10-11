#include <iostream>
#include <unordered_map>
#include <Python.h>


class PyPointer{
	private:
		std::string name;
		PyObject* physicalObject;
	public:
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
#define SmartPy(variableName,Object) PyPointer variableName=PyPointer(Object,"variableName")
#define SmartPyFunc(resultName,functionObject,arguments) PyPointer resultName=PyPointer(PyObject_CallObject(functionObject,arguments),"resultName"); 
#define SmartPyAttr(resultObject,container,handle)	PyPointer resultObject=PyPointer(PyObject_GetAttrString(container,handle),"resultObject");
int main(){
	/// Initialize python interpreter
    Py_Initialize();
	/// Grab main: global symbol
    PyObject* po_main = PyImport_AddModule("__main__");
	
	///  Grab module object: Memoize
    PyObject* wrapper = PyImport_Import(PyString_FromString("CPyWrapper.wrapper"));
	
	/// Grab function object: Memoize
    PyObject* function = PyObject_GetAttrString(wrapper,"executeFunction");	
	
	/// Grab function object: Memoize
    PyObject* higherOrderFunction = PyObject_GetAttrString(wrapper,"tostring");	

	std::cout << function << std::endl;
	std::cout << higherOrderFunction << std::endl;
	
    std::string resultString;
    for(int i = 0; i < 1000000; i++){
		SmartPy(one,PyString_FromString("1.1"));
		SmartPy(two,PyString_FromString("2.1"));
		SmartPy(tupleObj,PyTuple_Pack(2,one(),two()));
		SmartPy(nestedTuple,PyTuple_Pack(2,higherOrderFunction,tupleObj()));
		SmartPyFunc(functionResult,function,nestedTuple());
		SmartPyAttr(resultObject,functionResult(),"result");
		resultString = PyString_AsString(resultObject());
    }
    std::cout << resultString << std::endl;
	/// Close the python interpreter
    Py_Finalize();
    return 0;
}
