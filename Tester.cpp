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


//define varibales of GUI
float g_Zoom = 1.0f;
float g_Rotation[] = {0.0f,0.0f,0.0f,0.0f};

int Zero = 0;

int playAnimation = 0;

int startTime = 0.0f;
int duration = 0.0f;
////////////////////////////////////////////////////////////////////////////////

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{TESTER->Draw();}
static void idle()										{TESTER->Update();}
static void resize(int x,int y)							{TESTER->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{TESTER->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{TESTER->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{TESTER->MouseMotion(x,y);}

////////////////////////////////////////////////////////////////////////////////

//callback function for play button
void TW_CALL PlayAnimation(void*)
{
	playAnimation = 1;
	startTime = glutGet(GLUT_ELAPSED_TIME);
}

void TW_CALL Pause(void*)
{
	playAnimation = 0;
}

void TW_CALL Resume(void*)
{
	playAnimation = 1;
}
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
	TwGLUTModifiersFunc(glutGetModifiers);
	//Setup AntTweakBar	
	TwInit(TW_OPENGL, NULL);
	TwBar *bar;
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 230' color='96 216 224' ");

	// Background color
	glClearColor( 0., 0., 0., 1. );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );

	//glutMouseFunc( mousebutton );
	//glutMotionFunc( mousemotion );
	//glutPassiveMotionFunc( mousemotion );
	glutReshapeFunc( resize );
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);
	
	//add components to TweakBar
	TwAddVarRW(bar, "Zoom", TW_TYPE_FLOAT, &g_Zoom,
		" min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
		" label='Object rotation' opened=true help='Change the object orientation.' ");

	TwAddButton(bar, "Play", PlayAnimation, NULL, "");
	TwAddButton(bar, "Stop", Pause, NULL, "");
	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	if (argc <=1)
	{
		myScene = new Scene("wasp.skel", "wasp.skin");
	}
	else {
		myScene = new Scene(argv[1], argv[2]);
	}
	int numJoints = myScene->getSkeleton()->joints.size()-1;

	//initialize animation
	myAnimation = new AnimationClip();
	//load animation
	myAnimation->Load(argv[3]);
	

	Cam=new Camera;
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

Tester::~Tester() {
	//delete Program;
	//delete Cube;
	delete myScene;
	delete Cam;

	glFinish();
	glutDestroyWindow(WindowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Update() {
	// Update the components in the world
	Cam->Update();
	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Reset() {
	Cam->Reset();
	Cam->SetAspect(float(WinX)/float(WinY));
}

////////////////////////////////////////////////////////////////////////////////

void Tester::Draw() {
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//if user chooses to play the animation
	if (playAnimation == 1)
	{
		int currTime = glutGet(GLUT_ELAPSED_TIME);
		duration = currTime - startTime;
		//duration = duration % 4000;
		this->myAnimation->Evaluate((float)duration / 1000, myScene->getSkeleton());
	}
	

	//set up model matrix using GUI
	//myScene->getSkin()->model = myScene->getSkeleton()->getRoot()->getWorldMatrix()*myScene->getSkin()->model;
	myScene->getSkin()->model = myScene->getSkeleton()->getRoot()->getWorldMatrix();
	glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(g_Zoom));
	myScene->getSkin()->model = scaling * myScene->getSkin()->model;
	glm::quat myQuat = glm::quat(g_Rotation[0], g_Rotation[1], g_Rotation[2], g_Rotation[3]);
	glm::mat4 rotationMatrix = glm::toMat4(myQuat);
	myScene->getSkin()->model = rotationMatrix * myScene->getSkin()->model;
	myScene->render(Cam->GetViewMtx(), Cam->GetProjMtx());
	TwDraw();
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
	TwWindowSize(x, y);
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
