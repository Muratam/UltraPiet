#ifndef DEFINES_H
#define DEFINES_H
#include <QMessageBox>
#include <integerrange.h>

using namespace iota;
auto MSGBOX = [] (const char* str, QWidget* parent = 0) {    QMessageBox msgBox(parent); msgBox.setText(QString(str)); msgBox.exec();};

#endif // DEFINES_H
