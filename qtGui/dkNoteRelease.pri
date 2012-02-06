### Задание папки, куда будет положен исполняемый файл ###
DESTDIR = ./

### Конфигурация - release или debug, можно добавить warn_off ###
CONFIG += release


### Пути include'ов ###
win32 {
# Пути к include'ам
	AXIOMLIB_INCLUDE = "C:\Users\dk\Documents\Visual Studio 2008\Projects\AxiomLib\AxiomLib\src"
	QWT_INCLUDE = "D:\qwt\src"
	MPICH_INCLUDE = "C:\Program Files\MPICH2\include"
	BOOST_INCLUDE = "D:\boost_1_42"
	OTHER_INCLUDE =
# Подключение библиотек
	AXIOMLIB_LIB = "C:\Users\dk\Documents\Visual Studio 2008\Projects\AxiomLib\release\AxiomLib.lib"
	MPICH_LIB = "C:\Program Files\MPICH2\lib\mpi.lib"
	QWT_LIB = D:\qwt\lib\qwt5.lib
	BOOST_LIB = /LIBPATH:"D:\boost_1_42\lib"
	OTHER_LIB = Shell32.lib
}
unix {
# Пути к include'ам
	AXIOMLIB_INCLUDE = /home/wictor/workspace/AxiomLib/axiomlib/src
	QWT_INCLUDE = /usr/include/qwt-qt4
	MPICH_INCLUDE = /home/wictor/mpich2-install/include
	BOOST_INCLUDE = 
	OTHER_INCLUDE = 
# Подключение бибилиотек
	AXIOMLIB_LIB = ../libAxiomLib.a
	MPICH_LIB =	/home/wictor/mpich2-install/lib/libmpich.a \
		/home/wictor/mpich2-install/lib/libmpichcxx.a
	QWT_LIB = -lqwt-qt4
	BOOST_LIB = -lboost_system -lboost_filesystem -lboost_serialization
	OTHER_LIB = -lrt -lpthread -lgomp
}

### Дополнительные флаги компиляции ###

# Для windows
win32 {
	QMAKE_CXXFLAGS += -openmp
	QMAKE_CXXFLAGS_RELEASE += /O2
}
# Для unix
unix {
	QMAKE_CXXFLAGS += -fopenmp
}

### Дополнительные флаги линковки ###

# Для windows
win32 {
	QMAKE_LFLAGS +=
}
# Для unix
unix {
	QMAKE_LFLAGS += -fopenmp
}

### Задание флагов отключение warning'ов (работает только если warn_off есть в CONFIG)

# Для windows - надо читать мануал по cl

# Для unix
unix {
	QMAKE_CXXFLAGS_WARN_OFF = -Wno-sign-compare -Wno-sign-conversion -Wno-unused-parameter
}
