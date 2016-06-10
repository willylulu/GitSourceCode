#include "firstman.h"
#include "heightMap.h"
#include "skyBox.h"
#include "Screen.h"
#include "wingjean.h"
#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3
#define MENU_Image_Abstraction 4
#define MENU_Sharpen_Filter 5
#define MENU_Fish_Eye_Distortion 6
#define MENU_Pixelization 7
#define MENU_Bloom_Effect 8
#define MENU_Magnifier 9
#define MENU_Frosted_Glass_Effect 10
#define MENU_Thermal_Vision_Effect 11
#define MENU_Swirl_Effect 12

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

// load bmp texture
FileHeader FH; //BMP FileHeader 
InfoHeader IH; //BMP InfoHeader
int textureIndex = 0;

// window size
float windowWidth;
float windowHeight;

// uniform attribute
mat4 mvp;
GLint um4mvp;
int toggler = 1;
GLint uToggler;
GLint uImg_size;
GLint uComparisonBarPosition;
GLint uEffectMode;
GLint uMousePosition;
GLuint noiseTexture;
// switch
bool comparisonBarPress = false;
int effectMode = 0;
bool changeMode = true;
int screenState = 0;

// v p matrix
mat4 vMatrix;
mat4 pMatrix;
mat4 skyVMatrix;

vec3 viewRotate = vec3(0.0f, 0.0f, 0.0f);

float scaleFactor = 0.5f;

// mouse attribute
int mouseSensitive = 5;
int preX = -1, preY = -1, addX = 0, addY = 0;
float passiveMousePosition[2];
// obj and fbx
StaticObject *city;
FbxEntity *zombie;

// animation attribute
int animateType = 0;
float animateTime = 0.0f;
float animateDuration = 3.0f;

// shader object
ShaderObject *renderShader;

// post processing
GLuint	FBO;
GLuint	depthRBO;
GLuint	FBODataTexture;
GLuint	window_vao;
float comparisonBarPosition = 0.5f;

//First man
FirstMan* firstMan;
Camera* camera;

vec3 eyePosition = vec3(0, 0, 0);
vec3 eyeDirect = vec3(0, 0, 0);

//Sky Box
SkyBox* skyBox;

//screen
Screen* screen;

//heightmap
HeightMap* heightMap;

//water
Water* water;

//WingJean
WingJean* wingJean;

float before;
float after;

std::vector<float> buttonListY = { 142,219,293,365,440,519 };
float buttonX = 60;
float buttonWidth = 53;
float buttonHeight = 49;

bool flying = false;
bool dragLock = false;
bool wideSight = false;
// window attribute
static const GLfloat window_positions[] = {
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

void checkError(const char *functionName)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "GL error 0x%X detected in %s\n", error, functionName);
	}
}

// Print OpenGL context related information.
void dumpInfo(void)
{
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void My_Init()
{

	screen = new Screen();
	screen->bindMainPosition(firstMan);
	screen->bindMapSize(1000, 1000);
	screen->bindButton(buttonListY,buttonX);

	wingJean = new WingJean(firstMan, &flying, &dragLock);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// alpha blend -> leaf
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// shader object
	renderShader = new ShaderObject("vertex.vs.glsl", "fragment.fs.glsl");
	
	std::vector<char*> uniform;
	uniform.push_back("mMatrix");
	uniform.push_back("vMatrix");
	uniform.push_back("pMatrix");
	uniform.push_back("toggler");
	renderShader->initialUniform(uniform);
}

void My_Load_Things()
{
	firstMan = new FirstMan(vec3(300.0f, 200.0f, 450.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0), vec3(0.0f, 1.0f, 0.0f));
	camera = new Camera(firstMan);
	// skybox
	char* skyBoxNames[6] = { "right.bmp","left.bmp","top.bmp","bottom.bmp","back.bmp","front.bmp" };
	skyBox = new SkyBox(skyBoxNames);

	//water
	water = new Water( 0.00002, "vertex.watervs.glsl", "fragment.waterfs.glsl");
	
	// height map
	heightMap = new HeightMap("heightMap.bmp",0.005,1000,1000);

	// zombie
	zombie = new FbxEntity("Medieval_character_01.FBX", vec3(300.0f, 0.0f, 450.0f), vec3(radians(-90.0f), 0, radians(90.0f)), vec3(5, 5, 5));	
	zombie->position.y = heightMap->getHeight(zombie->position.x, zombie->position.z);
	std::vector<float> time_list;
	time_list.push_back(0.0);
	time_list.push_back(1.5/7.0);
	time_list.push_back(1.5 / 7.0);
	time_list.push_back(3/7);
	time_list.push_back(3 / 7.0);
	time_list.push_back(3.5 / 7.0);
	time_list.push_back(3.5 / 7.0);
	time_list.push_back(4 / 7.0);
	time_list.push_back(4 / 7.0);
	time_list.push_back(4.5 / 7.0);
	time_list.push_back(4.5 / 7.0);
	time_list.push_back(5.2 / 7.0);
	time_list.push_back( 5.2 / 7.0);
	time_list.push_back(5.7 / 7.0);
	time_list.push_back(5.7 / 7.0);
	time_list.push_back(6.3 / 7.0);
	time_list.push_back(6.3 / 7.0);
	time_list.push_back(7.0 / 7.0);
	zombie->save_animation(time_list);

	// city
	city = new StaticObject("Medieval_City.obj", vec3(0, 0, 0), vec3(0, 0, 0), vec3(1000, 1000, 1000));
}


bool startHeight = true;
// GLUT callback. Called to draw the scene.
void My_Display()
{
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	
	firstMan->setForwardVectorRecur(rotate(mat4(), radians(viewRotate.z), vec3(0.0f, 0.0f, 1.0f))
		* rotate(mat4(), radians(-viewRotate.y), vec3(0.0f, 1.0f, 0.0f)));

	zombie->position = firstMan->getPosition();
	zombie->position.y -= 2.5f;
	zombie->rotation = vec3(zombie->rotation.x , zombie->rotation.y, zombie->rotation.z + radians(-viewRotate.y));

	viewRotate = vec3(0);
	// vp matrices 
	float viewportAspect = windowWidth / windowHeight;

	//first people gravity effect
	if (!flying)heightMap->gravity(*firstMan,time);
	wingJean->getWingJean();
	//eyePosition.y = heightMap->getHeight(eyePosition.x, eyePosition.z);
	
	//set v & p matrix
	camera->setManView();
	vMatrix = camera->getView();
	pMatrix = perspective(radians(wideSight ? 150.0f : 65.0f), viewportAspect, 0.1f, 10000.0f);
	mvp = pMatrix * vMatrix;


	water->bindCamera(&pMatrix,camera->getPosition(),camera->getForward(),camera->getUpVector());
	//set water uniform data
	std::vector<UniformData> uniformdataForWater;
	uniformdataForWater.push_back(UniformData{ "vpMatrix","mat4",1,water->getPtrWaterVP() });
	uniformdataForWater.push_back(UniformData{ "toggler","int",1,water->getPtrRefleType() });
	//water cature reflect fbo
	
	water->startFbo();
	{
		//draw sky
		skyBox->displaySkyBox(&water->getWaterReflectMatrix(), &pMatrix, &water->getWaterReflectPosition());
		// draw city
		city->render(water->renderShader, uniformdataForWater);
		// draw zombie
		zombie->do_animation(1);
		zombie->render(water->renderShader, uniformdataForWater);
	}
	water->endFbo();

	uniformdataForWater.clear();
	uniformdataForWater.push_back(UniformData{ "vpMatrix","mat4",1,water->getPtrWaterVP2() });
	uniformdataForWater.push_back(UniformData{ "toggler","int",1,water->getPtrRefreType() });
	water->startFbo2();
	{
		// draw city
		city->render(water->renderShader, uniformdataForWater);
	}
	water->endFbo2();

	std::vector<UniformData> uniformdata;
	uniformdata.push_back(UniformData{ "vMatrix","mat4",1,value_ptr(vMatrix) });
	uniformdata.push_back(UniformData{ "pMatrix","mat4",1,value_ptr(pMatrix) });
	uniformdata.push_back(UniformData{ "toggler","int",1,&toggler });
	screen->setScreenState(screenState);
	screen->startFbo();
	{
		//draw sky
		skyBox->displaySkyBox(&vMatrix, &pMatrix,camera->getPosPtr());
		// draw city
		city->render(renderShader, uniformdata);
		// draw zombie
		zombie->do_animation(1);
		zombie->render(renderShader, uniformdata);

		//draw water
		water->displayWater(mvp,vMatrix,time);
	}
	screen->endFbo();
	screen->displayScreen();

	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	windowWidth = (float)width;
	windowHeight = (float)height;
	glViewport(0.0f, 0.0f, windowWidth, windowHeight);

	heightMap->reshape(width, height);
	screen->reShape(width, height);
	water->reshpae(width, height);
}

void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if (timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void buttonFunction(int i)
{
	screen->switchButton(i);
	switch (i)
	{
	case 5:
		wideSight = wideSight ? false : true;
		break;
	case 4:
		screen->switchRbo();
		break;
	case 3:
		skyBox->switchSkyBox();
		break;
	case 2:
		screen->switchToon();
		break;
	case 1:
		screen->switchsmallmap();
		break;
	case 0:
		screen->switchGroupTeam();
		break;
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			printf("press:%d %d\n", x, y);
			if (screenState==1)
			{
				float windowX = x / windowWidth;
				float windowY = y / windowHeight;
				float mx = (windowX - 0.5) * 1000;
				float my = ((1 - windowY) - 0.5) * 1000;
				firstMan->setPosition(vec3(mx,200,my));
				firstMan->setSpeed(vec3(0));
				printf("%f %f %f\n",mx,my,heightMap->getHeight(mx,my));
			}
			else if (screenState == 2)
			{
				x = x * 1280 / windowWidth;
				y = y * 720 / windowHeight;
				if (buttonX <= x && x <= buttonX + buttonWidth)
				{
					for (int i = 0; i < buttonListY.size(); i++)
					{
						if (y >= buttonListY[i] && y <= buttonListY[i] + buttonHeight)
						{
							buttonFunction(i);
						}
					}
				}
				if (60 <= x&&x <= 113 && 595 <= y&&y <= (595 + buttonHeight))
				{
					screen->addColorFactor();
				}
				if (316 <= x&&x <= (316 + 53) && 595 <= y&&y <= (595 + buttonHeight))
				{
					screen->minusColorFactor();
				}
			}
			break;
		}
	}
}

void My_Drag_Mouse(int x, int y)
{
	if (changeMode ) {
		if (preX != -1 || preY != -1) {
			if (abs(x - preX) < mouseSensitive * 4) {
				addX += x - preX;
			}
			if (abs(y - preY) < mouseSensitive * 4) {
				addY += y - preY;
			}
		}
		if (addX >= mouseSensitive || addX <= -mouseSensitive) {
			viewRotate.y += addX / mouseSensitive;
			addX = 0;
		}
		if (addY >= mouseSensitive || addY <= -mouseSensitive) {
			viewRotate.z += addY / mouseSensitive;
			addY = 0;
		}
		preX = x;
		preY = y;
	}
	if (comparisonBarPress) {
		comparisonBarPosition = x / windowWidth;
	}
}

void My_Passive_Mouse(int x, int y) {
	/*passiveMousePosition[0] = (float)x;
	passiveMousePosition[1] = (float)(windowHeight - y);*/
	if (changeMode && screenState==0 && !dragLock ) {
		if (preX != -1 || preY != -1) {
			if (abs(x - preX) < mouseSensitive * 4) {
				addX += x - preX;
			}
			if (abs(y - preY) < mouseSensitive * 4) {
				addY += y - preY;
			}
		}
		if (addX >= mouseSensitive || addX <= -mouseSensitive) {
			viewRotate.y += addX;
			addX = 0;
		}
		if (addY >= mouseSensitive || addY <= -mouseSensitive) {
			viewRotate.z += addY;
			addY = 0;
		}
		preX = x;
		preY = y;
	}
}

void checkMove(vec3 *eyePosition, vec3 forwardVector)
{
	before = (heightMap->getHeight(eyePosition->x, eyePosition->z));
	after = (heightMap->getHeight((*eyePosition + forwardVector * scaleFactor).x, (*eyePosition + forwardVector * scaleFactor).z));
	if (after - before < 10) 
	{
		eyePosition->x += forwardVector.x* scaleFactor;
		eyePosition->z += forwardVector.z* scaleFactor;
		eyePosition->y = after;
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
	case 'W':
		if (flying) firstMan->setMove(firstMan->getForwardVector());
		else checkMove(firstMan->getPtrPos(), firstMan->getForwardVector());
		break;
	case 's':
	case 'S':
		if (flying)firstMan->setMove(-firstMan->getForwardVector());
		else checkMove(firstMan->getPtrPos(), -firstMan->getForwardVector());
		break;
	case 'a':
	case 'A':
		if (flying) firstMan->setMove(-firstMan->getRightVector());
		else checkMove(firstMan->getPtrPos(), -firstMan->getRightVector());
		break;
	case 'd':
	case 'D':
		if (flying) firstMan->setMove(firstMan->getRightVector());
		else checkMove(firstMan->getPtrPos(), firstMan->getRightVector());
		break;
	case '2':
		if (screenState != 0) 
		{
			screenState = 0;
		}
		else 
		{
			screenState = 1;
		}
		break;
	case '1':
		if (screenState != 0)
		{
			screenState = 0;
		}
		else
		{
			screenState = 2;
		}
		break;
	case 'F':
	case 'f':
		flying = flying ? false : true;
		break;
	case 'H':
	case 'h':
		if (!dragLock)wingJean->startWingJean();
		else wingJean->endWingJean();
		break;
	case 'P':
	case 'p':
		camera->setMan();
		break;
	}
}

void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	case MENU_Image_Abstraction:
		effectMode = 0;
		break;
	case MENU_Sharpen_Filter:
		effectMode = 1;
		break;
	case MENU_Fish_Eye_Distortion:
		effectMode = 2;
		break;
	case MENU_Pixelization:
		effectMode = 3;
		break;
	case MENU_Bloom_Effect:
		effectMode = 4;
		break;
	case MENU_Magnifier:
		effectMode = 5;
		break;
	case MENU_Frosted_Glass_Effect:
		effectMode = 6;
		break;
	case MENU_Thermal_Vision_Effect:
		effectMode = 7;
		break;
	case MENU_Swirl_Effect:
		effectMode = 8;
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Assignment 03 102062215"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glewInit();
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	dumpInfo();
	My_Load_Things();
	My_Init();

	////////////////////

	// Create a menu and bind it to mouse right button.
	////////////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_effects = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddSubMenu("Effects", menu_effects);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_effects);
	glutAddMenuEntry("Image Abstraction", MENU_Image_Abstraction);
	glutAddMenuEntry("Sharpen Filter", MENU_Sharpen_Filter);
	glutAddMenuEntry("Fish-Eye Distortion", MENU_Fish_Eye_Distortion);
	glutAddMenuEntry("Pixelization", MENU_Pixelization);
	glutAddMenuEntry("Bloom Effect", MENU_Bloom_Effect);
	glutAddMenuEntry("Magnifier", MENU_Magnifier);
	glutAddMenuEntry("Frosted Glass Effect", MENU_Frosted_Glass_Effect);
	glutAddMenuEntry("Thermal Vision Effect", MENU_Thermal_Vision_Effect);
	glutAddMenuEntry("Swirl Effect", MENU_Swirl_Effect);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////////////

	// Register GLUT callback functions.
	///////////////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutMotionFunc(My_Drag_Mouse);
	glutPassiveMotionFunc(My_Passive_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);
	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}