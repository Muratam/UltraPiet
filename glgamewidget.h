#ifndef GLGAMEWIDGET_H
#define GLGAMEWIDGET_H
#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>


class GLGameWidget : public QGLWidget{
    Q_OBJECT
public:
    explicit GLGameWidget(QWidget *parent = 0);
    QSize sizeHint() const{return QSize(w,h);}
    void setSize(int w,int h){if(w < 0 || h < 0) return;this->w = w;this->h = h; }
    static GLGameWidget* SingleGLWidget ;
protected:
    void initializeGL();
    void resizeGL(int width,int height);
    void paintGL();
private:
    void draw();
    int w = 600, h = 400;
    QColor faceColor;
};

#endif // GLGAMEWIDGET_H
