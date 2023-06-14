#include <cmath>
#include <SDL2/SDL_opengl.h>
#include <GL/GLU.h>
#include "forms.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void Form::update(double delta_t)
{
    // Nothing to do here, animation update is done in child class method
}


void Form::render()
{
    // Point of view for rendering
    // Common for all Forms
    Point org = anim.getPos();
    glColor3f(col.r, col.g, col.b);
    glTranslated(org.x, org.y, org.z);
    //glRotated(getAnim().getPhi(), 1, 0, 0);
}


Sphere::Sphere(double r, Color cl)
{
    radius = r;
    col = cl;
}




void Sphere::update(double delta_t) {
    double dim = 1.0;
    double waterLevel = 0.5 * dim;
    double sphereBottom = this->anim.getPos().y - this->radius;

    // Calcul de la distance entre le centre de la sphère et la surface
    double distance = this->anim.getPos().y - (waterLevel + this->radius);

    // Vérifier si la sphère est au-dessus de la surface de l'eau
    if (sphereBottom > waterLevel) {
        // La sphère est en train de chuter, mettre à jour la position et la vitesse comme précédemment
        Point ptM = this->anim.getPos();
        Vector OM(Point(0, 0, 0), ptM);
        Vector vit;
        Vector g(0, -9.81, 0);
        vit = this->anim.getSpeed() + delta_t * g;
        this->anim.setSpeed(vit);
        OM = OM + delta_t * this->anim.getSpeed();
        ptM = Point(OM.x, OM.y, OM.z);
        this->anim.setPos(ptM);
    } else if (distance <= 0) {
        // La sphère touche la surface de l'eau, arrêter la chute en fixant la vitesse à zéro
        this->anim.setSpeed(Vector(0, 0, 0));
    }
}



void Sphere::rotate()
{
    //glTranslated(1,1,1);
    glRotated(this->anim.getPhi(), 0, 1, 0) ; //Rotation sur y
    glRotated(this->anim.getTheta(), 1, 0, 0) ; //Rotation sur x
}

void Sphere::translation(int axe)
{
    //x
    if (axe == 1){
        glTranslated(this->anim.getSpeed().x,1,0);
    }

    //y
    else if (axe == 2){
        glTranslated(0.5,this->anim.getSpeed().x,0.5);
    }

    //z
    else if (axe == 3){
        glTranslated(0,0,this->anim.getSpeed().x);
    }
}


void Sphere::render()
{
    GLUquadric *quad;

    quad = gluNewQuadric();

    // Complete this part
    Form::render(); //Comme pour Cube_face render, on appelle Form:render

    this->translation(2) ;
    //this->rotate() ;
    gluSphere(quad,radius,10,10);
    //par3 et 4 = nombre de cotés de la forme

    gluDeleteQuadric(quad);

}


Cube_face::Cube_face(Vector v1, Vector v2, Point org, double l, double w, Color cl)
{
    vdir1 = 1.0 / v1.norm() * v1;
    vdir2 = 1.0 / v2.norm() * v2;
    anim.setPos(org);
    length = l;
    width = w;
    col = cl;
}


void Cube_face::update(double delta_t)
{
    // Complete this part
    this->anim.setPhi(this->anim.getPhi() + 1);

}


void Cube_face::render()
{
    Point p1 = Point();
    Point p2 = p1, p3, p4 = p1;
    p2.translate(length*vdir1);
    p3 = p2;
    p3.translate(width*vdir2);
    p4.translate(width*vdir2);

    Form::render();

    glBegin(GL_QUADS);
    {
        glVertex3d(p1.x, p1.y, p1.z);
        glVertex3d(p2.x, p2.y, p2.z);
        glVertex3d(p3.x, p3.y, p3.z);
        glVertex3d(p4.x, p4.y, p4.z);
    }
    glEnd();
}

Surface::Surface(GLfloat *points, int nbPointsX, int nbPointsZ)
{
    //Allocate 3d array of correct size
    GLfloat *ctrlPoints = new GLfloat[nbPointsX*nbPointsZ*3];
    // Store pointer in object
    this->ctrlPoints = ctrlPoints;

    // Copying array
    std::copy(points, points + nbPointsX * nbPointsZ * 3, ctrlPoints);

    std::cout<<"Points : \n";
    for (int i = 0; i < 108; i++) {
        std::cout<<i<<" -> "<<points[i]<<"\n";
    }

    std::cout<<"ctrlPoints : \n";
    for (int i = 0; i < 108; i++) {
        std::cout<<i<<" -> "<<ctrlPoints[i]<<"\n";
    }

    this->nbPointsX = nbPointsX;
    this->nbPointsZ = nbPointsZ;

    this->nbNoeudsX = nbPointsX * 2;
    this->nbNoeudsZ = nbPointsZ * 2;

    int degreX = nbPointsX;
    int degreZ = nbPointsZ;
//
//     influence parameter setting of each control point
//     Les valeurs des noeuds doivent alterner au minimum tout les degrés fois
    GLfloat *noeudsX = new GLfloat[nbNoeudsX];
    {
        int k = 0;
        int j = 0;
        for(int h = 0; k < nbNoeudsX; h++) {
            for(int i = 0; i < degreX; i++) {
                j = 0;
                noeudsX[k] = h;
                k = k+1;
            }
            j=j+1;
        }
    }
    this->NoeudsX = noeudsX;

    GLfloat *noeudsZ = new GLfloat[nbNoeudsZ];
    {
        int k = 0;
        int j = 0;
        for(int h = 0; k < nbNoeudsZ; h++) {
            for(int i = 0; i < degreZ; i++) {
                j = 0;
                noeudsZ[k] = h;
                k = k+1;
            }
            j=j+1;
        }
    }
    this->NoeudsZ = noeudsZ;

}


void Surface::update(double delta_t)
{
}


void Surface::render()
{

// Turn on the automatic method vector switch
    glEnable(GL_AUTO_NORMAL);
// Allow regularization vector
    glEnable(GL_NORMALIZE);
    GLUnurbsObj *theNurb;
    theNurb = gluNewNurbsRenderer(); // Create a NURBS surface object

// Modify the properties of NURBS surface objects-glu library function

// Sampling fault tolerance tolerance

    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 50);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);

//    std::cout<<"Nb noeuds X : "<<nbNoeudsX<<"\n";
//    std::cout<<"Nb noeuds Z : "<<nbNoeudsZ<<"\n";
//    std::cout<<"Nb pts X : "<<nbPtsCtrlX<<"\n";
//    std::cout<<"Nb pts Z : "<<nbPtsCtrlZ<<"\n";

    //glColor3f( 1.0, 1.0, 1.0 );
    gluBeginSurface(theNurb); // Start surface drawing
    gluNurbsSurface(theNurb, nbNoeudsX, NoeudsX, nbNoeudsZ, NoeudsZ, nbPointsX * 3, 3, ctrlPoints, nbPointsX, nbPointsZ, GL_MAP2_VERTEX_3); // Define the surface Mathematical model to determine its shape
    Form::render();
    gluEndSurface(theNurb); // End surface drawing

}
