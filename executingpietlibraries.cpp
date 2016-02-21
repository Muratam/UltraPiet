#include "executingpietlibraries.h"
#include "piettree.h"

QHash<QString,std::function<void(std::vector<PietTree>&)>> ExecutingPietLibraries::functionHash;
bool ExecutingPietLibraries::Hash_HadSet = false;
void ExecutingPietLibraries::Hash_FuncSet(){
    //C++で定義した関数はここで関数ハッシュに登録すると使える
    #define REGISTFUNCTION(FUNCTIONNAME) functionHash[QString( #FUNCTIONNAME)] = (std::function<void(std::vector<PietTree>&)>) FUNCTIONNAME;
    if(Hash_HadSet) return;
    Hash_HadSet = true;
    REGISTFUNCTION(A);
    #undef REGISTFUNCTION
}


