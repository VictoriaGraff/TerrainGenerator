//  Terrain.cpp
//  Victoria Graff
//  001401451
//  graffve


 

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;

/*************** GLOBAL VARIABLES ******************/
GLint window1, window2; //window1 = 3D, window2 = 2D

float** map;    //global height map
int mapSize = 0;    //global size of the map
float** normalx;
float** normaly;
float** normalz;

//camera variables
float camPos[] = {10, 10, 10}; //starting camera position
float Y = 5;    //starting viewing angle
float X = 5;    //starting viewing angle

//booleans for drawing and lighting modes to toggle:
bool triangle = false;
bool quad = true;
bool lighting = true; //lighting starts as on
bool flatshading = false;    //start with flatshading off
bool gouradshading = true; //start with gouradshading on
int wire = 3; //toggle wire viewing if 1=wire, 2=both, 3=surface

/**************** INSTRUCTIONS *********************/

void printInstructions(){
    printf("\n\n\nTerrain Modeling\n");
    printf("by Victoria Graff, 001401451, graffve\n\n");
    printf("INSTRUCTIONS:\n\n");
    printf("            Main window:\n");
    printf("use the arrow keys to rotate the view point\n");
    printf("press 'w' to toggle the wireframe between three options\n");
    printf("press 'l' to toggle the lighting scene\n");
    printf("press 's' to toggle between flat shading and Gouraud shading\n");
    printf("press 'r' to generate a new random terrain\n");
    printf("press 't' to draw the terrain using triangles\n");
    printf("press 'y' to draw the terrain using Quad\n");
    printf("press 'ESC' to quit\n\n");
    printf("            Second Window:\n");
    printf("right click to add a hill to terrain\n");
    printf("left click to do the inverse\n\n\n");
    cout << "Enter the size you wish the terrain map to be (minimum 10): ";    //map size selection
    cin >> mapSize;
}

/**************** CIRCLE ALGORITHM *********************/

void circle(int x, int y, int r, int disp){
    
    //loop through each x in terrain:
    for (int i = 0; i <mapSize; i++){
        //loop through each y in terrain
        for (int j = 0; j <mapSize; j++){
            float distFromCircle = sqrt(pow(x-i,2)+pow(y-j,2));
            float pd = distFromCircle * 2 / (r*2);
            if (fabs(pd) <= 1.0){
                map[i][j] +=  disp/2 + cos(pd*3.14)*disp/2; //writes onto global map
            }
        }
    }
}

/**************** CREATE RANDOM HEIGHT MAP *********************/

//creates map with hill values - global variable map
void createMap(){
    
    map = new float*[mapSize];
    // initializing all values in heightMap to 0
    for (int i = 0; i < mapSize; i++) {
        map[i] = new float[mapSize];
        for (int j = 0; j < mapSize; j++){
            map[i][j] = 0;
        }
    }
    
    //To make the number of hills relevant to the mapSize:
    int hills = rand()%(int)mapSize*3+mapSize;  //random number between the map size and 3 times the map size
    //random hill spots
    for (int h=0; h < hills; h++){  //for each hill
        //x and y coordinate for hill:
        int x = rand()%(mapSize-1); //random # between 0 and mapSize-1
        int y = rand()%(mapSize-1); //random # between 0 and mapSize-1
        int r = rand()%(int)round(mapSize/10)+5; //random radius (between 5 and a tenth of the map size)
        int disp = rand()%5; //random number between 0 and 5
        circle(x, y, r, disp); //circle algorithm
    }
}

/**************** FIND NORMALS *********************/

void getNormals(){
    normalx = new float*[mapSize];
    normaly = new float*[mapSize];
    normalz = new float*[mapSize];
    

    for (int i = 0; i < mapSize; i++) {
        normalx[i] = new float[mapSize];
        normaly[i] = new float[mapSize];
        normalz[i] = new float[mapSize];
        for (int j = 0; j < mapSize; j++){
            
            //edge cases
            if(i==0 | j==0 | i==mapSize-1 | j==mapSize-1){
                normalx[i][j] = 0;
                normaly[i][j] = 0;
                normalz[i][j] = 0;
            }
            
            else{
            //STEP 1: EDGE VECTORS
            float edge1[3];   //first edge vector [i][j] to [i+1][j]
            edge1[0] =  (i+1)-i;                //x
            edge1[1] =  map[i+1][j] - map[i][j]; //y
            edge1[2] =  j-j;                    //z
            float edge2[3];   //second edge vector [i][j] to [i][j-1]
            edge2[0] =  i-i;                    //x
            edge2[1] =  map[i][j-1] - map[i][j]; //y
            edge2[2] =  (j-1)-j;                //z
            float edge3[3];   //third edge vector [i][j] to [i-1][j]
            edge3[0] =  (i-1)-i;                //x
            edge3[1] =  map[i-1][j] - map[i][j]; //y
            edge3[2] =  j-j;                    //z
            float edge4[3];   //fourth edge vector [i][j] to [i][j+1]
            edge4[0] =  i-i;                    //x
            edge4[1] =  map[i][j+1] - map[i][j]; //y
            edge4[2] =  (j+1)-j;                //z
            
            //STEP 2: now cross products:
            float cross1[3];  //cross edge1 and edge2
            cross1[0] = (edge1[1]*edge2[2]) - (edge1[2]*edge2[1]);  //x
            cross1[1] = (edge1[2]*edge2[0]) - (edge1[0]*edge2[2]);  //y
            cross1[2] = (edge1[0]*edge2[1]) - (edge1[1]*edge2[0]);  //z
            float cross2[3];  //cross edge2 and edge3
            cross2[0] = (edge2[1]*edge3[2]) - (edge2[2]*edge3[1]);  //x
            cross2[1] = (edge2[2]*edge3[0]) - (edge2[0]*edge3[2]);  //y
            cross2[2] = (edge2[0]*edge3[1]) - (edge2[1]*edge3[0]);  //z
            float cross3[3];  //cross edge3 and edge4
            cross3[0] = (edge3[1]*edge4[2]) - (edge3[2]*edge4[1]);  //x
            cross3[1] = (edge3[2]*edge4[0]) - (edge3[0]*edge4[2]);  //y
            cross3[2] = (edge3[0]*edge4[1]) - (edge3[1]*edge4[0]);  //z
            float cross4[3];  //cross edge4 and edge1
            cross4[0] = (edge4[1]*edge1[2]) - (edge4[2]*edge1[1]);  //x
            cross4[1] = (edge4[2]*edge1[0]) - (edge4[0]*edge1[2]);  //y
            cross4[2] = (edge4[0]*edge1[1]) - (edge4[1]*edge1[0]);  //z
            
            //STEP 3: normalize
            normalx[i][j] = (cross1[0]+cross2[0]+cross3[0]+cross4[0])/(sqrt(cross1[0]*cross1[0]+cross2[0]*cross2[0]+cross3[0]*cross3[0]));    //x
            normaly[i][j] = (cross1[1]+cross2[1]+cross3[1]+cross4[1])/(sqrt(cross1[1]*cross1[1]+cross2[1]*cross2[1]+cross3[1]*cross3[1]));    //y
            normalz[i][j] = (cross1[2]+cross2[2]+cross3[2]+cross4[2])/(sqrt(cross1[2]*cross1[2]+cross2[2]*cross2[2]+cross3[2]*cross3[2]));    //z
            }
        }
    }
}


/**************** WIREFRAME FUNCTION *********************/

void drawWire(){
    //enable lines mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //drawing with quads
    glBegin(GL_QUADS);
    for (int x = 1; x < mapSize-1; x++) {
        for (int z = 1; z < mapSize-1; z++) {
            glColor3f(0,1,0); //green
            //draw vertices COUNTER CLOCKWISE
            glVertex3f(x, map[x][z], z);
            glVertex3f(x+1, map[x+1][z], z);
            glVertex3f(x+1, map[x+1][z+1], z+1);
            glVertex3f(x, map[x][z+1], z+1);
        }
    }
    glEnd();
}

/**************** DRAW QUAD *********************/

void drawQuad(){
    
    float max = 0;      //FIND MAX HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] > max){
                max=map[i][j];
            }
        }
    }
    float min = 1000;      //FIND MIN HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] < min){
                min=map[i][j];
            }
        }
    }

    getNormals();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_QUADS);
    //for each point within the grid (not the outer perimeter)
    for(int i=1; i<mapSize-1; i++){
        for(int j=1; j<mapSize-1; j++){
            
            //colour calculations
            float col1 = (map[i][j]-min)/(max-min);
            float col2 = (map[i+1][j]-min)/(max-min);
            float col3 = (map[i+1][j+1]-min)/(max-min);
            float col4 = (map[i][j+1]-min)/(max-min);
            
            glColor3f(col1, col1, col1);
            glNormal3f(normalx[i][j], normaly[i][j], normalz[i][j]);
            glVertex3f(i, map[i][j], j);
            
            glColor3f(col2, col2, col2);
            glNormal3f(normalx[i+1][j], normaly[i+1][j], normalz[i+1][j]);
            glVertex3f(i+1, map[i+1][j], j);
            
            glColor3f(col3, col3, col3);
            glNormal3f(normalx[i+1][j+1], normaly[i+1][j+1], normalz[i+1][j+1]);
            glVertex3f(i+1, map[i+1][j+1], j+1);
            
            glColor3f(col4, col4, col4);
            glNormal3f(normalx[i][j+1], normaly[i][j+1], normalz[i][j+1]);
            glVertex3f(i, map[i][j+1], j+1);
        }
        
    }
    glEnd();
}

/**************** DRAW TRIANGLE *********************/

void drawTriangle(){
    
    int max = 0;      //FIND MAX HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] > max){
                max=map[i][j];
            }
        }
    }
    float min = 1000;      //FIND MIN HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] < min){
                min=map[i][j];
            }
        }
    }
    getNormals();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_TRIANGLES);
    for(int i=1; i<mapSize-1; i++){
        for(int j=1; j<mapSize-1; j++){
            
            //colour calculations
            float col1 = (map[i][j]-min)/(max-min);
            float col2 = (map[i+1][j]-min)/(max-min);
            float col3 = (map[i+1][j+1]-min)/(max-min);
            float col4 = (map[i][j+1]-min)/(max-min);
            
            glColor3f(col1, col1, col1);    //bottom left
            glNormal3f(normalx[i][j], normaly[i][j], normalz[i][j]);
            glVertex3f(i, map[i][j], j);
            
            glColor3f(col2, col2, col2);    //top left
            glNormal3f(normalx[i+1][j], normaly[i+1][j], normalz[i+1][j]);
            glVertex3f(i+1, map[i+1][j], j);
            
            glColor3f(col3, col3, col3);    //top right
            glNormal3f(normalx[i+1][j+1], normaly[i+1][j+1], normalz[i+1][j+1]);
            glVertex3f(i+1, map[i+1][j+1], j+1);
            
            glColor3f(col1, col1, col1);    //bottom left
            glNormal3f(normalx[i][j], normaly[i][j], normalz[i][j]);
            glVertex3f(i, map[i][j], j);
            
            glColor3f(col3, col3, col3);    //top right
            glNormal3f(normalx[i+1][j+1], normaly[i+1][j+1], normalz[i+1][j+1]);
            glVertex3f(i+1, map[i+1][j+1], j+1);
            
            glColor3f(col4, col4, col4);    //bottom right
            glNormal3f(normalx[i][j+1], normaly[i][j+1], normalz[i][j+1]);
            glVertex3f(i, map[i][j+1], j+1);
            

        }
    }
    glEnd();
}

/**************** DRAW TERRAIN *********************/

void drawTerrain(){
    
    glPushMatrix();
        //drawing terrain
        if(triangle && !quad){
            drawTriangle();
        }
        if(quad && !triangle){
            drawQuad();
        }
        
        //shading
        if(flatshading){
            glShadeModel(GL_FLAT);
        }
        if(gouradshading){
            glShadeModel(GL_SMOOTH);
        }
    glPopMatrix();
}

/**************** DISPLAY FUNCTION *********************/

void display(void)
{
    //window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //clears screen
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //camera
    gluLookAt(mapSize, mapSize/2, mapSize/2, 0, 0, 0, 0,1,0); //sets camera position
    glRotatef(X,1,0,0);
    glRotatef(Y,0,1,0);
    glTranslatef(-mapSize/2, 0, -mapSize/2);

    //if lighting is on
    if (lighting){
        glEnable(GL_COLOR_MATERIAL);
        float pos1[] = {mapSize,0,mapSize,1.0};
        float amb1[4] = {0,0,0,1};
        float diff1[4] = {1, 1, 1, 1};
        float spec1[4] = {1, 1, 1, 1};
        glPushMatrix();
            glEnable(GL_LIGHTING);
            //turn on light bulb 0
            glEnable(GL_LIGHT0);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diff1);
            glLightfv(GL_LIGHT0, GL_POSITION, pos1);
            glLightfv(GL_LIGHT0, GL_SPECULAR, spec1);
            glLightfv(GL_LIGHT0, GL_AMBIENT, amb1);
        glPopMatrix();
    }
    else{
        glDisable(GL_LIGHTING);
    }
    
    //toggle with wire and surface viewing
    glPushMatrix();
        if(wire==2 | wire == 3){
            drawTerrain();
        }
        glPushMatrix();
            if(wire==1 | wire == 2){
                drawWire();
            }
        glPopMatrix();
    glPopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

/**************** DISPLAY FUNCTION FOR 2D TERRAIN *********************/

void display2(){
    
    int max = 0;      //FIND MAX HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] > max) {
               max=map[i][j];
            }
        }
    }
    float min = 1000;      //FIND MIN HEIGHT for colour calculations
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; j++){
            if (map[i][j] < min){
                min=map[i][j];
            }
        }
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluOrtho2D(0,mapSize*3,mapSize*3,0);
    glMatrixMode(GL_MODELVIEW);
    
    //move to middle
    glTranslatef(mapSize,mapSize,0);
    
    glBegin(GL_POINTS);
    for (int i = 0; i<mapSize; i++){
        for (int j = 0; j<mapSize; j++){
            int y = map[i][j];
            glColor3f((map[i][j]-min)/(max-min),(map[i][j]-min)/(max-min),(map[i][j]-min)/(max-min));
            glVertex2i(i,j);
        }
    }
    glEnd();
    //move back
    glTranslatef(-mapSize,-mapSize,0);
    glutSwapBuffers();
    glutPostRedisplay();
}
/**************** MOUSE FUNCTION FOR 2D TERRAIN *********************/

void mouse2(int button, int state, int x, int z){
    int r = rand()%(int)round(mapSize/10)+5; //random radius (between 5 and a tenth of the map size)
    int disp = rand()%5; //random number between 0 and 5
    x -= round(mapSize);
    z -= round(mapSize);
    
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON){  //if left button clicked
        circle(x,z,r,disp); //add hill to that point
        getNormals();
        glutSetWindow(window1);
        glutPostRedisplay();
    }
    if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON){ //if right button clicked
        circle(x,z,r,-disp); //decrease hill at that point
        getNormals();
        glutSetWindow(window1);
        glutPostRedisplay();
    }
}

/**************** KEYBOARD KEYS *********************/

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:	//27 is the esc key
            exit(0);    //quit the program
            break;
        case 'w':
            //toggle  wireframe  mode  between  three  options
            if(wire==1)wire=2;
            else if(wire==2)wire=3;
            else if(wire==3)wire=1;
            glutPostRedisplay;
            break;
        case 'l':
            //toggle to implement lighting in the scene
            if(lighting == true){
                lighting = false;
            }
            else if(lighting == false){
                lighting = true;
            }
            glutPostRedisplay;
            break;
        case 's':
            //toggle between flat shading and Gouraud shading
            if(flatshading == true && gouradshading == false){
                flatshading = false;
                gouradshading = true;
            }
            else if(flatshading == false && gouradshading == true){
                flatshading = true;
                gouradshading = false;
            }
            glutPostRedisplay;
            break;
        case 'r':
            createMap();
            glutPostRedisplay;
            break;
        case 't':
            //draws the terrain using triangles (strips)
            triangle = true;
            quad = false;
            glutPostRedisplay;
            break;
        case 'y':
            //pressing “y” key draws the same terrain using Quad (strips).
            triangle = false;
            quad = true;
            glutPostRedisplay;
            break;
    }
}
/**************** SPECIAL KEYS *********************/

void special(int key, int x, int y){
    switch (key){
        case GLUT_KEY_RIGHT:
            //rotate screen about y axis
            Y--;
            glutPostRedisplay;
            break;
        case GLUT_KEY_LEFT:
            //rotate screen about y axis
            Y++;
            glutPostRedisplay;
            break;
        case GLUT_KEY_UP:
            //rotate screen about x axis
            X++;
            glutPostRedisplay;
            break;
        case GLUT_KEY_DOWN:
            //rotate screen about x axis
            X--;
            glutPostRedisplay;
            break;
    }
}


/**************** MAIN FUNCTION - PROGRAM ENTRY POINT *********************/

int main(int argc, char **argv) {
    printInstructions();
    
    glutInit(&argc, argv); //starts up GLUT
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    //2nd window for 2D map display
    glutInitWindowSize(mapSize*3,mapSize*3);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(600, 50);
    window2 = glutCreateWindow("2D Terrain");
    glutSetWindow(window2);
    glutDisplayFunc(display2);
    glutMouseFunc(mouse2);
    
    //1st Window for Terrain
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    window1 = glutCreateWindow("Terrain Modeling");
    glutSetWindow(window1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,1,1,800);
    glClearColor(0, 0, 0, 0);
    createMap();    //create initial map
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    
    glutMainLoop();
    return(0);					//return may not be necessary on all compilers
}
