#ifndef WIDGETTEST_H
#define WIDGETTEST_H

#include <QWidget>

namespace Ui {
class WidgetTest;
}

class WidgetTest : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTest(QWidget *parent = 0);
    ~WidgetTest();

private:
    Ui::WidgetTest *ui;
};

#endif // WIDGETTEST_H
