compiler=x86_64-w64-mingw32-g++
python_include=C:\Python27\Python2764\include
python_lib=C:\Python27\Python2764\libs
build-lib: CPyWrapper.cpp
	$(compiler) -std=c++14 -c CPyWrapper.cpp -I$(python_include) -o CPyWrapper.o
	ar -x libpython27.a
	ar rcs CPyWrapper.a *.o
	rm *.o
build-main: main.cpp
	$(compiler) -std=c++14 main.cpp CPyWrapper.a -I$(python_include) -o main.exe