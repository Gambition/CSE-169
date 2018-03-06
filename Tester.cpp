////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"
#include "AntTweakBar.h"

////////////////////////////////////////////////////////////////////////////////
using namespace std;
static Tester *TESTER=0;

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	TESTER=new Tester("Spinning Cube",argc,argv);
	glutMainLoop();
	delete TESTER;
	return 0;
}

vec3 air = vec3(0.0f,0.0f,0.01f);
vec3 translatation = vec3(0.0f, 0.0f, 0.0f);
int rowIndex = 0;
int columnIndex = 0;
int width;
int height;
////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{TESTER->Draw();}
static void idle()										{TESTER->Update();}
static void resize(int x,int y)							{TESTER->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{TESTER->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{TESTER->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{TESTER->MouseMotion(x,y);}
static void specialkeys(int key, int x, int y)			{ TESTER->specialKeys(key, x, y); }

////////////////////////////////////////////////////////////////////////////////

Tester::Tester(const char *windowTitle,int argc,char **argv) {
	WinX=800;
	WinY=600;
	LeftDown=MiddleDown=RightDown=false;
	MouseX=MouseY=0;

	// Initialize components

	// Create the window
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WinX, WinY );
	glutInitWindowPosition( 100, 100 );
	WindowHandle = glutCreateWindow( windowTitle );
	glutSetWindowTitle( windowTitle );
	glutSetWindow( WindowHandle );

	// Background color
	glClearColor( 0., 0., 0., 1. );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );

	glutReshapeFunc( resize );
	glutMouseFunc(mousebutton);
	glutMotionFunc(mousemotion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeys);
	glewInit();
	glEnable(GL_DEPTH_TEST);
	
	width = atoi(argv[1]);
	height = atoi(argv[2]);
	myScene = new Scene(width, height);
	myScene->myCloth->Vair = air;
	myScene->myCloth->fixRow(0);
	Cam=new Camera;
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

Tester::~Tester() {

	delete myScene;
	delete Cam;
	

	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Update() {
	// Update the components in the world
	Cam->Update();
	myScene->myCloth->update();
	myScene->myCloth->setupMesh();
	myScene->myCloth->Vair = air;
	
	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Reset() {
	myScene->myCloth->reset();
	Cam->Reset();
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Draw() {

	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	myScene->render(Cam->GetViewMtx(), Cam->GetProjMtx());
	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Quit() {
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Resize(int x,int y) {
	WinX = x;
	WinY = y;
	Cam->SetAspect(float(WinX)/float(WinY));

}

////////////////////////////////////////////////////////////////////////////////

void Tester::Keyboard(int key,int x,int y) {
	switch(key) {
		case 0x1b:		// Escape
			Quit();
			break;
		case 'r':
			Reset();
			break;
		case 'w':
			if (air.z <= 70.0f) {
				air.z += 5.0f;
			}
			break;
		case 's':
			if (air.z >= 0.0f) {
				air.z -= 5.0f;
			}
			break;
	}


}


void Tester::specialKeys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		//translatation.x = 0.01f;
		myScene->myCloth->adjust(vec3(0.1f, 0.0f, 0.0f));
		break;
	case GLUT_KEY_RIGHT:
		//translatation.x = -0.01f;
		myScene->myCloth->adjust(vec3(-0.1f, 0.0f, 0.0f));
		break;

	case GLUT_KEY_UP:
		//translatation.y = 0.01f;
		myScene->myCloth->adjust(vec3(0.0f, 0.1f, 0.0f));
		break;

	case GLUT_KEY_DOWN:
		//translatation.y = -0.01f;
		myScene->myCloth->adjust(vec3(0.0f, -0.1f, 0.0f));
		break;
	}

}


////////////////////////////////////////////////////////////////////////////////

void Tester::MouseButton(int btn,int state,int x,int y) {
	if(btn==GLUT_LEFT_BUTTON) {
		LeftDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_MIDDLE_BUTTON) {
		MiddleDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_RIGHT_BUTTON) {
		RightDown = (state==GLUT_DOWN);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Tester::MouseMotion(int nx,int ny) {
	int maxDelta=100;
	int dx = glm::clamp(nx - MouseX,-maxDelta,maxDelta);
	int dy = glm::clamp(-(ny - MouseY),-maxDelta,maxDelta);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(LeftDown) {
		const float rate=1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth()+dx*rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline()-dy*rate,-90.0f,90.0f));
	}
	if(RightDown) {
		const float rate=0.005f;
		float dist=glm::clamp(Cam->GetDistance()*(1.0f-dx*rate),0.01f,1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
