#ifndef FORMS_H_INCLUDED
#define FORMS_H_INCLUDED
#include "geometry.h"
#include "animation.h"



class Color
{
public:
    float r, g, b, t;
    Color(float rr = 1.0f, float gg = 1.0f, float bb = 1.0f, float tt = 0.0f) {r=rr; g=gg; b=bb;t=tt;}
};

// Constant Colors
const Color RED(1.0f, 0.0f, 0.0f);
const Color BLUE(0.0f, 0.0f, 1.0f);
const Color BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const Color LIGHT_BLUE(0.0f, 1.0f, 0.7f,0.5f );
const Color GREEN(0.0f, 1.0f, 0.0f);
const Color YELLOW(1.0f, 1.0f, 0.0f);
const Color WHITE(1.0f, 1.0f, 1.0f,1.0f);
const Color ORANGE(1.0f, 0.65f, 0.0f);
const Color WATER_TRANSPARENT(0.0f, 1.0f, 1.0f, 0.5f);
const Color DARK_BLUE_TRANSPARENT(0.0f, 0.7f, 1.0f, 0.5f);








// Generic class to render and animate an object
class Form
{
protected:
    Color col;
    Animation anim;
public:
    Animation& getAnim() {return anim;}
    void setAnim(Animation ani) {anim = ani;}
    // This method should update the anim object with the corresponding physical model
    // It has to be done in each inherited class, otherwise all forms will have the same movements !
    // Virtual method for dynamic function call
    // Pure virtual to ensure all objects have their physics implemented
    virtual void update(double delta_t) = 0;
    // Virtual method : Form is a generic type, only setting color and reference position
    virtual void render();
};


// A particular Form
class Sphere : public Form {
    private:
        // The sphere center is aligned with the coordinate system origin
        // => no center requirepd here, information is stored in the anim object
        double radius; //radius = rayon
    public:
        Sphere(double r = 1.0, Color cl = Color());
        double getRadius() const {return radius;}
        void setRadius(double r) {radius = r;}
        void update(double delta_t);
        double getVolume();
        double getDensity();
        double getMass();
        void render();
        void rotate() ;
        void translation(int x) ;
        void setWater(double width, double height, double depth, double density);
};


// A face of a cube
class Cube_face : public Form
{
private:
    Vector vdir1, vdir2;
    double length, width;
public:
    Cube_face(Vector v1 = Vector(1,0,0), Vector v2 = Vector(0,0,1),
          Point org = Point(), double l = 1.0, double w = 1.0,
          Color cl = Color());
    void update(double delta_t);
    void render();
};

class Surface : public Form
{
private:
    GLfloat *ctrlPoints;
    int nbPointsX;
    int nbPointsZ;
    GLfloat *NoeudsX;
    int nbNoeudsX;
    GLfloat *NoeudsZ;
    int nbNoeudsZ;
public:
    Surface(GLfloat *ctrlPoints, int nbPointsX, int nbPointsZ);
    void update(double delta_t);
    void render();
};


#endif // FORMS_H_INCLUDED
