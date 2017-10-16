#include <unordered_map>
#include <Python.h>

/// Enclosing class for the wrapper.
/// Accessible as the symbol CPy.
class CPyWrapper{
	private:
	    /// Holds the main symbol. Needed to embed python variables into interpreter.
	    static PyObject* main;
	    /// Holds the wrapperFunction, this is accessed during every function call so it should not have to be retrieved each time.
	    static PyObject* wrapperFunction;
	    /// Holds any accessed module in order to prevent memory leaks.
	    /// This does incur a static memory cost for each unique module.
        static std::unordered_map<std::string,PyObject*> moduleMap;
        /// Holds any accessed functions in order to prevent memory leaks.
        /// Incurs a static memory cost for each unique function.
        static std::unordered_map<std::string,PyObject*> functionMap;
        /// A function to automatically retrieve and managed accessed functions/functions using their respective maps.
		static PyObject* GetFunction(std::string moduleName,std::string functionName);
		/// A smart pointer for storing short lived Python Objects.
        class PyPointer{
            private:
                std::string name;
                PyObject*physicalObject;
            public:
                /// Primary accessor for object
                PyObject* get();
                /// Secondary accessor for object
                PyObject* operator()();
                /// Constructs with a uniqueID.
                PyPointer(PyObject* obj,std::string uniqueName);
                /// Destructor manages reference count and embedding.
                ~PyPointer();
        };
        /// A longer lived Smart Pointer for the results of functions.
        /// A ReturnType instance should not live longer than the scope that it was made.
        class ReturnType{
            private:
                /// Unique ID to prevent conflicts in GC
                std::string gcID;
            public:
                /// The python error object, always a string. Empty string means no error.
                PyObject*error;
                /// The python result object, this is based on the function called.
                PyObject*result;
                /// Constructor for the result object.
                ReturnType(PyObject*e,PyObject*r,std::string setgcID);
                /// Destructor manages reference count and embedding.
                ~ReturnType();
        };
    public:
        /// The main function of the wrapper.
        /// Calls a python function returning a ReturnType smart pointer.
        /// A unique (but static) ID should be provided in order to prevent GC caused segmentation faults.
        static ReturnType Func(std::string moduleName, std::string functionName, PyObject* args, std::string gcID);
};
using CPy = CPyWrapper;
