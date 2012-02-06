#############################################################################
#			Project name: 
#			File type: GNU make makefile
#
#			Аксиоматический подход, __UNIX makefile
#
#	Full name:	.../v2/__UNIX/makefile
#	Version:	0.4
#
#	File history:	26nov2005 (0.1) - первая версия, сборка тестовой болванки для linux/gcc
#			2mar2006 (0.2) - файл переделан, собирает статически слинкованную библиотеку
#			25june2008 (0.3) - добавлена компиляция при помощи Intel C/C++ compiler v.9.1.053 and v.10.1.015
#			02june2009 (0.4) - обновлены и расширены классы библиотеки
#
#	Notes:		
#
#############################################################################

# глобальный ключ (дефолтный)
ifndef AXIOMLIB_COMPILE_MODE
# debug performance ICC_9 ICC_9_debug ICC_10 ICC_10_debug ICC_11 ICC_11_debug
  AXIOMLIB_COMPILE_MODE = performance
  HOSTNAME=wictor
# dknote_gcc42 dknote_gcc43 ka50 vostok ka50gcc34 pentium3_gcc34 athlonxp_gcc34 zigzag kali
endif

# Пути
SRCPATH = ../src
TESTSRCPATH = ../tests-src
OBJPATH = ../obj
PRGPATH = ..
LIBPATH = ..
LIBNAME = libAxiomLib.a

# ВАЖНО! ! !   Путь к месту установки MPI
#MPI_HOME_DIR = /home/usr1/mpich2-inst
MPI_HOME_DIR = /home/wictor/mpich2-install
MPI_LIB_DIR = $(MPI_HOME_DIR)/lib

# ВАЖНО! ! !   Путь к месту установки Intel C++ Compiler
ICC_9_HOME_DIR = /opt/intel/cc/9.1.053/bin
#ICC_10_HOME_DIR = /opt/intel/cce/10.1.015/bin
#ICC_10_HOME_DIR = /opt/intel/cc/10.1.015/bin
ICC_11_HOME_DIR = /opt/intel/Compiler/11.1/059/bin/ia32
# ВАЖНО! ! !   Какой компилятор используется (icc / icpc)
ICC_9 = $(ICC_9_HOME_DIR)/icpc
ICC_10 = $(ICC_10_HOME_DIR)/icc
ICC_11 = $(ICC_11_HOME_DIR)/icc

# ВАЖНО! ! !   Путь к месту установки QT
QT_USING = false
QT_HOME_DIR = /usr/local/Trolltech/Qt-4.5.0

MPILIBNAME= mpich
PMPILIBNAME= pmpich
MPICXXLIBNAME= mpichcxx
MPI_OTHERLIBS= -lpthread -lrt 

mpilibs=-l$(MPILIBNAME)
cxxlibs=-l$(MPICXXLIBNAME)

# Флаги компилятора - компиляция при помощи Intel C/C++ Compiler v 9.1.053
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_9)
  CXXFLAGS += -cxxlib-icc -g0 -O0 -vec-report0
  CXX = $(ICC_9)
endif
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_9_debug)
  CXXFLAGS += -cxxlib-icc -g0 -O0 -vec-report0 -debug
  CXX = $(ICC_9)
endif

# Флаги компилятора - компиляция при помощи Intel C/C++ Compiler v 10.1.015
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_10)
  CXXFLAGS += -vec-report0 -D"__sync_fetch_and_add(ptr,addend)=_InterlockedExchangeAdd(const_cast<void*>(reinterpret_cast<volatile void*>(ptr)), addend)"
  CXX = $(ICC_10)
endif
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_10_debug)
  CXXFLAGS += -vec-report0 -debug -D"__sync_fetch_and_add(ptr,addend)=_InterlockedExchangeAdd(const_cast<void*>(reinterpret_cast<volatile void*>(ptr)), addend)"
  CXX = $(ICC_10)
endif

# Флаги компилятора - компиляция при помощи Intel C/C++ Compiler v 11.1.059
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_11)
  CXXFLAGS += -vec-report0 
  CXX = $(ICC_11)
endif
ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_11_debug)
  CXXFLAGS += -vec-report0 -debug
  CXX = $(ICC_11)
endif

# Флаги компилятора - отладочный режим, общий
ifeq ($(AXIOMLIB_COMPILE_MODE),debug)
#  CXXFLAGS += -ggdb -Winline -fno-implement-inlines -O3 -finline-limit=5000 --param inline-unit-growth=500
  CXXFLAGS += -ggdb -Winline  -fno-implement-inlines
endif

# Флаги компилятора - с оптимизацией, без отладочной информации
ifeq ($(AXIOMLIB_COMPILE_MODE),performance)
  ifeq ($(HOSTNAME),ka50)
    CXXFLAGS += -mtune=pentium-m -march=pentium-m -O3 -mfpmath=sse,387 -msse2 -mmmx -msse -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param inline-unit-growth=1000 -fgcse-sm -fgcse-las    -fmodulo-sched -freschedule-modulo-scheduled-loops -floop-optimize2 -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),vostok)
    CXXFLAGS += -mtune=athlon-xp -march=athlon-xp -O3 -mfpmath=sse,387 -mmmx -msse -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param inline-unit-growth=1000 -fgcse-sm -fgcse-las    -fmodulo-sched -freschedule-modulo-scheduled-loops -floop-optimize2 -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),dknote_gcc42)
    CC=/usr/bin/gcc-4.2
    CXX=/usr/bin/g++-4.2
    CXXFLAGS += -mtune=native -O3
  endif
  ifeq ($(HOSTNAME),dknote_gcc43)
    CC=/usr/bin/gcc-4.3
    CXX=/usr/bin/g++-4.3
    CXXFLAGS += -mtune=native -O3
  endif
  ifeq ($(HOSTNAME),ka50gcc34)
    CC=/usr/bin/gcc-3.4
    CXX=/usr/bin/g++-3.4
    CXXFLAGS += -mtune=pentium-m -march=pentium-m -O3 -mfpmath=sse,387 -msse2 -mmmx -msse -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param inline-unit-growth=1000 --param max-inline-insns-single=1000 -fgcse-sm -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),pentium3_gcc34)
    CC=/usr/bin/gcc-3.4
    CXX=/usr/bin/g++-3.4
    CXXFLAGS += -static -mtune=pentium3 -march=pentium3 -O3 -mfpmath=sse,387 -mmmx -msse -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param inline-unit-growth=1000 --param max-inline-insns-single=1000 -fgcse-sm -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),athlonxp_gcc34)
    CC=/usr/bin/gcc-3.4
    CXX=/usr/bin/g++-3.4
    CXXFLAGS += -static -mtune=athlon-xp -march=athlon-xp -O3 -mfpmath=sse,387 -m3dnow -mmmx -msse -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param inline-unit-growth=1000 --param max-inline-insns-single=1000 -fgcse-sm -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),zigzag)
    CXXFLAGS += -mcpu=prescott -march=prescott -O3 -mmmx -msse -msse2 -mfpmath=sse,387 -maccumulate-outgoing-args -fno-implement-inlines -Winline -finline-limit=5000 --param max-inline-insns-single=1000 --param max-inline-insns-auto=1000 --param max-inline-insns=2000 -fgcse-sm -ffast-math -funroll-loops -malign-double
  endif
  ifeq ($(HOSTNAME),kali)
    CXXFLAGS += -O3 -mcpu=ultrasparc -mvis -m32 -fno-implement-inlines -Winline -finline-limit=5000 --param max-inline-insns-single=1000 --param max-inline-insns-auto=1000 --param max-inline-insns=2000 -fgcse-sm -ffast-math -funroll-loops
  endif
endif

# флаги библиотек
ifeq ($(HOSTNAME),kali)
  LIBFLAGS += -L/export/home/gevor/local/lib -L/usr/lib -lboost_filesystem-gcc-d -static -static-libgcc -L/opt/swf/gcc-3/lib
  CXXFLAGS += -I/export/home/gevor/local/include/boost-1_33_1
  ld=/opt/sfw/bin/gld
  LD=/opt/sfw/bin/gld
else
  LIBFLAGS += -lboost_filesystem -lboost_system -lboost_serialization
endif

#библиотека boost.program_options - используется только для таргета test_fmd_extended
BOOST_PO = -lboost_program_options

# Добавление возможности компиляции с учетом использования средств OpenMP
LOC_BOOL_VAL = true
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_9)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_9_debug)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_10)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_10_debug)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_11)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  ifeq ($(AXIOMLIB_COMPILE_MODE),ICC_11_debug)
    CXXFLAGS += -openmp
    LOC_BOOL_VAL = false
  endif
endif
ifeq ($(LOC_BOOL_VAL),true)
  CXXFLAGS += -fopenmp
endif


#Добавлние флагов для сборки с возможностью использования интерфейса MPI
CXXFLAGS += -I$(MPI_HOME_DIR)/include
LIBFLAGS += -L$(MPI_LIB_DIR) $(mpilibs) $(cxxlibs) $(MPI_OTHERLIBS)

#Добавлние флагов для сборки с возможностью использования библиотеки QT
ifeq ($(QT_USING),true)
    CXXFLAGS += -I$(QT_HOME_DIR)/include/QtCore -I$(QT_HOME_DIR)/mkspecs/linux-g++ -I. -I$(QT_HOME_DIR)/include/QtGui -I$(QT_HOME_DIR)/include -I. -I. -I.
    LIBFLAGS += -L$(QT_HOME_DIR)/lib
endif

# Входные классы для библиотеки
# Для каждого класса нужен .h - файл с заголовками и .cxx с реализацией

# получено командой (for i in $( ls *.cxx ); do echo `basename $i .cxx`; done;) | tr '\n' ' ' > sources.txt
SOURCE_BASENAMES = AxiomBase Axiom AxiomExpr AxiomExprPlus AxiomExprSet AxiomExprSetPlus AxiomLibException AxiomSetBase AxiomSet AxiomSetPlus AxiomSetPop AxiomSetWithStats BaseGenetics Check CheckName ClippedFullFuzzyDataSet CompareClassFactory CompareStatisticSimple CrossoverStrategyFactory CrossoverTpl CrossoverTry DataSet ElemConditionsFactory ElemCondPlus ElemCondPlusStat EnvDataSet Environment ExtGenetics FullFuzzyDataSet FuzzyDataLearnAlgorithm FuzzyDataSet FuzzyMultiData_Axioms FuzzyMultiData_AxiomSets FuzzyMultiData_ElemConds FuzzyMultiDataLearnAlgorithm GaOps GoalStrategyFactory IntervalSet Net Nets NeuralActFuncStr NeuralAuxSupplier NeuralCoderGeneric NeuralCoding NeuralCoin NeuralCommon NeuralCommonSettings NeuralDataHolder NeuralDistRejecter NeuralMultiplexer PercSettings PercSingle PercWrapper PopElemExt PreprocessFactory PreprocessSimple PreprocessSqueeze PrimeGoalStrategy QnaryPerc QnaryPercSingle QnaryPercWrapper RecognizerExtend RecognizerFactory RecognizerMetric RecognizerNet RecognizerNeuralNet ReducedRecognizerDTW ReducedRecognizerFactory ReducedRecognizerMetric RefMarkup RefMarkupSet RefMarkupSetSimple RefMarkupSimple RefTrajFinder SelectionStrategyFactory SimpleCrossoverStrategy SimpleSelectionStrategy SimpleTransmuteStrategy StartStrategy TemplateGeneticsExt TemplateGeneticsFactory TemplateManager TemplateRecognizer TestTS TransmutePrecision TransmuteStrategyFactory TransmuteTpl TreatmentFactory TreatmentSample

CXXSOURCES = $(foreach source, $(SOURCE_BASENAMES), $(source).cxx)
OBJECTS = $(foreach source, $(SOURCE_BASENAMES), $(source).o)
POBJECTS = $(foreach obj, $(OBJECTS), $(OBJPATH)/$(obj))

# Автоматически генерирующиеся классы элементарных аксиом, НЕ ТРОГАТЬ!!!
EL_AXIOMS = ElemCondition  Integral DecreaseLeftElemCondition IncreaseLeftElemCondition DecreaseRightElemCondition SecDerivativeDecreaseLeft DerivativeDecreaseLeft BetweenTwoLevels Volatility SecDerivativeIncreaseLeft SecDerivativeDecreaseRight DerivativeDecreaseRight DerivativeIncreaseRight DerivativeIncreaseLeft SecDerivativeIncreaseRight IncreaseRightElemCondition
EL_AXIOM_OBJECTS = $(foreach ax, $(EL_AXIOMS), $(ax).o)
EL_AXIOM_POBJECTS = $(foreach ax, $(EL_AXIOMS), $(OBJPATH)/$(ax).o)

# Классы библиотеки Fast Artificial Neural Networks 
FANNCLASSES = doublefann
FANN_OBJECTS = $(foreach fx, $(FANNCLASSES), $(fx).o)
FANN_POBJECTS = $(foreach fx, $(FANNCLASSES), $(OBJPATH)/$(fx).o)

# Общий target библиотеки, с перемещением includes (в будущем)
lib: $(LIBPATH)/$(LIBNAME)

# Конкретный target: статичеси связанная библиотека
$(LIBPATH)/$(LIBNAME): $(POBJECTS) $(EL_AXIOM_POBJECTS) $(FANN_POBJECTS)
	ar cr $(LIBPATH)/$(LIBNAME) $(POBJECTS) $(EL_AXIOM_POBJECTS) $(FANN_POBJECTS)
	ranlib $(LIBPATH)/$(LIBNAME)

# тестовый target
test_axioms: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_axioms.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_axioms.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестирование BaseGenetics
test_base_genetics: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_base_genetics.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_base_genetics.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_data_set: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_data_set.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_data_set.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_element: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_element.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_element.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_execute: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_execute.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_execute.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестирование ExtGenetics
test_ext_genetics: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_ext_genetics.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_ext_genetics.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_fuzzy_data: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_fuzzy_data.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_fuzzy_data.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target: test_fuzzy_multi_data - особенность сборки - без явного включения библиотек не выходила полная сборка
test_fuzzy_multi_data: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_fuzzy_multi_data.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_fuzzy_multi_data.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) $(MPI_LIB_DIR)/libmpich.a $(MPI_LIB_DIR)/libmpichcxx.a -o $(PRGPATH)/$@

# тестовый target: test_fmd_extended
test_fmd_extended: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_fmd_extended.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_fmd_extended.o $(LIBFLAGS) $(BOOST_PO) $(LIBPATH)/$(LIBNAME) $(MPI_LIB_DIR)/libmpich.a $(MPI_LIB_DIR)/libmpichcxx.a -o $(PRGPATH)/$@

# тестовый target
test_neural: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_neural.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_neural.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_precision: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_precision.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_precision.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестовый target
test_reading_ec: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_reading_ec.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_reading_ec.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# тестирование StartStrategy
test_start: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_start.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_start.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# Цель для формирования и обучения шаблонов test_template
test_template: $(LIBPATH)/$(LIBNAME) $(OBJPATH)/test_template.o
	$(CXX) $(CXXFLAGS) $(OBJPATH)/test_template.o $(LIBFLAGS) $(LIBPATH)/$(LIBNAME) -o $(PRGPATH)/$@

# сборка объектников
include $(CXXSOURCES:.cxx=.d)
%.d : $(SRCPATH)/%.cxx
	set -e; $(CXX) $(LIBDIRFLAGS) $(LIBFLAGS) $(INCDIRFLAGS) -M $(CXXFLAGS) $< \
		| sed 's/\('$*'\)\.o[ :]*/\1.o '$@' : /g' > '$@'; \
		[ -s '$@' ] || rm -f '$@'

%.d : $(SRCPATH)/elem_conditions/%.cxx
	set -e; $(CXX) $(LIBDIRFLAGS) $(LIBFLAGS) $(INCDIRFLAGS) -M $(CXXFLAGS) $< \
		| sed 's/\('$*'\)\.o[ :]*/\1.o '$@' : /g' > '$@'; \
		[ -s '$@' ] || rm -f '$@'

%.d : $(SRCPATH)/FANN/%.c
	set -e; $(CXX) $(LIBDIRFLAGS) $(LIBFLAGS) $(INCDIRFLAGS) -M $(CXXFLAGS) $< \
		| sed 's/\('$*'\)\.o[ :]*/\1.o '$@' : /g' > '$@'; \
		[ -s '$@' ] || rm -f '$@'

%.d : $(TESTSRCPATH)/%.cxx
	set -e; $(CXX) $(LIBDIRFLAGS) $(LIBFLAGS) $(INCDIRFLAGS) -M $(CXXFLAGS) $< \
		| sed 's/\('$*'\)\.o[ :]*/\1.o '$@' : /g' > '$@'; \
		[ -s '$@' ] || rm -f '$@'

%.o : $(SRCPATH)/%.cxx
	$(CXX) $(CXXFLAGS) $(LIBDIRFLAGS) $(INCDIRFLAGS) -c -o $@ $< $(LIBFLAGS)
	mv -f $@ $@.ext
	mv -f $@.ext $(OBJPATH)/$@

%.o : $(SRCPATH)/elem_conditions/%.cxx
	$(CXX) $(CXXFLAGS) $(LIBDIRFLAGS) $(INCDIRFLAGS) -c -o $@ $< $(LIBFLAGS)
	mv -f $@ $@.ext
	mv -f $@.ext $(OBJPATH)/$@

%.o : $(SRCPATH)/FANN/%.c
	$(CXX) $(CXXFLAGS) $(LIBDIRFLAGS) $(INCDIRFLAGS) -c -o $@ $< $(LIBFLAGS)
	mv -f $@ $@.ext
	mv -f $@.ext $(OBJPATH)/$@

%.o : $(TESTSRCPATH)/%.cxx
	$(CXX) $(CXXFLAGS) $(LIBDIRFLAGS) $(INCDIRFLAGS) -c -o $@ $< $(LIBFLAGS)
	mv -f $@ $@.ext
	mv -f $@.ext $(OBJPATH)/$@

# Target используется для полного ребилда списка аксиом
gen_axiomlib:
	@# перегенерируем фабрику аксиом
	./update_axioms.sh

clean:
	rm $(OBJPATH)/*.o
