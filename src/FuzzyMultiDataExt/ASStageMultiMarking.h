#include "../ReducedRecognizer.h"
#include "../GoalStrategy.h"
#include "../SatPointSet.h"

#include "../Environment.h"
#include "../MultiMarking/MetricsEqual.h"
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
                    return m->compute(v1, v2);
            }
        };

    inline double distanceFunctionForAxiom(int s1, int s2) {
        return s1 == s2 ? 0 : 1;
    }

    // Выбор элемента

    inline int choiceFunctionForAxiom(int s1, int s2) {
        return s1;
    }
    inline std::vector<bool> choiceFunctionForMultiMark(const std::vector<bool>& v1, const std::vector<bool>& v2){
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

    inline void matrix_debug(std::vector<std::vector<bool> > D){
        std::cout << "\n/////////////////////////\nMatrix:\n";
        std::vector<bool> temp;
        for (int i = 0; i < D.size(); i++){
            temp.clear();
            temp = D[i];
            std::cout << "\n-------------------------------\n";
            for (int j = 0; j < temp.size(); j++){
                std::cout << temp[j] << "\t";
            }
        }
        std::cout << "\n/////////////////////////\n";
    }

    template<class Symbol, class DistanceFunction>
    std::vector <std::vector<Symbol> > delEqual (std::vector<std::vector<Symbol> > temp, DistanceFunction distF ){
        std::vector <std::vector<Symbol> > result;
        result.push_back(temp[0]);
        bool equal;
        for (int i=0;i<temp.size();i++){
            equal=true;
            std::vector<Symbol> subTemp=temp[i];
            for(int j=0;j<result.size();j++){
                // Сравнение на отсчетах
                std::vector<Symbol> subResult=result[j];
                for(int k=0;k < std::min (temp[i].size(),result[j].size()) ; k++ ){
                    if (distF(subTemp[k],subResult[k])!=0){
                        equal=false;
                    }
                }
                }
                if (!equal){
                    result.push_back(temp[i]);
                }
            }
        return result;
    }

    template<class Symbol>
    std::vector <std::vector<Symbol> > findMax (std::vector<std::vector<Symbol> > temp,int maxNOP){
        // TODO Проверка, что вектор непустой
        std::vector <std::vector<Symbol> > result;
        int max=0;
        for (int i=0;i<temp.size();i++){
            if (temp[i].size() > max){
                max=temp[i].size();
                }
        }
        int j=0;
        for (int i=0;i<temp.size();i++){
            if (j>= maxNOP) break;
            if (temp[i].size()==max){
                result.push_back(temp[i]);
                j++;
            }
        }
        return result;
    }


    template<class Symbol, class ChoiceFunction>
    int find(std::vector <std::vector<Symbol> >* r,std::vector<std::vector<bool> > L,
              std::vector<std::vector<bool> > D,std::vector<std::vector<bool> > U,
              const std::vector<Symbol>& s1,const std::vector<Symbol>& s2,int i,int j,
              std::vector<Symbol> result,ChoiceFunction choiceF,int numOfIt,int maxNOP){

        while ((i > 0) && (j > 0)){
            // Debug
           //std::cout << "\ni = "<<i<<" j = "<<j<<"\n";

            if (D[i][j] == true){
                // Debug
                //std::cout << "Equal " << i << " " << j;

                result.push_back(choiceF(s1[i-1], s2[j-1]));
                i--;
                j--;
            }
            else {
                // Debug
                //std::cout << "else";
            if (U[i][j]==true && L[i][j]==true){
                // Debug
//                std::cout << "U&&L=true " << i << " " << j;
//                std::cout << "numOfIt" << numOfIt << "maxNOP" << maxNOP ;
                if (numOfIt < maxNOP){
                    // debug
                    // std::cout << "\nnumOfIt" << numOfIt << "\n";
                    numOfIt++;
                    find(r, L, D, U, s1, s2, i, j-1, result, choiceF, numOfIt, maxNOP);
                }
                i--;
            }
            else
            {
                // Debug
                // std::cout << "else-else";
            if (L[i][j]==true){
                j--;
            }
            if (U[i][j]==true){
                i--;
            }
        }
        }
        }
        if (result.size() > 0){
            std::vector < Symbol >  rev=result;
            std::reverse(rev.begin(),rev.end());
            r->push_back(rev);
        }
            return 0;


/*
        if ((i<=0)||(j<=0)){
            std::cerr << "\nI'm inside i:"<< i << "j:"<< j<<"\n";
            std::vector < Symbol >  rev=result;
            if (result.size()>0){
            std::reverse(rev.begin(),rev.end());
            r->push_back(rev);
            return 0;
        }
            return 0;
        }
        else{
            if (D[i][j]==true){
                result.push_back(choiceF(s1[i-1],s2[j-1]));
                find(r,L,D,U,s1,s2,i-1,j-1,result,choiceF);
                return 0;
            }
            else{
                if (L[i][j]==true){
                    find(r,L,D,U,s1,s2,i-1,j,result,choiceF);

                }
                if (U[i][j]==true){
                    find(r,L,D,U,s1,s2,i,j-1,result,choiceF);
                }
                return 0;
            }
        }
        return 0;
*/
    }

    template<class Symbol, class DistanceFunction, class ChoiceFunction>
    std::vector<std::vector<Symbol> >  findCommonSubsequence(const std::vector<Symbol>& s1,const std::vector<Symbol>& s2,DistanceFunction distF,ChoiceFunction choiceF,double porog,int maxNOP){
        int n=s1.size()+1;
        int m=s2.size()+1;

        std::vector<std::vector<double> > A(n);
        std::vector<std::vector<bool> > L(n),D(n),U(n);
        for (int u = 0;u < n; u++){
                A[u].resize(m);
                L[u].resize(m);
                D[u].resize(m);
                U[u].resize(m);
        }
        int k = 0;
        // Заполнение матриц.
        for (int b = 0 ; b < m ; b++ ){
                for (int a = 0 ; a < n ; a++) {
                        A[a][b] = 0;
                        D[a][b] = false;
                        L[a][b] = false;
                        U[a][b] = false;
                }
        }
        for (int b = 1 ; b < m ; b++ ){
                for (int a = 1 ; a < n ; a++) {
                    distF (s1[a - 1], s2[b - 1]);
                    if (distF(s1[a - 1], s2[b - 1]) < porog){
                        A[a][b] = A[a - 1][b - 1] + 1;
                    }
                    else
                    {
                        A[a][b] = std::max(A[a - 1][b],A[a][b - 1]);
                    }
                }
        }

        for (int b = 1 ; b < m ; b++ ){
                for (int a = 1 ; a < n ; a++) {
                    if (distF(s1[a - 1],s2[b - 1]) < porog){
                        D[a][b] = true;
                    }
                    else {
                        if (A[a - 1][b] <= A[a][b - 1]){
                            L[a][b] = true;
                        }
                        if (A[a - 1][b] >= A[a][b - 1]){
                            U[a][b] = true;
                        }
                        if (A[a - 1][b] == A[a][b - 1]){
                            k++;
                        }

                    }
                }
            }
        // Debug
        //matrix_debug( D );
        //matrix_debug( L );
        //matrix_debug( U );


        // Построение наибольшей общей подстроки
        std::vector <std::vector<Symbol> > j;
        std::vector <std::vector<Symbol> >* r;
        r=&j;
        std::vector<Symbol> temp;
        // Wrong work
        find(r, L, D, U, s1, s2, n-1, m-1, temp, choiceF, 0, maxNOP);

        // Debug
//        std::cout << "r->size() = " << r->size();
//        j=*r;
//        return j;

        if (r->size()<1){
            j=*r;
            return j;
        }
        else{
            j=*r;
            j=findMax (j, maxNOP);
            return delEqual(j,distF) ;
        }
    }


    class ASStageMultiMarking: public ASStage
    {
    public:
        ASStageMultiMarking(FuzzyDataSet* fuzzyDataSet,
                            AXStage* stage2);
        //Параметры запуска;

        const FuzzyDataSet* fuzzyDataSet;
        const AXStage* stage2;
        // Метрика для поиска наибольшей общей подпоследовательности
        MultiMarking::DTWMetric* m;

        // Метрика для распознавателя.
        MultiMarking::DTWMetric* mForRecognize;
        //Порог для сравнения при подходе с подмножествами
        double porog;
        double porogForDTW;
		bool areMultiMark;

		double maxGoalToOptimizeWeights;

        // Растяжка для нештатных при расплознавании(Размер окна)
        double mStretch;
        // Максимальное количество наибольших общих подпоследовательностей у двух(!) разметок
        int maxNOP;
        // Максимальное число аксиом в популяции
        int maxAxiomSetPopSize;
        // Максимальное число шагов в алгоритме построения систем аксиом
        int maxNumberOfSteps;
        // Чиcло лучших сохраняемых систем аксиом для каждого процесса
        int numberOfBestAxiomSets;
        // Набор лучших систем аксиом
        std::vector<AxiomExprSetPlus> bestAxiomSets;
        // Набор лучшей системы аксиом
        Logger* logger;
        // Процент лучших систем аксиом в популяции, которые сохраняются при переходе на следующий шаг - (1 - percentBestAxiomSets) - выбираются случайно
        double percentBestAxiomSets;
        int ccNumPoints; // Число точек в области, после которого начислять ошибку за каждую лишнюю точку в этой области (используется при подсчете статистики)
        // Критерий остановки для алгоритма построения системы аксиом - требуемое значение целевой функции
        double bestAxiomSetGoal;
        typedef boost::shared_ptr<GoalStrategy> PGoalStrategy;
        // Стратегия подсчета целевой функции по ошибкам I и II рода
        PGoalStrategy goalStrategy;

        // Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
        double shiftErrIn;
        // Степень расширения разметки (неточность при поиске разметок во время их подбора)
        double extendMarkUpLensLevel;
    private:
        // Функции, используемые в run
		std::vector<int> convert(std::vector<double> resultInDouble) const;
		void recognize (const AxiomExprSetPlus& as, std::vector <std::vector<bool> > &markUp, const std::vector <std::vector<bool> > &genMarkUp, std::vector <int> &result) const;
        int sortBestAxiomSets (void);
        int cutDownAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) const;
        int addAxiomSets(std::vector <AxiomExprSetPlus> &nextStepAxiomSets, std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets) const;
        int sortAxiomSets (const std::vector <AxiomExprSetPlus> &axiomSets, std::vector <int> &indecies) const;
		inline int getStatistic (std::vector <int> &row) const;
		int chooseBestMarkUp (AxiomExprSetPlus &as, int abType, std::vector <std::vector <bool> > &markUp, const std::vector <std::vector <std::vector <bool> > > &genMarkUps, int &errFirstVal, int &errSecondVal) const;
		double matterAxiomSetFunc(AxiomExprSetPlus &as, const std::vector<std::vector<std::vector<std::vector<bool> > > >& markupVariants) const;
		double matterAxiomSetFunc (AxiomExprSetPlus &as, std::vector <std::vector <std::vector <bool> > > &markUps) const;
		double matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, const std::vector <std::vector<bool> > &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) const;
		double matterAxiomSetFunc (AxiomExprSetPlus &as) const;

		/// Выбор веса для последней добавленной аксиомы
		void selectWeight(AxiomExprSetPlus& as) const;

        static std::vector<std::vector<int > > stringIn(std::vector<std::vector<std::vector<bool> > > &genMarkUps,int &numberOfAxiom);
        static std::vector<std::vector<std::vector<bool> > > stringOut(std::vector<std::vector<int > > &genMarkUps,int &numberOfAxiom);

        inline int createSimpleMarkUpVariants (std::vector<std::vector<std::vector<bool> > > &genMarkUps, const int numOfAxioms) const ;
		inline int createMarkUpVariants (std::vector<std::vector<std::vector<bool> > > &genMarkUps,std::vector<std::vector<std::vector<bool> > >  &resMarkUps) const;
		void createRefMarkUp ( AxiomExprSetPlus &as, FuzzyDataSet::DataSetDivisionType division, int classNo, int multiTSNo, std::vector <std::vector<bool> >& result) const;
        int tryAddAxiom(AxiomExprSetPlus &as, const AxiomExpr &ax, int axIndex);
		void createAllMarkUpVariants(AxiomExprSetPlus &as,std::vector<std::vector<std::vector<std::vector<bool> > > > &markUpVariants) const;


        inline int simplifyMarkUps (std::vector <std::vector <std::vector<bool> > > &markUps) const;



        int chooseIndicesOfBestAxiomSets (std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets, double &goal) const;
        int addToBestAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets);

    public:
        //Наследованные функции
        void initFromEnv(const Environment& env);
        // Построение системы аксиом
        void run();
        AxiomExprSetPlus &getAS(int n);
        const AxiomExprSetPlus &getAS(int n) const;
        int getASSize() const;
		void recalculateMatterASFunc(AxiomExprSetPlus& as) const;
        void setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS);

    };

// end
}

}
