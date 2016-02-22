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
    const std::vector<PietTree> copyNodes() const {return nodes;} 
    PietTree(int val){ this->val = val; isleaf = true; }
    PietTree(QChar val){ this->val = val.unicode();isleaf = true; }
    PietTree(std::vector<PietTree> nodes){ this->nodes = nodes; isleaf = false;}
    PietTree(const QString &str);
    static void MakeStackByMod0(int n,std::vector<PietTree> & pts); //Mod 0 : OK
    //Push InC,InNは定義されない
    //Popはそのまま捨てる
    //@Dup は普通にコピー
    //@Out(N) は Nodes + Nodes.size()だから処理側で
    QString showStack(bool asNumber) const ;
    int Not() const;    //@Not  : OK
    void flatten();     //@Switch:×
    PietTree popHead(); //@Point:×
    QString toString(); //@Out(C): OK //副作用:flatten
    void append(const PietTree& pt);  //@Add : OK
    void split(PietTree & pt);        //@Sub : OK
    void product(const PietTree & pt);//@Mul : OK
    void match(PietTree & pt);        //@Div  : OK
    void zip(const PietTree &pt);     //@Mod : OK
    void loadFile(const PietTree &pt); //@Greater : 未実装
};

#endif // PIETTREE_H
