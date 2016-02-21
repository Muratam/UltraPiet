#ifndef DEFINES_H
#define DEFINES_H
#include <QMessageBox>

//REPだけはエイリアスとしてこのプロジェクト全体どうしても使いたいです！ごめんね！
#define REP(i,n) for(int i = 0 ; i < (int)(n);++i)
auto MSGBOX = [] (const char* str, QWidget* parent = 0) {    QMessageBox msgBox(parent); msgBox.setText(QString(str)); msgBox.exec();};

#endif // DEFINES_H
