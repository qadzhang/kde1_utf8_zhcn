#ifndef GLLANDSCAPE_H
#define GLLANDSCAPE_H

#include <ntqgl.h>

class GLLandscape : public TQGLWidget
{
    TQ_OBJECT

public:
    GLLandscape( TQWidget * parent = 0, const char * name = 0 );
    ~GLLandscape();
    
public slots:
    void rotateX( int );
    void rotateY( int );
    void rotateZ( int );
    void zoom( int );
    void fractalize();
    void resetGrid();
    
    void setWireframe( int );
    void setFilled( int );
    void setSmoothShaded( int );
    void setLandscape( int );
    void setGridSize( int );
    
    void toggleWaveAnimation( bool );

signals:
    void rotatedX( int );
    void rotatedY( int );
    void rotatedZ( int );
    
protected:
    void paintGL();
    void initializeGL();
    void resizeGL( int w, int h );
    void mousePressEvent( TQMouseEvent * );
    void mouseReleaseEvent( TQMouseEvent * );
    void mouseMoveEvent( TQMouseEvent * );
    void timerEvent( TQTimerEvent * );
    void showEvent( TQShowEvent * );
    void hideEvent( TQHideEvent * );
    
    void drawWireframe();
    void drawFilled();
    void drawSmoothShaded();
    void drawAxis();
    void drawCube();
    
private:
    enum Axis { XAxis, YAxis, ZAxis };
    enum RenderModes { Wireframe, Filled, SmoothShaded, Landscape };
    enum Views { DefaultView, CurrentView, AxisView };

    void rotate( GLfloat deg, Axis axis );
    void calculateVertexNormals();
    void averageNormals();
    void createGrid( int size );
    void destroyGrid();
    void initDisplayLists();
    
    RenderModes mode;
    
    typedef struct grid_normals {
	double u[3], l[3];
    } gridNormals;

    // Structure used to store the vertex normals for the landscape
    typedef struct avg_normals {
	double n[3];
    } avgNormals;

    typedef struct viewMatrix {
	GLfloat model[4][4];      // OpenGL model view matrix for the view
	GLfloat projection[4][4]; // OpenGL projection matrix for the view
    } viewMatrix;

    double      ** landscape; // Height field data
    double      ** wave;      // Wave data
    double      ** wt;        // Parameterized wave data
    gridNormals ** normals;
    avgNormals  ** vertexNormals;
    viewMatrix     views[3];
    
    TQPoint  oldPos;
    GLfloat oldX, oldY, oldZ;
    bool initFractals;
    int  gridSize, gridHalf;
    bool animationRunning;
    bool mouseButtonDown;
    int cubeTimer, animTimer;
    GLfloat cubeRot;
    GLint axisList, cubeList;
};

#endif
