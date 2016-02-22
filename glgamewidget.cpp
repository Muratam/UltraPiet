#include "glgamewidget.h"
#include "defines.h"

GLGameWidget* GLGameWidget::UniqueGLWidget = nullptr;
GLGameWidget::GLGameWidget(QWidget *parent):QGLWidget(parent){
    setFormat(QGLFormat(QGL::DoubleBuffer ));
    connect(this,GLGameWidget::destroyed ,[=](){UniqueGLWidget = nullptr;});
}

GLGameWidget* GLGameWidget::MakeUniqueGLWidget (QWidget *parent ){
    if(UniqueGLWidget == nullptr){
        return UniqueGLWidget = new GLGameWidget(parent);
    }else return nullptr;
}


void GLGameWidget::initializeGL(){
    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    qglClearColor(Qt::black);
    glOrtho(0.0, w, 0.0, h , -1.0, 1.0);
}

void GLGameWidget::resizeGL(int width,int height){
    glViewport(0, 0, width, height);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
}
void GLGameWidget::refresh (){
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT);
}


void GLGameWidget::drawRect(int x ,int y,int w,int h,QColor color){
    qglColor(color);
    glBegin(GL_QUADS);
    glVertex2d(x,y);
    glVertex2d(x+w,y);
    glVertex2d(x+w,y+h);
    glVertex2d(x,y+h);
    glEnd();
}

// 0   : Miss
// > 1 : Succ
int GLGameWidget::loadImage(QString path){
    QImage image(path);
    if(image.isNull())return 0;
    unsigned int uihandle = bindTexture(image);
    imageSizeHash[uihandle] = QSize(image.width(),image.height());
    return (int)(uihandle);
}

void GLGameWidget::drawImage(int x,int y,int handle){
    unsigned int uihandle = (unsigned int)(handle);
    if(!imageSizeHash.contains(uihandle)) return;
    QSize size = imageSizeHash[uihandle];
    glBindTexture(GL_TEXTURE_2D, uihandle);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex2d(x,y);
        glTexCoord2f(1,0); glVertex2d(x+size.width(),y);
        glTexCoord2f(1,1); glVertex2d(x+size.width(),y+size.height());
        glTexCoord2f(0,1); glVertex2d(x,y+size.height());
    glEnd();
}


// 当面2Dで考える 3DにしたかったらGLGameWidget3Dとかつくればいい
// 本当の最低限だけでいいや (本当のゲームのライブラリならもっと関数が必要なところだ)
// 音楽も同期をとらなければ
// texture = bindTexture(QImage(":/image.jpg", GL_TEXTURE_2D));
// QMediaPlayer
//0. L/system         ["ls"]            => output ni are
//1. // L/GLShowWindow   [600,400,"title"] => [0|1] (SingleTon) / MainWindow | NULL
//2. //L/GLLoadImage    ["res/chihaya.png"]  => [0|handle]     / GLView
//   L/MPLoadAudio    ["res/arcadia.mp3"]  => [0|mhandle] (SingleTon)
//3. //L/GLDrawImage    [handle,100,200]  => None              / GLView
//   L/MPPlayAudio    [mhandle]         => None
//4. //L/GLProcessAll   []                => 0 | 1             / GLView
//   L/MPGetPosition  [mhandle]         => 100
//N. // L/GLProcessAll   []                => (processMessage & sleep(1) & showbuffer)

//3D memo
//construct : | QGL::DepthBuffer));
//init      : glEnable(GL_DEPTH_TEST);
//resize    : glMatrixMode(GL_PROJECTION);  auto x = GLfloat(width) / height;  glFrustum(-x,x,-1.0,1.0,4.0,15.0);  glMatrixMode(GL_MODELVIEW); //2Dなのでプロジェクションする意味はない
//draw      : glCLear(~ | GL_DEPTH_BUFFER_BIT); glMatrixMode(GL_MODELVIEW);   glLoadIdentity();  glTranslatef(0.0,0.0,-10.0);
