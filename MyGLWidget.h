#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event
    // corresponent de ratolí
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);

  private:
    void iniEscena ();
    void iniCamera ();
    void creaBuffersPatricio ();
    void creaBuffersTerra ();
    void carregaShaders ();
    void viewTransform ();
    void projectTransform ();
    void patricioTransform(int numPatricio);
    void terraTransform();
    void calculaCapsaModel ();

    
    // attribute locations
    GLuint vertexLoc, colorLoc;

    // uniform locations
    GLuint transLoc, viewLoc, projLoc;

    // VAO i VBO names
    GLuint VAO_Patricio, VAO_Terra;

    // Program
    QOpenGLShaderProgram *program;

    // Viewport
    GLint ample, alt;

    // Mouse interaction
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    typedef enum {PERSP, ORTHO} TypeCam;
    TypeCam Camera = PERSP;
    //XCLOCK Y YCLICK NO LO USO!
    int xClick, yClick;
    float psi = 45.0;
    float theta = 45.0;
    float psiAnt,thetaAnt;
    // Internal vars
    float radiEscena;
    float fov, ra, znear, zfar;
    glm::vec3 obs, vrp, up;
    float midaTerra;

    //modificaciones patricio
    Model modelPatricio;
    float rotPatricio = 360.0;
    int totalPatricios;
    float bailePatricio = 0.0;
    float zPatricio;
    //Calculo esfera contenidora Patricio
    float r = 0.0;
    GLfloat alturaPatricio;
    glm::vec3 centroPatricio;
    glm::vec3 puntMax, puntMin, puntAvg;
    glm::vec3 base;
};

