#! /bin/sh
#############################################################################
#			File name: update_axioms.sh
#			File type: Unix shell script
#
#			Собирает информацию об аксиомах, апдейтит фабрику и хедер
#
#	Full name:	$evde_root/.sh
#	Author:		Eugene A. Vasin
#	Version:	0.9
#
#	File history:	6 декабря 2005 - 0.9
#
#			В настоящее время в качестве классов - аксиом распознаются
#			классы, имеющие своим непосредственным предком один только
#			класс, el_axiom. Это, конечно, недостаток.
#			Если это станет критичным, без особых проблем можно перейти
#			к системе, при которой будут распознаваться вообще все
#			классы из каталога ../src/axioms , но тогда встанет
#			другое ограничение.
#
#############################################################################

# количество аксиом (не считая el_axiom)
export N_AXIOMS=0
export AXIOM_HEADER_FILES
export AXIOM_CLASS_LIST

#############################################################################
#			get_axiom_name
#
#	Description:	Выдирает из header-файла имя аксиомы, возвращает его
#	Parameters:	$1 - имя хедер-файла в каталоге ../src/axioms
#	Returns:	0 если найден соответствующий класс, наследний el_axiom
#			    также в этом случае он добавляет имя класса в переменную AXIOM_CLASS_LIST и увеличивает на 1 значение N_AXIOMS
#			1 если в файле было 0 аксиом
#	Author:		Eugene A. Vasin
#	History:
#
#############################################################################
function get_axiom_name ( ) {
  echo Processing file $FL
  
  # пытаемся понять, сколько потенциальных названий аксиом в классе (если >1 - error)
  NUM_AXIOM_NAMES=`cat $1 | grep public | grep ElemCondition | wc -l`
  
  # проверяем, сколько потенциальных аксиом
  if [ $NUM_AXIOM_NAMES -eq 1 ]
  then
    AXIOM_NAME=`cat $1 | grep public | grep ElemCondition | sed 's/.*class[ ]\([a-zA-Z0-9_]*\).*/\1/'`
    echo "    Found elementary conditions: $AXIOM_NAME"
    AXIOM_CLASS_LIST="$AXIOM_CLASS_LIST $AXIOM_NAME"
    N_AXIOMS=`expr $N_AXIOMS + 1`

  elif [ $NUM_AXIOM_NAMES -eq 0 ]
  then
    echo "WARNING: No elementary conditions in file $FL."
  else
    echo "ERROR: ambiguos file $FL. More than one elementary condition in a single file. Exiting."
    exit -1
  fi

  return 0
}

# ходим по всем файлам в каталоге
# неправильный вариант - пробелы в именах файлов не отслеживаем
for FL in `find ../src/elem_conditions -name \*h -type f`
do
  get_axiom_name $FL
  FL=`echo $FL|sed 's/.*\/\(.*\)/\1/'`
  AXIOM_HEADER_FILES="$AXIOM_HEADER_FILES $FL"
done

# все, аксиомы найдены, можно порадовать пользователя надписью
echo Found $N_AXIOMS elementary conditions: $AXIOM_CLASS_LIST
echo "Headers in: $AXIOM_HEADER_FILES"

# шаг 1 - формируем all_axioms.h
#cat ../src/all_axioms.h | ./all_axioms_h.awk
./all_axioms_h.awk ../src/all_axioms.h > ../src/all_axioms_new.h
mv ../src/all_axioms_new.h ../src/all_axioms.h

# шаг 2 - запонляем фабрику аксиом
./el_axiom_factory_cxx.awk ../src/ElemConditionsFactory.cxx > ../src/ElemConditionsFactory_new.cxx
mv ../src/ElemConditionsFactory_new.cxx ../src/ElemConditionsFactory.cxx

# шаг 3 - делаем makefile
cat makefile | sed "s/EL_AXIOMS =.*/EL_AXIOMS = ElemCondition $AXIOM_CLASS_LIST/" > makefile.1
mv makefile.1 makefile