#include "glgamewidget.h"
#include "defines.h"

GLGameWidget* GLGameWidget::SingleGLWidget = nullptr;
GLGameWidget::GLGameWidget(QWidget *parent):QGLWidget(parent){
    setFormat(QGLFormat(QGL::DoubleBuffer ));
    faceColor = Qt::red;
    if(SingleGLWidget == nullptr) SingleGLWidget = this;
    else SingleGLWidget = nullptr;
}
//before paintGL
void GLGameWidget::initializeGL(){
    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    qglClearColor(Qt::black);
    glOrtho(0.0, w, 0.0, h , -1.0, 1.0);
}

//paintGL <= resizeGL <= initializeGL
void GLGameWidget::resizeGL(int width,int height){
    glViewport(0, 0, width, height);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}

void GLGameWidget::paintGL(){
        QGLContext* context = QGLWidget::context();
        context->makeCurrent();
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT);
    draw();
        context->swapBuffers();
        context->doneCurrent();
}

void GLGameWidget::draw(){
    qglColor(faceColor);
    glBegin(GL_QUADS);
    glVertex2d(w*0.1,h* 0.1);
    glVertex2d(w*0.9,h* 0.1);
    glVertex2d(w*0.9,h* 0.9);
    glVertex2d(w*0.1,h* 0.9);
    glEnd();
}

// 当面2Dで考える 3DにしたかったらGLGameWidget3Dとかつくればいい
// 本当の最低限だけでいいや (本当のゲームのライブラリならもっと関数が必要なところだ)
// 音楽も同期をとらなければ
// texture = bindTexture(QImage(":/image.jpg", GL_TEXTURE_2D));
// QMediaPlayer
//0. L/system         ["ls"]            => output ni are
//1. L/GLShowWindow   [600,400,"title"] => [0|1] (SingleTon) / MainWindow | NULL
//2. L/GLLoadImage    ["res/chihaya.png"]  => [0|handle]     / GLView
//   L/MPLoadAudio    ["res/arcadia.mp3"]  => [0|mhandle] (SingleTon)
//3. L/GLDrawImage    [handle,100,200]  => None              / GLView
//   L/MPPlayAudio    [mhandle]         => None
//4. L/GLShowBuffer   []                => None              / GLView
//   L/MPGetPosition  [mhandle]         => 100
//5. L/sleep          [16]              => None


//3D memo
//construct : | QGL::DepthBuffer));
//init      : glEnable(GL_DEPTH_TEST);
//resize    : glMatrixMode(GL_PROJECTION);  auto x = GLfloat(width) / height;  glFrustum(-x,x,-1.0,1.0,4.0,15.0);  glMatrixMode(GL_MODELVIEW); //2Dなのでプロジェクションする意味はない
//draw      : glCLear(~ | GL_DEPTH_BUFFER_BIT); glMatrixMode(GL_MODELVIEW);   glLoadIdentity();  glTranslatef(0.0,0.0,-10.0);
