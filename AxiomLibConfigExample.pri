# Файл, в котором задаются пути к либам и некоторые общие параметры сборки проектов из AxiomLib

### Пути include'ов ###

# Для windows
win32 {
	AXIOMLIB_INCLUDE = D:\My_documents\Workspace\Axiomlib_working_copy\AxiomLib\src
        QWT_INCLUDE = D:\lib\qwt-5.2.1\src
	MPICH_INCLUDE = "C:\Program Files (x86)\MPICH2\include"
        BOOST_INCLUDE = D:\lib\boost_1_44_0
	OTHER_INCLUDE =
}
# Для unix
unix {
	AXIOMLIB_INCLUDE = /home/wictor/Projects/axiomlib/src
        QWT_INCLUDE = "/usr/include/qwt-qt4"
        MPICH_INCLUDE = "/usr/include/mpich2"
        BOOST_INCLUDE = "/home/wictor/boost_1_46_1/include"
	OTHER_INCLUDE =	
}

### Спецификация библиотек ###

# Для windows
win32 {

debug {
        AXIOMLIB_LIB = D:\My_documents\Workspace\Axiomlib_working_copy\AxiomLib\debug\AxiomLib.lib
        QWT_LIB = D:\lib\qwt-5.2.1\lib\qwtd.lib
        MPICH_LIB = "C:\Program Files (x86)\MPICH2\lib\mpi.lib"
} release {
        AXIOMLIB_LIB = D:\My_documents\Workspace\Axiomlib_working_copy\AxiomLib\release\AxiomLib.lib
        QWT_LIB = D:\lib\qwt-5.2.1\lib\qwt.lib
        MPICH_LIB = "C:\Program Files (x86)\MPICH2\lib\mpi.lib"
}
        BOOST_LIB = /LIBPATH:"D:\lib\boost_1_44_0\stage\lib"
        #MPICH_LIB = /LIBPATH:"C:\Program Files (x86)\MPICH2\lib"

#        BOOST_LIB = D:\lib\boost_1_44_0\stage\lib\boost_filesystem-vc100-mt.lib \
#            D:\lib\boost_1_44_0\stage\lib\boost_system-vc100-mt.lib \
#            D:\lib\boost_1_44_0\stage\lib\boost_serialization-vc100-mt.lib
	OTHER_LIB = Shell32.lib
}

# Для unix
unix {
linux-icc-64|linux-icc {
        AXIOMLIB_LIB = /home/wictor/Projects/axiomlib/release/libAxiomLib.a
} else {
        AXIOMLIB_LIB = /home/wictor/Projects/axiomlib/release/libAxiomLib.a
}
        BOOST_LIBDIR = /home/wictor/boost_1_46_1/lib

        MPICH_LIB = -lmpi -lgomp -lmpich
	QWT_LIB = -lqwt-qt4
        BOOST_LIB = $${BOOST_LIBDIR}/libboost_filesystem.a \
        $${BOOST_LIBDIR}/libboost_system.a \
        $${BOOST_LIBDIR}/libboost_serialization.a \
        $${BOOST_LIBDIR}/libboost_thread.a
        OTHER_LIB = -lpthread
}


### Дополнительные флаги компиляции ###

# Для windows
win32 {
	QMAKE_CXXFLAGS_RELEASE += -openmp
	QMAKE_CXXFLAGS_RELEASE += -o2 -DRELEASE
	QMAKE_CXXFLAGS_DEBUG += /ZI /Od
}
# Для unix
linux-g++|linux-g++-64 {
        QMAKE_CXXFLAGS_RELEASE += -fopenmp -o2 -std=c++0x
}
linux-icc|linux-icc-64 {
        QMAKE_CXXFLAGS += -std=c++0x -no-multibyte-chars -wd913
        QMAKE_CXXFLAGS_RELEASE += -openmp
        QMAKE_LFLAGS_RELEASE += -openmp
}

### Дополнительные флаги линковки ###

# Для windows
win32 {
	QMAKE_LFLAGS +=
	QMAKE_LFLAGS_DEBUG += /debug /incremental:yes
}

### Задание флагов отключение warning'ов (работает только если warn_off есть в CONFIG)

# Для unix
unix {
	#QMAKE_CXXFLAGS_WARN_OFF = -Wno-sign-compare -Wno-sign-conversion -Wno-unused-parameter
	QMAKE_CXXFLAGS_WARN_OFF = -Wno-unused-parameter
}

win32 {
        #Отключение ворнингов, связанных со стандартными функциями,
        #которые cl считает небезопасными
        QMAKE_CXXFLAGS_WARN_OFF += -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS
}
