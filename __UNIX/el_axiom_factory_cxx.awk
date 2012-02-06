#!/usr/bin/awk -f
#############################################################################
#			File name: el_axiom_factory_cxx.awk
#			File type: Awk script
#
#			Заполнение файла el_axiom_factory.cxx
#
#	Version:	0.9
#
#	File history:	6 декабря 2005 - 0.9
#
#############################################################################

/\/\/ BEGIN CREATE_ALL_AXIOMS/ {
  state="writing";
  print $_;
  split (ENVIRON["AXIOM_CLASS_LIST"], el_axioms, " ")
  for (axiom in el_axioms) {
    printf ("  all_axioms[%d] = new %s();\n", axiom-1, el_axioms[axiom]);
  }
}

/\/\/ BEGIN CREATE_ALL_CREATE_AXIOM_BY_NUM/ {
  state="writing";
  print $_;
  split (ENVIRON["AXIOM_CLASS_LIST"], el_axioms, " ")
  for (axiom in el_axioms) {
    printf ("    case %d:\n      toReturn = new %s();\n      break;\n", axiom, el_axioms[axiom]);
  }
}

/\/\/ END CREATE_ALL_AXIOMS/ {
  state="stalled";
}

/\/\/ END CREATE_ALL_CREATE_AXIOM_BY_NUM/ {
  state="stalled";
}

// {
  if (state != "writing") {
    print $_;}
}
