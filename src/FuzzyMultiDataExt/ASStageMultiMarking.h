#include "../ReducedRecognizer.h"
#include "../GoalStrategy.h"
#include "../SatPointSet.h"

#include "../Environment.h"

#include "../FuzzyDataSet.h"
#include "../MultiMarking/dtwmetric.h"
#include "Common.h"
#include "ForwardDeclarations.h"
#include "ASStage.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {
    //  Расстояние между элементами.
    struct DistanceFunctor {
        MultiMarking::DTWMetric* m;
        DistanceFunctor(MultiMarking::DTWMetric* m) { this->m = m; }

            // Сигнатура оператора соответствует требованию к DistanceFunction
            double operator() (const std::vector<bool>& v1, const std::vector<bool>& v2) {
                    return m->compute(v1,v2);
            }
        };



    double distanceFunctionForAxiom(int s1, int s2) {
        return s1 == s2 ? 0 : 1;
    }
    // Выбор элемента

    int choiceFunctionForAxiom(int s1, int s2) {
        return s1;
    }
    std::vector<bool> choiceFunctionForMultiMark(const std::vector<bool>& v1, const std::vector<bool>& v2){
        std::vector<bool> result;
        result.resize(std::max(v1.size(),v2.size()));
        for (int i=0;i<result.size();i++){
            result[i]=false;
        }
        for (int i=0; i<(std::min(v1.size(),v2.size()));i++){
            if (v1[i] && v2[i]){
                result[i]=true;
            }
        }
        return result;

    };



    template<class Symbol, class ChoiceFunction>
    void find(std::vector <std::vector<Symbol> >* r,std::vector<std::vector<bool> > L,
              std::vector<std::vector<bool> > D,std::vector<std::vector<bool> > U,
              const std::vector<Symbol>& s1,const std::vector<Symbol>& s2,int i,int j,
              std::vector<Symbol> result,ChoiceFunction choiceF){
        while ((i>0)&&(j>0)){
            if (D[i][j]==true){
                choiceF(s1[i-1],s2[j-1]);
                result.push_back(choiceF(s1[i-1],s2[j-1]));
                i--;
                j--;
            }
            else {
            if (L[i][j]==true && U[i][j]==true){
                find(r,L,D,U,s1,s2,i-1,j,result,choiceF);
                find(r,L,D,U,s1,s2,i,j-1,result,choiceF);
                i=-1;
            }
            else
            {
            if (L[i][j]==true){
                j--;
            }
            if (U[i][j]==true){
                i--;
            }
        }
        }
        }
        std::vector<Symbol> rev;
        for (int k=result.size()-1;k>=0;k--){
            rev.push_back(result[k]);
        }
        r->push_back(rev);
    }

    template<class Symbol, class DistanceFunction, class ChoiceFunction>
    std::vector<std::vector<Symbol> >  findCommonSubsequence(const std::vector<Symbol>& s1,const std::vector<Symbol>& s2,DistanceFunction distF,ChoiceFunction choiceF){

        int n=s1.size()+1;
        int m=s2.size()+1;

        std::vector<std::vector<double> > A(n);
        std::vector<std::vector<bool> > L(n),D(n),U(n);
        for (int u=0;u<n;u++){
                A[u].resize(m);
                L[u].resize(m);
                D[u].resize(m);
                U[u].resize(m);
        }

        // Заполнение матриц.
        for (int b=0 ; b < m ; b++ ){
                for (int a=0 ; a < n ; a++) {
                        A[a][b]=0;
                        D[a][b]=false;
                        L[a][b]=false;
                        U[a][b]=false;
                }
        }
        for (int b=1 ; b < m ; b++ ){
                for (int a=1 ; a < n ; a++) {
                    if (distF(s1[a-1],s2[b-1])==0){
                        A[a][b]=A[a-1][b-1]+1;
                    }
                    else
                    {
                        A[a][b]=std::max(A[a-1][b],A[a][b-1]);
                    }
                }
        }

        for (int b=1 ; b < m ; b++ ){
                for (int a=1 ; a < n ; a++) {
                    if (distF(s1[a-1],s2[b-1])==0){
                        D[a][b]=true;
                    }
                    else{
                        if (A[a-1][b]<=A[a][b-1]){
                            L[a][b]=true;
                        }
                        if (A[a-1][b]>=A[a][b-1]){
                            U[a][b]=true;
                        }

                    }
                }
            }
        /*
        ///////////////////Дебаг
        for (int b=0 ; b < m ; b++ ){
                for (int a=0 ; a < n ; a++) {
                    std::cout << A[b][a],D[b][a],L[b][a],U[b][a],'/n';

                }
        }
*/
       /////////////////////

        // Построение наибольшей общей подстроки
        std::vector <std::vector<Symbol> > j;
        std::vector <std::vector<Symbol> >* r;
        r=&j;
        std::vector<Symbol> temp;
        find(r,L,D,U,s1,s2,n-1,m-1,temp,choiceF);
        return j;

    }

class ASStageMultiMarking: public ASStage
{    
public:
    ASStageMultiMarking();
    //Параметры запуска;
    MultiMarking::DTWMetric* m;
    double porog;


    void initFromEnv(const Environment& env);

    void run();

    AxiomExprSetPlus &getAS(int n);
    const AxiomExprSetPlus &getAS(int n) const;
    int getASSize() const;
    void recalculateMatterASFunc(AxiomExprSetPlus& as);
    void setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS);




};
// end
}

}
