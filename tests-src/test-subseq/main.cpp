#include "FuzzyMultiDataExt/ASStageMultiMarking.h"
#include "MultiMarking/dtwmetric.h"
#include "MultiMarking/AsymmBetweenSets.h"
#include "MultiMarking/AsymmHamming.h"
#include "MultiMarking/Dice.h"
#include "MultiMarking/Jaccard.h"
#include "MultiMarking/MetricsBetweenSets1.h"
#include "MultiMarking/MetricsBetweenSets2.h"
#include "MultiMarking/MetricsEuclidean.h"
#include "MultiMarking/MetricsHamming.h"
#include "MultiMarking/MetricsMatrix.h"
#include "MultiMarking/Priority.h"
#include "MultiMarking/WeakEqual.h"
#include "vector"

using namespace std;
using namespace AxiomLib;

std::vector<std::vector<std::vector<bool>>> vec4;
std::vector<std::vector<bool> > s41;
std::vector<std::vector<bool> > s42;
MultiMarking::DTWMetric* m;

void makeTest(void){
    FuzzyMultiDataExt::DistanceFunctor distanceFunctor = FuzzyMultiDataExt::DistanceFunctor(m);

    vec4=FuzzyMultiDataExt::findCommonSubsequence(s41,s42,distanceFunctor,FuzzyMultiDataExt::choiceFunctionForMultiMark, 1, 100);

    for (int i = 0;i < vec4.size(); i++){

        cout <<"Common sequence №"<<i <<'\n';
        std::vector<std::vector<bool>> p;
        p = vec4[i];
        cout <<'{';
        for (int j=0;j<p.size();j++){
            cout <<'{';
            std::vector<bool> element;
            element = p[j];

            for (int k=0;k<element.size();k++){
                cout<<element[k];
            }
            cout <<'}';
        }
        cout <<'}';
        cout <<'\n';
    }
}

void startTest(void){
    std::cout <<'\n' << "Test Equal"<<'\n';
    m=new MultiMarking::Equal;
    makeTest();
 /*   std::cout <<'\n' << "Test AsymmBetweenSets"<<'\n';
    m=new MultiMarking::AsymmBetweenSets;
    makeTest();
    std::cout <<'\n' << "Test AsymmHamming"<<'\n';
    m=new MultiMarking::AsymmHamming;
    makeTest();
    std::cout <<'\n'<< "Test Dice"<<'\n';
    m=new MultiMarking::Dice;
    makeTest();
    std::cout <<'\n'<< "Test Jaccard"<<'\n';
    m=new MultiMarking::Jaccard;
    makeTest();
    std::cout <<'\n' << "Test BetweenSets1"<<'\n';
    m=new MultiMarking::BetweenSets1;
    makeTest();
    std::cout <<'\n' << "Test BetweenSets2"<<'\n';
    m=new MultiMarking::BetweenSets2;
    makeTest();
    std::cout <<'\n' << "Test Euclidean"<<'\n';
    m=new MultiMarking::Euclidean;
    makeTest();*/
    std::cout <<'\n' << "Test Hamming"<<'\n';
    m=new MultiMarking::Hamming;
    makeTest();/*
    std::cout <<'\n' << "Test Priority"<<'\n';
    m=new MultiMarking::Priority;
    makeTest();
    std::cout <<'\n' << "Test WeakEqual"<<'\n';
    m=new MultiMarking::WeakEqual;
    makeTest();*/
}


int main(){
    cout <<"Test1"<<'\n';

    // Случай разметки-строки
    // Test1
    std::vector<std::vector<int> > vec;
    std::vector<int> s1={1,3};
    std::vector<int> s2={3};

    vec = FuzzyMultiDataExt::findCommonSubsequence(s1,s2,
                                    FuzzyMultiDataExt::distanceFunctionForAxiom,
                                    FuzzyMultiDataExt::choiceFunctionForAxiom,1, 100);

    for (int i = 0;i < vec.size();i++){
        std::vector<int> p;
        p = vec[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }

    // Test 2
    cout <<"Test2"<<'\n';

    std::vector<std::vector<int> > vec2;
    std::vector<int> s12={1,3,2,8,4};
    std::vector<int> s22={3,1,8,4,3,8,9,1,2,7,3,8,4};

    vec2 = FuzzyMultiDataExt::findCommonSubsequence(s12,s22,FuzzyMultiDataExt::distanceFunctionForAxiom,FuzzyMultiDataExt::choiceFunctionForAxiom,1, 100);

    for (int i=0;i<vec2.size();i++){
        std::vector<int> p;
        p=vec2[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }
   // Test 3
    cout <<"Test3"<<'\n';

    std::vector<std::vector<int> > vec3;
    std::vector<int> s23={1,3,2,8,4};
    std::vector<int> s121={3,1,8,4,3,8,9,1,2,7,3,8,4};

    vec3 = FuzzyMultiDataExt::findCommonSubsequence(s121,s23,FuzzyMultiDataExt::distanceFunctionForAxiom,FuzzyMultiDataExt::choiceFunctionForAxiom,1, 100);

    for (int i=0;i<vec3.size();i++){
        std::vector<int> p;
        p=vec3[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }

    //Using Metric
    
    // Test 1
    // Inicializacia pervoj sravnivaemoj stroki
    std::vector<bool> part1Ofs41={false,false,true};
    std::vector<bool> part2Ofs41={true,true,false};
    std::vector<bool> part3Ofs41={false,true,false};
    std::vector<bool> part4Ofs41={false,false,false};
    s41.push_back(part1Ofs41);
    s41.push_back(part2Ofs41);
    s41.push_back(part3Ofs41);
    s41.push_back(part4Ofs41);

    // Inicializacia vtoroj sravnivaemoj stroki
    std::vector<bool> part1Ofs42={true,true,false};
    std::vector<bool> part2Ofs42={true,true,true};
    std::vector<bool> part3Ofs42={false,false,false};
    std::vector<bool> part4Ofs42={true,false,false};
    s42.push_back(part1Ofs42);
    s42.push_back(part2Ofs42);
    s42.push_back(part3Ofs42);
    s42.push_back(part4Ofs42);

    cout <<"Test: Using Metrics"<<'\n'<<'\n';
    cout << "Compare:"<< '\n';
    cout << '{'<<'{'<<part1Ofs41[0]<<','<<part1Ofs41[1]<<','<<part1Ofs41[2]<<'}'
            <<'{'<<part2Ofs41[0]<<','<<part2Ofs41[1]<<','<<part2Ofs41[2]<<'}'
            <<'{'<<part3Ofs41[0]<<','<<part3Ofs41[1]<<','<<part3Ofs41[2]<<'}'
            <<'{'<<part4Ofs41[0]<<','<<part4Ofs41[1]<<','<<part4Ofs41[2]<<'}'<<'}';

   cout <<'\n';
   cout << '{'<<'{'<<part1Ofs42[0]<<','<<part1Ofs42[1]<<','<<part1Ofs42[2]<<'}'
        <<'{'<<part2Ofs42[0]<<','<<part2Ofs42[1]<<','<<part2Ofs42[2]<<'}'
        <<'{'<<part3Ofs42[0]<<','<<part3Ofs42[1]<<','<<part3Ofs42[2]<<'}'
        <<'{'<<part4Ofs42[0]<<','<<part4Ofs42[1]<<','<<part4Ofs42[2]<<'}'<<'}';

    startTest();

    s41.clear();
    s42.clear();
    std::vector<bool> s41_part;
    s41_part.push_back(true);
    // [1]	[1]	[1]	[1]	[1]	[1]	[1]	[1]
    s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);
    s41_part.clear();
    s41_part.push_back(false);
    for (int i = 0;i < 47; i++){
        s41.push_back(s41_part);
    }
    s41_part.clear();
    s41_part.push_back(true);
    s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);s41.push_back(s41_part);

    for (int i = 0; i < 6; i++){
        s42.push_back(s41_part);
    }
    std::cout << "-----------------------------------------------------------" ;

    std::cout << "\nCompare";

    cout << '{';
    for (int j = 0;j< s41.size(); j++){
        cout <<'{';
        std::vector<bool> element;
        element = s41[j];

        for (int k = 0;k < element.size(); k++){
            cout << element[k];
        }
        cout << '}';
    }
    cout << '}';
    cout << '\n';

    std::cout << "\nWith";
    cout << '{';
    for (int j = 0;j< s42.size(); j++){
        cout <<'{';
        std::vector<bool> element;
        element = s42[j];

        for (int k = 0;k < element.size(); k++){
            cout << element[k];
        }
        cout << '}';
    }
    cout << '}';
    cout << '\n';


    startTest();

// Test stringIn stringOut

    //Comparing 01    01      01      01      01      01      11      11      01      01      01      01      01      01      01      01 01   00      00      00      00      10      00      00      00      00      00      00      00      10

   // with01  01      01      01      01      01      01      01

    s41.clear();
    s42.clear();
    std::vector<bool> part, part2, part3, part4;
    part.push_back(false);
    part.push_back(true);

    for (int i = 0; i < 7; i++)
    {
        s42.push_back(part);
    }
    // 01    01      01      01      01      01
    for (int i = 0; i < 6; i++)
    {
        s41.push_back(part);
    }

    part2.push_back(true);
    part2.push_back(true);
    // 11   11
    s41.push_back(part2);
    s41.push_back(part2);
    // 01      01      01      01      01      01      01      01 01
    for (int i = 0; i < 9; i++)
    {
        s41.push_back(part);
    }
    part3.push_back(false);
    part3.push_back(false);
    // 00      00      00      00
    for (int i = 0; i < 4; i++)
    {
        s41.push_back(part3);
    }
    part4.push_back(true);
    part4.push_back(false);
    // 10
    s41.push_back(part4);

    //00      00      00      00      00      00      00
    for (int i = 0; i < 7; i++)
    {
        s41.push_back(part3);
    }
    //10
    s41.push_back(part4);


    std::cout << "-----------------------------------------------------------" ;

    std::cout << "\nCompare";

    cout << '{';
    for (int j = 0;j< s41.size(); j++){
        cout <<'{';
        std::vector<bool> element;
        element = s41[j];

        for (int k = 0;k < element.size(); k++){
            cout << element[k];
        }
        cout << '}';
    }
    cout << '}';
    cout << '\n';

    std::cout << "\nWith";
    cout << '{';
    for (int j = 0;j< s42.size(); j++){
        cout <<'{';
        std::vector<bool> element;
        element = s42[j];

        for (int k = 0;k < element.size(); k++){
            cout << element[k];
        }
        cout << '}';
    }
    cout << '}';
    cout << '\n';


    startTest();

    return 0;
}

