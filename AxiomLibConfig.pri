# Файл, в котором задаются параметры сборки AxiomLib
# Этот файл не надо коммитить в репозиторий, он у каждого свой (в репозитории -- образец)

# В данном файле должны быть заданы значения переменных 
# MPICH_INCLUDE, MPICH_LIB, BOOST_INCLUDE, BOOST_LIB, AXIOMLIB_INCLUDE, 
# AXIOMLIB_LIBDIR, AXIOMLIB_LIBNAME

### Конфигурация ###

CONFIG += static
#CONFIG += release
#CONFIG -= debug
#CONFIG += debug
#CONFIG -= release

### Пути include'ов и библиотек ###

# Для windows
win32 {
	MPICH_INCLUDE = "C:\Program Files (x86)\MPICH2\include"
	MPICH_LIB = "C:\Program Files (x86)\MPICH2\lib\mpi.lib"

    BOOST_INCLUDE = D:\lib\boost_1_44_0
	BOOST_LIB = /LIBPATH:"D:\lib\boost_1_44_0\stage\lib"	

	OTHER_INCLUDE = 
	OTHER_LIB = 
}

# Для unix
unix {
	MPICH_INCLUDE = -I/usr/include/mpich2/
	MPICH_LIB = -lmpi -lgomp -lmpich
	BOOST_INCLUDE = "/home/wictor/boost_1_46_1/include"

	BOOST_LIBDIR = "/home/wictor/boost_1_46_1/include"
	BOOST_LIB = -lboost_filesystem -lboost_serialization -lboost_system -lboost_thread


	OTHER_INCLUDE = 
	OTHER_LIB = -lpthread
}

AXIOMLIB_INCLUDE = ./src

debug{
	AXIOMLIB_LIBDIR = ./lib/debug/
}
release {
	AXIOMLIB_LIBDIR = ./lib/release/
}

AXIOMLIB_LIBNAME = AxiomLib

AXIOMLIB_LIB = $$AXIOMLIB_LIBDIR$$AXIOMLIB_LIBNAME
