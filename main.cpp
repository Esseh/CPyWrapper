#include <iostream>
#include "CPyWrapper.h"
int main(){
    std::string resultString;
    for(int i = 0; i < 1000000; i++){
		auto functionResult = CPy::Func(
			"CPyWrapper.wrapper",
			"tostring",
			PyTuple_Pack(2,
				PyString_FromString("1.1"),
				PyString_FromString("2.1")
			),
			"fR"
		);

		resultString = PyString_AsString(functionResult.result);
    }
    std::cout << resultString << std::endl;
    return 0;
}
