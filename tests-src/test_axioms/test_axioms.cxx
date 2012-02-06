/****************************************************************************
*			File name: test_elem_conditions.cxx
*			File type: C++ program file
*
*			Тестовая площадка для отработки классоа ElemConditions
*
*	Copyright (C) Eugene A. Vasin, 2000 - 2001. All rights reserved
*
*	Version:	0.1
*
*	File history:	26 нобяря 2005 (0.1) - болванка, linux/gcc
*
****************************************************************************/

#include "all_axioms.h"
#include "Axiom.h"
#include "AxiomSet.h"
#include "ElemConditionsFactory.h"
#include <iostream>
#include <string>
#include <time.h>
#include "AxiomLibException.h"

using namespace std;
using namespace AxiomLib;

int main(int argc, char** argv) {
 try {

  cout<< "\nTest starts.\n";

  srand( (unsigned)time( NULL ) );

  ElemConditionsFactory eaf;

  Axiom *ax1, *ax2;
  ax1 = new Axiom ();
  ax2 = new Axiom ("\n\thzzz");
  cout << "\n\tnames of a created axioms are:" << "\n\t";
  cout << ax1->name() << "\n\t";
  cout << ax2->name();
  delete (ax1);
  delete (ax2);

  Axiom* ax3;
  ax3 = new Axiom ("Testing nums");
  cout << "\n\t" << ax3->name() << "\n\t";
  std::vector<signed int> v;
  v.resize(1);
  v[0] = 1;
  //v[1] = 1;
  ax3->initAxiomByNums(v);
  delete (ax3);

  Axiom* ax4;
  ax4 = new Axiom ("Testing names");
  cout << ax4->name() << "\n\t";
  std::vector <std::string> s;
  s.resize(1);
  s[0].assign ("integral");
  //s[1] = "template axiom";
  ax4->initAxiomByNames(s);
  delete (ax4);

  std::map<std::string, std::map<std::string, std::map<std::string, std::string> > > axPars;
  std::map<std::string, std::map<std::string, std::string> > mapAx;
  std::map<std::string, std::string> mapEC;

  mapEC["left"] = "0.5";
  mapEC["right"] = "0.75";
  mapAx["integral"] = mapEC;
  axPars["no_name"] = mapAx;

  Axiom* ax5;
  ax5 = new Axiom ("Testing Set Axiom Params");
  cout << ax5->name() << "\n\t";
  std::vector <std::string> sp;
  sp.resize(1);
  sp[0] = "integral";
  //sp[1] = "template axiom";
  ax5->initAxiomByNames(sp);  
  ax5->setECParameters(mapAx);
  delete (ax5);

  Axiom* ax6;
  ax6 = new Axiom ("Testing TP");
  cout << ax6->name() << "\n\t";
  std::vector <std::string> spt;
  spt.resize(1);
  spt[0] = "integral";
  //spt[1] = "template axiom";
  ax6->initAxiomByNames(sp);  
  ax6->setECParameters(mapAx);
  ax6->transmute(0.9, 0.3);
  delete (ax6);

  Axiom* ax7;
  ax7 = new Axiom ("Testing rand");
  cout << ax7->name() << "\n\t";
  //std::vector <std::string> spt;
  //spt.resize(1);
  //spt[0] = "template axiom";
  //spt[1] = "template axiom";
  ax7->initAxiomByRand(1);
  ax7->setECParameters(mapAx);
  ax7->transmute(0.9, 0.3);
  delete (ax7);

  cout<< "\n\tTesting AxiomSets" << "\n\t" << "Name of a testing AxiomSets are:" << "\n";
  AxiomSet* axiomSet;
  axiomSet = new AxiomSet("My AxiomSet");  
  cout << "\n\t" << axiomSet->name() << "\n\t";
  axiomSet->initAxiomSetByRand(2,1);
  axiomSet->transmuteAxiom(0.5, 0.6, 0.4);
  axiomSet->setAxiomParameters(axPars);
  delete(axiomSet);

  cout<< "\nTest done.\n\n";

  return 0;
 }
 catch ( AxiomLibException testex ) {
   cerr << "Fatal exception:\n\t";
   cerr << testex.error() << "\n";
 }
}

