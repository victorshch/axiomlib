# Файл, в котором задаются параметры сборки AxiomLib

# В данном файле должны быть заданы значения переменных 
# MPICH_INCLUDE, MPICH_LIB, BOOST_INCLUDE, BOOST_LIB, AXIOMLIB_INCLUDE, AXIOMLIB_LIB

### Пути include'ов и библиотек ###

# Для windows
win32 {
        MPICH_INCLUDE = "C:/Program Files (x86)/MPICH2/include"
        MPICH_LIB = "C:/Program Files (x86)/MPICH2/lib/mpi.lib"

        BOOST_INCLUDE = "C:/Program Files (x86)/boost/boost_1_47"
        BOOST_LIB = /LIBPATH:"C:/Program Files (x86)/boost/boost_1_47/lib/"

	OTHER_INCLUDE = 
        OTHER_LIB = shell32.lib
}

# Для unix
unix {
        MPICH_INCLUDE = /usr/include/mpich2/
	MPICH_LIB = -lmpi -lgomp -lmpich

	BOOST_LIB = -lboost_filesystem -lboost_serialization -lboost_system -lboost_thread



	OTHER_INCLUDE = 
	OTHER_LIB = -lpthread
}

win32 {

debug {
        AXIOMLIB_LIB = F:/Workspace/Axiomlib_working_copy2/debug/AxiomLib.lib
        QWT_LIB = D:/lib/qwt-5.2.1/lib/qwtd.lib
} release {
        AXIOMLIB_LIB = F:/Workspace/Axiomlib_working_copy2/release/AxiomLib.lib
        QWT_LIB = D:/lib/qwt-5.2.1/lib/qwt.lib
}
        BOOST_LIB = /LIBPATH:"C:/Program Files (x86)/boost/boost_1_47/lib"
        MPICH_LIB = "C:/Program Files (x86)/MPICH2/lib/mpi.lib"
        #MPICH_LIB = /LIBPATH:"C:/Program Files (x86)/MPICH2/lib"

}

release {
#для boost.ublas
    DEFINES += NDEBUG
}

unix {
#Здесь нужно указать абсолютнй путь к директории src
AXIOMLIB_INCLUDE = /home/mrmrmr/study/axiomlib/branches/armkor/src
#Здесь нужно указать абсолютные пути к файлу libAxiomLib.a
debug{
        AXIOMLIB_LIB = /home/mrmrmr/study/axiomlib/branches/armkor/debug/libAxiomLib.a
}
release {
        AXIOMLIB_LIB = /home/mrmrmr/study/axiomlib/branches/armkor/release/libAxiomLib.a
}

} 
win32 {
debug {
        AXIOMLIB_LIB = F:/Workspace/Axiomlib_working_copy2/debug/AxiomLib.lib
}
release {
        AXIOMLIB_LIB = F:/Workspace/Axiomlib_working_copy2/release/AxiomLib.lib
}
}

AXIOMLIB_LIBNAME = AxiomLib

#AXIOMLIB_LIB = $$AXIOMLIB_LIBDIR$$AXIOMLIB_LIBNAME

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
        QMAKE_CXXFLAGS_WARN_OFF += -Wno-unused-parameter
}

win32 {
        #Отключение ворнингов, связанных со стандартными функциями,
        #которые cl считает небезопасными
        QMAKE_CXXFLAGS_WARN_OFF += -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS
}
