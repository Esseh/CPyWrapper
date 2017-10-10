compiler=x86_64-w64-mingw32-g++
python_include=C:\Python27\Python2764\include
python_lib=C:\Python27\Python2764\libs
build-test: test.cpp
	$(compiler) -std=c++14 test.cpp -I$(python_include) -L$(python_lib) -lpython27 -o test.exe
build-main: CPyWrapper.cpp
	$(compiler) -std=c++14 CPyWrapper.cpp -I$(python_include) -L$(python_lib) -lpython27 -o main.exe