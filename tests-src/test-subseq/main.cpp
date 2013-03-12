#include "FuzzyMultiDataExt/ASStageMultiMarking.h"
#include "MultiMarking/dtwmetric.h"
#include "MultiMarking/MetricsEqual.h"
#include "vector"
using namespace std;
using namespace AxiomLib;

int main(){

    // Случай разметки-строки
    // Test1
    std::vector<std::vector<int> > vec;
    std::vector<int> s1={1,3};
    std::vector<int> s2={3};

    vec = FuzzyMultiDataExt::findCommonSubsequence(s1,s2,FuzzyMultiDataExt::distanceFunctionForAxiom,FuzzyMultiDataExt::choiceFunctionForAxiom);

    for (int i=0;i<vec.size();i++){
        std::vector<int> p;
        p=vec[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }

    // Test 2

    std::vector<std::vector<int> > vec2;
    std::vector<int> s12={1,3,2,8,4};
    std::vector<int> s22={3,1,8,4,3,8,9,1,2,7,3,8,4};

    vec2 = FuzzyMultiDataExt::findCommonSubsequence(s12,s22,FuzzyMultiDataExt::distanceFunctionForAxiom,FuzzyMultiDataExt::choiceFunctionForAxiom);

    for (int i=0;i<vec2.size();i++){
        std::vector<int> p;
        p=vec2[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }
   // Test 3

    std::vector<std::vector<int> > vec3;
    std::vector<int> s23={1,3,2,8,4};
    std::vector<int> s121={3,1,8,4,3,8,9,1,2,7,3,8,4};

    vec3 = FuzzyMultiDataExt::findCommonSubsequence(s121,s23,FuzzyMultiDataExt::distanceFunctionForAxiom,FuzzyMultiDataExt::choiceFunctionForAxiom);

    for (int i=0;i<vec3.size();i++){
        std::vector<int> p;
        p=vec3[i];
        for (int j=0;j<p.size();j++){
            cout << p[j];
        }
        cout <<'\n';
    }

    return 0;
}

