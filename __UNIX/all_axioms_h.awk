#!/usr/bin/awk -f
#############################################################################
#			File name: all_axioms_h.awk
#			File type: Awk script
#
#			Заполнение файла all_axioms
#
#	Version:	0.9
#
#	File history:	6 декабря 2005 - 0.9
#
#############################################################################

/\/\/ BEGIN AXIOM HEADERS/ {
  state="writing";
  print $_;
  split (ENVIRON["AXIOM_HEADER_FILES"], axiom_header_files, " ")
  for (axiom in axiom_header_files) {
    printf ("#include \"elem_conditions/%s\"\n",axiom_header_files[axiom]);
  }
  printf("#define N_EL_AXIOMS %s\n", ENVIRON["N_AXIOMS"]);
}

/\/\/ END AXIOM HEADERS/ {
  state="stalled";
}

// {
  if (state != "writing") {
    print $_;}
}
