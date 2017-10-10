#include <iostream>
#include <unordered_map>
#include <Python.h>

using ModuleMap = std::unordered_map<std::string,PyObject*>;
using FunctionMap = std::unordered_map<std::string, std::unordered_map<std::string,PyObject*> >;
class CPyWrapper {
    private:
        static ModuleMap PyModuleMap;
        static FunctionMap PyFunctionMap;
        static PyObject* pyLibrary(std::string libName){
            auto lib = Type(libName);
            auto importLib = PyImport_Import(lib);
            return importLib;
        }
        static PyObject* getFunction(std::string moduleName, std::string functionName){
            auto module = PyModuleMap.find(moduleName);
            if(module == PyModuleMap.end()){
                PyModuleMap.insert({moduleName,pyLibrary(moduleName)});
                PyFunctionMap.insert({moduleName,{}});
                module = PyModuleMap.find(moduleName);
            }
            auto functionSubMap = (PyFunctionMap.find(moduleName))->second;
            auto function = functionSubMap.find(functionName);
            if(function == functionSubMap.end()){
                auto foundFunction = Attribute(module->second,functionName);
                functionSubMap.insert({functionName,foundFunction});
                function = functionSubMap.find(functionName);
            }
            return function->second;
        }
    public:
        static void Init(int argc, char *argv[]){
            Py_SetProgramName(argv[0]);
            Py_Initialize();
            PyEval_InitThreads();
        }
        static void Close(){
            Py_Finalize();
        }
        static PyObject* Attribute(PyObject* moduleObject,std::string attributeOrMethodName){
            return PyObject_GetAttrString(moduleObject,(char*)attributeOrMethodName.c_str());
        }
        static PyObject* Function(std::string moduleName, std::string functionName, PyObject*args){
            static auto packedArgs = PyTuple_New(2);
            auto wrapperFunc = getFunction("CPyWrapper.wrapper", "executeFunction");
            auto mainFunc = getFunction(moduleName, functionName);

            PyGILState_STATE PyGILState_Ensure();
            PyTuple_SET_ITEM(packedArgs,0,mainFunc);
            PyTuple_SET_ITEM(packedArgs,1,args);
            auto functionResult = PyObject_CallObject(mainFunc,packedArgs);
            PyGILState_STATE PyGILState_Release();

            return functionResult;
        }
        static PyObject* Type(long t){
            return PyInt_FromLong(t);
        }
        static PyObject* Type(std::string t){
            return PyString_FromString((char*)t.c_str());
        }
        static PyObject* Error(PyObject* resultObject){
            return Attribute(resultObject,"error");
        }
        static PyObject* Result(PyObject* resultObject){
            return Attribute(resultObject,"result");
        }
};
FunctionMap CPyWrapper::PyFunctionMap = {};
ModuleMap CPyWrapper::PyModuleMap = {};
using Py = CPyWrapper;

/*int main(int argc, char *argv[]){
    static PyObject* args1 = PyTuple_New(2);
    static PyObject* args2 = PyTuple_New(2);
    PyObject *functionResult1, *functionResult2, *error1, *error2;
    PyObject *one, *two, *three, *four;
    std::string result1,result2;
    Py::Init(argc,argv);
    for(int i = 0; i < 1000000; i++){
        one   = PyFloat_FromDouble(1.0);
        two   = PyFloat_FromDouble(1.0);
        three = PyFloat_FromDouble(1.0);
        four  = PyString_FromString((char*)"abc");
        PyTuple_SET_ITEM(args1,0,one);
        PyTuple_SET_ITEM(args1,1,two);
        PyTuple_SET_ITEM(args2,0,three);
        PyTuple_SET_ITEM(args2,1,four);
        functionResult1 = Py::Function("CPyWrapper.wrapper","testAdd",args1);
        functionResult2 = Py::Function("CPyWrapper.wrapper","testAdd",args2);
        Py_DECREF(one);
        Py_DECREF(two);
        Py_DECREF(three);
        Py_DECREF(four);
        //error1 = Py::Error(functionResult1);
        //error2 = Py::Error(functionResult2);
        //result1 = PyString_AsString(error1);
        //result2 = PyString_AsString(error2);
    }
	std::cout << result1 << std::endl;
	std::cout << result2 << std::endl;
	Py::Close();
	std::cin >> result1;
}*/

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
    for(int i = 0; i < 10000000; i++){
		// Construct Args
        PyObject* one = PyString_FromString("1.1");
        PyObject* two = PyString_FromString("2.1");
        PyObject* tupleObj = PyTuple_Pack(2,one,two);
		PyObject* nestedTuple = PyTuple_Pack(2,higherOrderFunction,tupleObj);
		PyObject_SetAttrString(po_main, "outertuple", nestedTuple);
		// Function result gained
        PyObject* functionResult = PyObject_CallObject(function,nestedTuple);
		PyObject* actualResult = PyObject_GetAttrString(functionResult,"result");
		PyObject* errorPortion = PyObject_GetAttrString(functionResult,"error");
		
		// DID I DO A GOOD?
        resultString = PyString_AsString(actualResult);
		
		/// Done: Decrement reference count on C++ side so references only exist in interpreter
		// Automatable
		Py_XDECREF(errorPortion);
		Py_XDECREF(actualResult);
        Py_XDECREF(functionResult);
		Py_XDECREF(nestedTuple);
        Py_XDECREF(tupleObj);
        Py_XDECREF(one);
        Py_XDECREF(two);
    }
    std::cout << resultString << std::endl;
	/// Close the python interpreter
    Py_Finalize();
    return 0;
}
