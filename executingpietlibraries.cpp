#include "executingpietlibraries.h"
#include "piettree.h"

QHash<QString,std::function<void(std::vector<PietTree>&)>> ExecutingPietLibraries::functionHash;
bool ExecutingPietLibraries::Hash_HadSet = false;
void ExecutingPietLibraries::Hash_FuncSet(){
    //C++で定義した関数はここで関数ハッシュに登録すると使える
    //二回関数名と呼び出し名で分けて書いてしまうと整合性が取れない可能性があるため文字列化のマクロを用いる
    #define REGISTFUNCTION(FUNCTIONNAME) functionHash[QString( #FUNCTIONNAME)] = (std::function<void(std::vector<PietTree>&)>) FUNCTIONNAME;
    if(Hash_HadSet) return;
    Hash_HadSet = true;
    REGISTFUNCTION(A);
    #undef REGISTFUNCTION
}


