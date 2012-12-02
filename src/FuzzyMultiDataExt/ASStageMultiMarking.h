#include "../ReducedRecognizer.h"
#include "../GoalStrategy.h"
#include "../SatPointSet.h"

#include "../Environment.h"

#include "../FuzzyDataSet.h"

#include "Common.h"
#include "ForwardDeclarations.h"
#include "ASStage.h"
#include "../MultiMarking/dtwmetric.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

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
    int choiceFunctionForAxiom(int s1, int s2) {
        return s1;
    }


    template<class Symbol, class ChoiceFunction>
    void find(std::vector <std::vector<Symbol>>* r,std::vector<std::vector<bool> > L,
              std::vector<std::vector<bool> > D,std::vector<std::vector<bool> > U,
              const std::vector<Symbol>& s1,const std::vector<Symbol>& s2,int i,int j,
              std::vector<Symbol> result,ChoiceFunction choiceF){
        while ((i>0)&&(j>0)){
            if (D[i][j]){
                result->push_back(choiceF(s1[i],s2[j]));
            }
            if (L[i][j] && U[i][j]){
                find(r,L,D,U,s1,i,j,result);
            }
            if (L[i][j]){
                j--;
            }
            if (U[i][j]){
                i--;
            }
        }
        // Тут перевернуть вектор и записать в указатель на вектор
        std::vector<Symbol> rev;
        for (int k=result->size()-1;k>=0;k--){
            rev.push_back(result[k]);
        }
        r.push_back(rev);
    }



class ASStageMultiMarking: public ASStage
{    
public:
    ASStageMultiMarking();
    void initFromEnv(const Environment& env);

    void run();

    AxiomExprSetPlus &getAS(int n);
    const AxiomExprSetPlus &getAS(int n) const;
    int getASSize() const;
    void recalculateMatterASFunc(AxiomExprSetPlus& as);
    void setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS);

    //  Метод, реализующий поиск наибольшей общей подстроки.
    template<class Symbol, class DistanceFunction, class ChoiceFunction>
    std::vector<std::vector<Symbol>> findCommonSubsequence(const std::vector<Symbol>& s1,
                                                           const std::vector<Symbol>& s2,
                                                           DistanceFunction distF,
                                                           ChoiceFunction choiceF){

        int m=s1.size()+1;
        int n=s2.size()+1;

        // Инициализация матриц
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
                    if (distF(s1[a],s2[b])==0){
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
                    if (distF==0){
                        U[a][b]=true;
                    }
                    else{
                        if (A[a-1][b]>=A[a][b-1]){
                            L[a][b]=true;
                        }
                        if (A[a-1][b]<=A[a][b-1]){
                            U[a][b]=true;
                        }

                    }
                }
            }
        // Построение наибольшей общей подстроки
        std::vector <std::vector<Symbol>> result;
        std::vector <std::vector<Symbol>>* r;
        r=&result;
        find(r,L,D,U,s1,s2,m,n,result,choiceF);

    }
};
// end
}

}
