#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  setMouseTracking(true);
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersPatricio();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
}

void MyGLWidget::iniEscena()
{
  midaTerra = 50.0;
  totalPatricios = 10;
  zPatricio = -10.0;
}

void MyGLWidget::iniCamera(){

  obs = glm::vec3(0, 6, 6);
  vrp = glm::vec3(0, 0, 0.5);
  up = glm::vec3(0, 1, 0);
  fov = float(M_PI)/4.0f;
  ra  = 1.0;
  znear = r;
  zfar  = 3*r;

  viewTransform();
  projectTransform();
}

void MyGLWidget::paintGL ()
{
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  viewTransform();
  projectTransform();

  // Patricio
  for(int i = 0;i<totalPatricios;i++){
      patricioTransform(i);
      glBindVertexArray (VAO_Patricio);
      glDrawArrays(GL_TRIANGLES, 0, modelPatricio.faces().size()*3);
      glBindVertexArray (0);
  }

  // Terra
  terraTransform();
  glBindVertexArray (VAO_Terra);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray (0);
}

void MyGLWidget::patricioTransform (int numPatricio)
{
  // Matriu de transformació de Patricio

  glm::mat4 TG(1.0f);
  TG = glm::rotate(TG, float(glm::radians((rotPatricio/totalPatricios)*numPatricio)),glm::vec3(0,1,0));
  TG = glm::translate(TG, glm::vec3(0.0,0.0,zPatricio));
  TG = glm::scale(TG, glm::vec3(6.0/alturaPatricio));
  TG = glm::rotate(TG,float(glm::radians(bailePatricio)),glm::vec3(0.0,1.0,0.0));
  TG = glm::translate(TG, -base);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::terraTransform ()
{
  // Matriu de transformació del terra
  glm::mat4 TG(1.0f);
  TG= glm::translate(TG, glm::vec3(-25,0.0,-25));
  TG = glm::scale(TG, glm::vec3(midaTerra, 1.0, midaTerra));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
    if(Camera == PERSP){
        glm::mat4 Proj(1.0f);
        Proj = glm::perspective (fov, ra, znear, zfar);
        glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);

    }
    else{
        glm::mat4 Proj = glm::ortho(-r, r, r, -r,znear,zfar);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);

    }
}

void MyGLWidget::viewTransform ()
{
    if(Camera == ORTHO){
        glm::mat4 View = glm::lookAt (
                    glm::vec3(puntAvg[0],puntAvg[1],puntAvg[2]+2*r),
                    puntAvg,
                    glm::vec3(0,-1,0));

        View = glm::rotate(View,float(glm::radians(90.0)),glm::vec3(1,0,0));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE,&View[0][0]);
    }
    else{

      glm::mat4 View (1.0f);
      View = glm::translate(View,glm::vec3(0.0,0.0,-(2*r)));
      View = glm::rotate(View,float(glm::radians(theta)),glm::vec3(1.0,0.0,0.0));
      View = glm::rotate(View,float(glm::radians(-psi)),glm::vec3(0.0,1.0,0.0));
      View = glm::translate(View,-puntAvg);
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
    }
}


void MyGLWidget::resizeGL (int w, int h) 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  w = vp[2];
  h = vp[3];
#endif/*
  ample = w;
  alt = h;
  ra = float(ample)/float(alt);
  projectTransform();*/

  ample = w;
  alt = h;

  ra = float (w) / float (h);

  if(ra < 1){
      fov = 2*atan(tan(glm::asin((r /(2*r))))/ra);
  }
  else fov = glm::asin((r /(2*r)))*2;

}

void MyGLWidget::creaBuffersTerra ()
{
  // Pla quadrat pel terra
  glm::vec3 geomTerra[6] = {
      glm::vec3(0.0, 0.0, 0.0),
      glm::vec3(1.0, 0.0, 0.0),
      glm::vec3(1.0, 0.0, 1.0),
      glm::vec3(0.0, 0.0, 0.0),
      glm::vec3(1.0, 0.0, 1.0),
      glm::vec3(0.0, 0.0, 1.0)
  };

  glm::vec3 c(0.8, 0.8, 0.8);
  glm::vec3 colTerra[6] = { c, c, c, c, c, c };

  // VAO
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(geomTerra), geomTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  glBindVertexArray (0);
}

void MyGLWidget::creaBuffersPatricio ()
{
    modelPatricio.load("./models/Patricio.obj");
    calculaCapsaModel();
    modelPatricio.VBO_vertices();
    modelPatricio.VBO_matdiff();

    glGenVertexArrays(1, &VAO_Patricio);
    glBindVertexArray(VAO_Patricio);

    GLuint VBO_patricio[2];
    glGenBuffers(2, VBO_patricio);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_patricio[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * modelPatricio.faces().size()*3*3, modelPatricio.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_patricio[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * modelPatricio.faces().size()*3*3, modelPatricio.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glDrawArrays(GL_TRIANGLES, 0, modelPatricio.faces().size() * 3);


    glBindVertexArray (0);

}



void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
}

void MyGLWidget::calculaCapsaModel ()
{

    GLfloat
      min_x, max_x,
      min_y, max_y,
      min_z, max_z;

    min_x = max_x = modelPatricio.vertices()[0];
    min_y = max_y = modelPatricio.vertices()[1];
    min_z = max_z = modelPatricio.vertices()[2];
    //recorrer los vertices del patricio para saber su minimo y maximo en x y z
    for (unsigned int i = 0; i < (modelPatricio.vertices().size())-3; i+=3) {
      if (modelPatricio.vertices()[i] < min_x) min_x = modelPatricio.vertices()[i];
      if (modelPatricio.vertices()[i] > max_x) max_x = modelPatricio.vertices()[i];
      if (modelPatricio.vertices()[i+1] < min_y) min_y = modelPatricio.vertices()[i+1];
      if (modelPatricio.vertices()[i+1] > max_y) max_y = modelPatricio.vertices()[i+1];
      if (modelPatricio.vertices()[i+2] < min_z) min_z = modelPatricio.vertices()[i+2];
      if (modelPatricio.vertices()[i+2] > max_z) max_z = modelPatricio.vertices()[i+2];
    }

    puntMax = glm::vec3(max_x,max_y,max_z);
    puntMin = glm::vec3(min_x,min_y,min_z);
    puntAvg = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
    //nos guardamos el centro del patricio antes de actualizarlo junto con el suelo (no lo uso en esta practica, uso la base)
    centroPatricio = puntAvg;
    //radio de la esfera contenedora del patricio
    r = glm::distance(puntMax,puntAvg);
    std::cout<<"puntMax = "<<puntMax[0]<<" "<<puntMax[1]<<" "<<puntMax[2]<<std::endl;
    std::cout<<"puntMin = "<<puntMin[0]<<" "<<puntMin[1]<<" "<<puntMin[2]<<std::endl;
    std::cout<<"puntAvg = "<<puntAvg[0]<<" "<<puntAvg[1]<<" "<<puntAvg[2]<<std::endl;
    std::cout<<"r = "<<r<<std::endl;
    //base es igual a la base del patricio
    base = glm::vec3(puntAvg.x,puntMin.y,puntAvg.z);
    std::cout<<"base = "<<base[0]<<" "<<base[1]<<" "<<base[2]<<std::endl;
    //alturaPatricio es lo que mide el patricio
    alturaPatricio = puntMax.y - puntMin.y;
    //esfera contenedora del suelo + altura patricio
    puntMin = glm::vec3(-25.0,0.0,-25.0);
    puntMax = glm::vec3(25.0,6.0,25.0);
    //punto medio de la esfera contenedora
    puntAvg = glm::vec3((puntMin[0]+puntMax[0])/2, (puntMin[1]+puntMax[1])/2, (puntMin[2]+puntMax[2])/2);
    //radio final de la esfera contenedora
    r = glm::distance(puntMax,puntAvg);
    std::cout<<"r = "<<r<<std::endl;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
      case Qt::Key_Plus: {
        if(totalPatricios != 12) totalPatricios += 1;
        break;
      }
      case Qt::Key_Minus: {
        if(totalPatricios != 1) totalPatricios -= 1;
        break;
      }
      case Qt::Key_Left: {
        bailePatricio += 5.0;
        break;
      }
      case Qt::Key_Right: {
        bailePatricio -= 5.0;
        break;
      }
      case Qt::Key_Up: {
        if(zPatricio != -5.0) zPatricio += 5.0;
        break;
      }
      case Qt::Key_Down: {
        if(zPatricio != -20.0) zPatricio -= 5.0;
        break;
      }
      case Qt::Key_C:{
        if(Camera == PERSP) Camera = ORTHO;
        else if(Camera == ORTHO) Camera = PERSP;
        break;
      }
      case Qt::Key_I:{
        theta = 45.0;
        psi = 45.0;
        totalPatricios = 10;
        bailePatricio = 0.0;
        zPatricio = -10;
        Camera = PERSP;
        break;
      }
      default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  if(Camera == PERSP){
    psiAnt = e->x();
    thetaAnt = e->y();
  }

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  if(DoingInteractive == ROTATE && Camera == PERSP){
      makeCurrent();

      theta -= (e->y() - thetaAnt)*(float(ample)/360.0);
      psi -= (e->x() - psiAnt)*(float(alt)/360.0);
      thetaAnt = e->y();
      psiAnt = e->x();
      update();
  }
}
