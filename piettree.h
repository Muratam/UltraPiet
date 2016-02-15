#ifndef PIETTREE_H
#define PIETTREE_H
#include <QString>
#include <vector>
#include "defines.h"

//先にカラー数字Pushに対応した方がよいかもしれない

class PietTree{
private:
    int val; //isLeafの時しか意味を持たない
    bool isleaf;
    std::vector<PietTree> nodes;
public :
    int Val() const {return val;}
    bool isLeaf() const {return isleaf;}
    bool isEmptyTree() const {return !isleaf && (nodes.size() == 0); }
    const std::vector<PietTree> & Nodes() const {return nodes;}
    const std::vector<PietTree> copyNodes() const {
        return nodes;
    } //std::vector<PietTree> cp;std::copy(nodes.begin(), nodes.end(), back_inserter(cp) );

    PietTree(int val){ this->val = val; isleaf = true; }
    PietTree(std::vector<PietTree> nodes){ this->nodes = nodes; isleaf = false;}
    static PietTree MakeStackByMod0(int n,std::vector<PietTree> & pts);

    //Push InC,InNは定義されない
    //Popはそのまま捨てればよい
    //@Dup は普通にコピー(ポインタとか参照とか使ってないし)
    //@Out(N) は Nodes + Nodes.size()だから処理側で頑張れ
    QString showStack() const ; //@Show : OK
    int Not() const;    //@Not  : OK
    void flatten();     //@Switch:×
    PietTree popHead(); //@Point:×
    QString toString(); //@Out(C): OK //副作用:flatten
    void append(const PietTree& pt);  //@Add : OK
    void split(const PietTree & pt);  //@Sub : ?? 未実装
    void product(const PietTree & pt);//@Mul : OK
    void match(const PietTree & pt);  //@Div  : ?? 未実装
    void zip(const PietTree &pt);     //@Mod : OK
    void loadFile(const PietTree &pt); //@Greater : 未実装
    //void loadDll(4 args); //@Roll : 未実装
    //Div 0 //未実装

};

#endif // PIETTREE_H
