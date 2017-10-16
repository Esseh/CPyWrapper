#include "CPyWrapper.h"

/// Zero initialize globals
PyObject* CPyWrapper::main = NULL;
PyObject* CPyWrapper::wrapperFunction = NULL;
std::unordered_map<std::string,PyObject*> CPyWrapper::moduleMap = std::unordered_map<std::string,PyObject*>();
std::unordered_map<std::string,PyObject*> CPyWrapper::functionMap = std::unordered_map<std::string,PyObject*>();

PyObject* CPyWrapper::GetFunction(std::string moduleName,std::string functionName){
    /// If we can find an existing module then we should add the module to our map.
    if(moduleMap.find(moduleName) == moduleMap.end())
        moduleMap.insert({moduleName,PyImport_Import(PyString_FromString(moduleName.c_str()))});
    /// If we cannot find an existing function in our namespace then we should retrieve it.
    if(functionMap.find(moduleName+"."+functionName) == functionMap.end())
        functionMap.insert({
            moduleName+"."+functionName,
            PyObject_GetAttrString((moduleMap.find(moduleName))->second,functionName.c_str())
    });
    /// Pass up the function we have found at some point.
    return functionMap.find(moduleName+"."+functionName)->second;
}

PyObject* CPyWrapper::PyPointer::get(){ return physicalObject; }

PyObject* CPyWrapper::PyPointer::operator()(){ return get(); }

CPyWrapper::PyPointer::PyPointer(PyObject* obj,std::string uniqueName){
    name = uniqueName;
    physicalObject = obj;
};
CPyWrapper::PyPointer::~PyPointer(){
    /// Embed our object into interpreter with unique symbol.
    PyObject_SetAttrString(main, name.c_str(), physicalObject);
    /// Decrease reference count so object can be collected in interpreter.
    Py_XDECREF(physicalObject);
};

CPyWrapper::ReturnType::ReturnType(PyObject*e,PyObject*r,std::string setgcID){
    error = e;
    result= r;
    gcID = setgcID;
}
CPyWrapper::ReturnType::~ReturnType(){
    /// Embed both the error and result into the interpreter and then release the C++ references.
    PyObject_SetAttrString(main, ("tok1"+gcID).c_str(), error);
    PyObject_SetAttrString(main, ("tok2"+gcID).c_str(), result);
    Py_XDECREF(error);
    Py_XDECREF(result);
}

CPyWrapper::ReturnType CPyWrapper::Func(std::string moduleName, std::string functionName, PyObject* args, std::string gcID){
    /// If the interpreter's main symbol hasn't been loaded...
    if(main == NULL){
        /// Initialize python interpreter
        Py_Initialize();
        /// Get the main symbol.
        main = PyImport_AddModule("__main__");
        /// Get our common wrapper function.
        wrapperFunction = CPyWrapper::GetFunction("CPyWrapper.wrapper","executeFunction");
    }
    /// Embed our list of arguments into a smart pointer so that it can be collected afterward.
    PyPointer collectedArguments = PyPointer(args,"collectibleTuple");
    /// Get the function to be called within the wrapper.
    PyObject* innerFunction = CPyWrapper::GetFunction(moduleName,functionName);
    /// Create our argument tuple for our wrapper function.
    PyPointer wrapperArgs = PyPointer(
        PyTuple_Pack(2,
            CPyWrapper::GetFunction("CPyWrapper.wrapper","tostring"),
            args
        ),
        "wrapperArgs"
    );
    /// Get the function result and then release our arguments.
    PyPointer functionResult = PyPointer(PyObject_CallObject(CPyWrapper::GetFunction("CPyWrapper.wrapper","executeFunction"),wrapperArgs()),"functionResult");
    for(int i = 0; i < PyTuple_Size(args); i++){
        PyObject_SetAttrString(main, "temp", PyTuple_GetItem(args,i));
        Py_XDECREF(PyTuple_GetItem(args,i));
    }
    /// Return the function result as a smart pointer.
    return ReturnType(PyObject_GetAttrString(functionResult(),"error"),PyObject_GetAttrString(functionResult(),"result"),gcID);
}
