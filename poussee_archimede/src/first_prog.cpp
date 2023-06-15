// Using SDL, SDL OpenGL and standard IO
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/GLU.h>

// Module for space geometry
#include "geometry.h"
// Module for generating and rendering forms
#include "forms.h"


/***************************************************************************/
/* Constants and functions declarations                                    */
/***************************************************************************/
// Screen dimension constants
const int SCREEN_WIDTH = 950;
const int SCREEN_HEIGHT = 750;

// Max number of forms : static allocation
const int MAX_FORMS_NUMBER = 11;

// Animation actualization delay (in ms) => 100 updates per second
const Uint32 ANIM_DELAY = 10;


// Starts up SDL, creates window, and initializes OpenGL
bool init(SDL_Window** window, SDL_GLContext* context);

// Initializes matrices and clear color
bool initGL();

// Updating forms for animation
void update(Form* formlist[MAX_FORMS_NUMBER], double delta_t);

// Renders scene to the screen
void render(Form* formlist[MAX_FORMS_NUMBER], const Point &cam_pos);

// Frees media and shuts down SDL
void close(SDL_Window** window);


/***************************************************************************/
/* Functions implementations                                               */
/***************************************************************************/
bool init(SDL_Window** window, SDL_GLContext* context)
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        // Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // Create window
        *window = SDL_CreateWindow( "TP intro OpenGL / SDL 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
        if( *window == NULL )
        {
            std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
            success = false;
        }
        else
        {
            // Create context
            *context = SDL_GL_CreateContext(*window);
            if( *context == NULL )
            {
                std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                // Use Vsync
                if(SDL_GL_SetSwapInterval(1) < 0)
                {
                    std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
                }

                // Initialize OpenGL
                if( !initGL() )
                {
                    std::cout << "Unable to initialize OpenGL!"  << std::endl;
                    success = false;
                }
            }
        }
    }

    return success;
}


bool initGL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;

    // Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the viewport : use all the window to display the rendered scene
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Fix aspect ratio and depth clipping planes
    gluPerspective(40.0, (GLdouble)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0, 100.0);


    // Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Initialize clear color : black with no transparency
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

    // Activate Z-Buffer
    glEnable(GL_DEPTH_TEST);


    // Lighting basic configuration and activation
    const GLfloat light_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


    // Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        std::cout << "Error initializing OpenGL!  " << gluErrorString( error ) << std::endl;
        success = false;
    }

    return success;
}

void update(Form* formlist[MAX_FORMS_NUMBER], double delta_t)
{
    // Update the list of forms
    unsigned short i = 0;
    while(formlist[i] != NULL)
    {
        formlist[i]->update(delta_t);
        i++;
    }
}

void render(Form* formlist[MAX_FORMS_NUMBER], const Point &cam_pos, double deg)
{
    // Clear color buffer and Z-Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Set the camera position and parameters
        gluLookAt(  cam_pos.x, cam_pos.y, cam_pos.z,
                    0, 0, 0,
                    0.0f, 1.0f,  0.0f);
    // Isometric view
    glRotated(deg, 0, 1, 0);
    glRotated(30, 1, 0, -1);

    // X, Y and Z axis
    glPushMatrix(); // Preserve the camera viewing point for further forms
    // Render the coordinates system
    glBegin(GL_LINES);
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(1, 0, 0);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 1, 0);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3i(0, 0, 0);
        glVertex3i(0, 0, 1);
    }
    glEnd();
    glPopMatrix(); // Restore the camera viewing point for next object

    // Render the list of forms
    unsigned short i = 0;
    while(formlist[i] != NULL)
    {
        glPushMatrix(); // Preserve the camera viewing point for further forms
        formlist[i]->render();
        glPopMatrix(); // Restore the camera viewing point for next object
        i++;
    }
}

void close(SDL_Window** window)
{
    //Destroy window
    SDL_DestroyWindow(*window);
    *window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}
// Position du cube flottant dans l'eau
float cube_x = 0.0f;  // Coordonnée x du cube
float cube_y = -2.0f; // Coordonnée y du cube (hauteur de flottaison)
float cube_z = 0.0f;  // Coordonnée z du cube

class FallingObject : public Form
{
private:
    Vector acceleration;
    Vector velocity;
    Sphere* sphere; // Pointeur vers la sphère associée à l'objet

public:
    FallingObject(Point p, Vector accel, Vector speed, double radius, Color col)
    {
        anim.setPos(p);
        acceleration = accel;
        velocity = speed;
        sphere = new Sphere(radius, col);
    }

    void update(double delta_t) override
    {
        // Mettez à jour les valeurs de position et de vitesse en fonction de l'accélération
        Vector delta_v = acceleration.integral(delta_t);
        anim.setPos(anim.getPos() + velocity.integral(delta_t) + 0.5 * delta_v);anim.setPos(anim.getPos() + velocity.integral(delta_t) + Vector(0.5 * delta_v.x, 0.5 * delta_v.y, 0.5 * delta_v.z));
        velocity += delta_v;

        // Vérifiez si l'objet est immergé dans l'eau
        if (anim.getPos().y < 0)
        {
            // Calculez la force de poussée d'Archimède
            double submerged_volume = 4 / 3 * M_PI * pow(sphere->getRadius(), 3);
            Vector buoyant_force = submerged_volume * Vector(0, 1, 0); // Supposons que la densité de l'eau est 1 et la gravité est vers le bas

            // Ajustez l'accélération en ajoutant la force de poussée d'Archimède
            acceleration += buoyant_force;
        }
    }

    void render() override
    {
        sphere->render();
    }

    ~FallingObject()
    {
        delete sphere;
    }
};




/***************************************************************************/
/* MAIN Function                                                           */
/***************************************************************************/
int main(int argc, char* args[])
{
    // The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    // OpenGL context
    SDL_GLContext gContext;



    // Start up SDL and create window
    if( !init(&gWindow, &gContext))
    {
        std::cout << "Failed to initialize!" << std::endl;
    }
    else
    {
        // Main loop flag
        bool quit = false;
        Uint32 current_time, previous_time, elapsed_time;

        // Event handler
        SDL_Event event;

        // Camera position
        double xcam = 0;
        double ycam = 0;
        double zcam = 5;

        float vitesse = 0.1f; // vitesse de deplacement de la cam�ra

        // angle de rotation pour la direction de la cam�ra
        float angle=0.0;
        // vecteur repr�sentant la direction de la cam�ra
        float lx=0.0f,lz=-1.0f;

        double rho = -45;
        Point camera_position(xcam, ycam, zcam);

        // The forms to render
        Form* forms_list[MAX_FORMS_NUMBER];
        unsigned short number_of_forms = 0, i;
        for (i=0; i<MAX_FORMS_NUMBER; i++)
        {
            forms_list[i] = NULL;
        }

        // Create here specific forms and add them to the list...
        // Don't forget to update the actual number_of_forms !
          Cube_face *pFace = NULL;
//        pFace = new Cube_face(Vector(1,0,0), Vector(0,1,0), Point(-0.5, -0.5, -0.5), 1, 1, ORANGE);
//        forms_list[number_of_forms] = pFace;
//        number_of_forms++;
//
//        Cube_face *pFace2 = NULL;
//        pFace2 = new Cube_face(Vector(0,1,0), Vector(0,0,1), Point(-0.5, -0.5, -0.5), 1, 1, BLUE);
//        forms_list[number_of_forms] = pFace2;
//        number_of_forms++;
//
//        Cube_face *pFace3 = NULL;
//        pFace3 = new Cube_face(Vector(1,0,0), Vector(0,0,1), Point(-0.5, -0.5, -0.5), 1, 1, RED);
//        forms_list[number_of_forms] = pFace3;
//        number_of_forms++;
//
//        Cube_face *pFace4 = NULL;
//        pFace4 = new Cube_face(Vector(0,-1,0), Vector(0,0,-1), Point(0.5, 0.5, 0.5), 1, 1, GREEN);
//        forms_list[number_of_forms] = pFace4;
//        number_of_forms++;
//
//        Cube_face *pFace5 = NULL;
//        pFace5 = new Cube_face(Vector(-1,0,0), Vector(0,0,-1), Point(0.5, 0.5, 0.5), 1, 1, WHITE);
//        forms_list[number_of_forms] = pFace5;
//        number_of_forms++;
//
//        Cube_face *pFace6 = NULL;
//        pFace6 = new Cube_face(Vector(-1,0,0), Vector(0,-1,0), Point(0.5, 0.5, 0.5), 1, 1, YELLOW);
//        forms_list[number_of_forms] = pFace6;
//        number_of_forms++;

        // Cube_face *pFace = NULL;
        // pFace = new Cube_face(Vector(1,0,0), Vector(0,1,0), Point(0.5, 0, 0.5), 1, 1, ORANGE);
        // forms_list[number_of_forms] = pFace;
        // number_of_forms++;

//        Sphere *pSphere1 = NULL;
//        pSphere1 = new Sphere(1, BLUE);
//        forms_list[number_of_forms] = pSphere1;
//        number_of_forms++;

        double agr = 1;
        // arrière
        pFace = new Cube_face(Vector(1,0,0), Vector(0,1,0), Point(-0.5*agr, -0.5*agr, -0.5*agr), 1*agr, 1.2*agr, WHITE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;
         //coté gauche
        pFace = new Cube_face(Vector(0,0,1), Vector(0,1,0), Point(-0.5*agr, -0.5*agr, -0.5*agr), 1*agr, 1.2*agr, WHITE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;
        // sol
        pFace = new Cube_face(Vector(1,0,0), Vector(0,0,1), Point(-0.5*agr, -0.5*agr, -0.5*agr), 1*agr, 1*agr, WHITE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;
        // coté droit
        pFace = new Cube_face(Vector(0,0,1), Vector(0,1,0), Point(0.5*agr, -0.5*agr, -0.5*agr), 1*agr, 1.2*agr, WHITE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;
//         // coté HAUT pour l'eau
        pFace = new Cube_face(Vector(1,0,0), Vector(0,0,1), Point(-0.5*agr, 0.5*agr, -0.5*agr), 1*agr, 1*agr, LIGHT_BLUE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;
         // AVANT POUR L'eau
        pFace = new Cube_face(Vector(1,0,0), Vector(0,1,0), Point(-0.5*agr, -0.5*agr, 0.5*agr), 1*agr, 1*agr, LIGHT_BLUE);
        forms_list[number_of_forms] = pFace;
        number_of_forms++;



         // Création de deux sphères
        Sphere* sphere1 = new Sphere(0.25, YELLOW); // Réduction de moitié du rayon
        sphere1->getAnim().setPos(Point(-0.5, 6, -0.5));
        //sphere1->getAnim().setPos(Point(0.5, 0.5, 0.5));
        forms_list[number_of_forms] = sphere1;
        number_of_forms++;


        int nbPtsCtrlX = 6;
        int nbPtsCtrlZ = 6;

        GLfloat *ctrlPoints = new GLfloat[nbPtsCtrlX*nbPtsCtrlZ*3];

        GLfloat d = 0;
        for (int i = 0; i < nbPtsCtrlX; i++) {
            GLfloat c = 0;
            for (int j = 0; j < nbPtsCtrlZ; j++) {
                ctrlPoints[(i*6*3)+j*3+0] = c;
                ctrlPoints[(i*6*3)+j*3+1] = 0;
                ctrlPoints[(i*6*3)+j*3+2] = d;
                c++;
            }
            d++;
        }

        ctrlPoints[(3*6*3)+3*3+1] = 10;

//    std::cout<<"ctrlPoints : \n";
//    for (int i = 0; i < 108; i++) {
//        std::cout<<i<<" -> "<<ctrlPoints[i]<<"\n";
//    }


//        Surface *pSurface = NULL;
//        pSurface = new Surface(ctrlPoints, nbPtsCtrlX, nbPtsCtrlZ);
//        forms_list[number_of_forms] = pSurface;
//        number_of_forms++;

        // Get first "current time"
        previous_time = SDL_GetTicks();
        // While application is running
        while(!quit)
        {
            // Handle events on queue
            while(SDL_PollEvent(&event) != 0)
            {
                int x = 0, y = 0;
                SDL_Keycode key_pressed = event.key.keysym.sym;

                switch(event.type)
                {
                // User requests quit
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    // Handle key pressed with current mouse position
                    SDL_GetMouseState( &x, &y );

                    switch(key_pressed)
                    {
                    // Quit the program when 'q' or Escape keys are pressed

                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_UP :
                        xcam += lx * vitesse;
                        zcam += lz * vitesse;
                        break;

                    case SDLK_DOWN :
                        xcam -= lx * vitesse;
                        zcam -= lz * vitesse;
                        break;

                    case SDLK_o:
                        rho += 5;
                        std::cout<< "Pos Cam :  "<<xcam <<" "<< ycam <<" "<< zcam<<"\n";
                        break;

                    case SDLK_p:
                        rho -= 5;
                        std::cout<< "Pos Cam :  "<<xcam <<" "<< ycam <<" "<< zcam<<"\n";
                        break;

                    case SDLK_z:
                        ycam += 0.5;
                        std::cout<< "Pos Cam :  "<<xcam <<" "<< ycam <<" "<< zcam<<"\n";
                        break;

                    case SDLK_s:
                        ycam -= 0.5;
                        std::cout<< "Pos Cam :  "<<xcam <<" "<< ycam <<" "<< zcam<<"\n";
                        break;
                    case SDLK_u:
                        xcam = cube_x;// Position x de la caméra alignée avec le cube
                        break;

                    case SDLK_i:
                        ycam = cube_y + 5.0f; // Position y de la caméra au-dessus du cube
                        break;

                   case SDLK_j://zoom -
                        zcam = cube_z - 10.0f; // Position z de la caméra derrière le cube
                        break;

                 case SDLK_m:
                      rho = 0.0f;// Angle de rotation autour de l'axe vertical désactivé
                        break;


                    case SDLK_r:
                        ycam = 0;
                        xcam = 0;
                        zcam = 5;
                        rho = -45;
                        break;

                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
            }

            // Update the scene
            current_time = SDL_GetTicks(); // get the elapsed time from SDL initialization (ms)
            elapsed_time = current_time - previous_time;
            if (elapsed_time > ANIM_DELAY)
            {
                previous_time = current_time;
                update(forms_list, 1e-3 * elapsed_time); // International system units : seconds
            }

            // Render the scene
             camera_position = Point(xcam, ycam, zcam);
             render(forms_list, camera_position, rho);


            // Update window screen
            SDL_GL_SwapWindow(gWindow);
        }
    }

    // Free resources and close SDL
    close(&gWindow);

    return 0;
}
