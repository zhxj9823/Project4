
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "ball.h"
#include "table.h"

Table table;
double cameraAngle1 = -90;//平面角度
double cameraAngle2 = 25;//俯看角度

						 // handle key presses
void keyboard(unsigned char key, int x, int y)//键盘监听事件
{
	switch (key)
	{
	case ' ':
		// reset game on Space key
		table.reset();
		break;
	case 27:
		// exit on Escape key
		exit(0);
		break;
	}
}

// handle special key presses
void special(int key, int x, int y)//上下左右方向键控制摄像机角度，实现位置变换
{
	// rotate the camera
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cameraAngle1 += 5;
		break;
	case GLUT_KEY_RIGHT:
		cameraAngle1 -= 5;
		break;
	case GLUT_KEY_UP:
		cameraAngle2 += 5;
		break;
	case GLUT_KEY_DOWN:
		cameraAngle2 -= 5;
	}

	// limit the angle
	if (cameraAngle2 < 10) cameraAngle2 = 10;//防止钻到桌底和超过天花板
	if (cameraAngle2 > 80) cameraAngle2 = 80;
}

// handle mouse clicks
void mouse(int button, int state, int x, int y)//鼠标监听事件
{
	// shoot by clicking
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) //如果鼠标左键单击并且按下
	{
		if (!table.moving()) table.shoot(); //球不在移动，射击
	}
}

// handle mouse motion
void passive(int x, int y)//鼠标移动但没有按下，控制杆的移动
{
	// adjust stick angle by left/right mouse movement
	int centre = glutGet(GLUT_WINDOW_WIDTH) / 2;
	table.setStickAngle((x - centre) * 0.2 - cameraAngle1);
}

// draw scene
void display(void)//显示场景
{
	// update the game to the current time
	int currentTime = glutGet(GLUT_ELAPSED_TIME);//计算程序每秒的帧数
	table.update(currentTime);

	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清除颜色缓冲区和深度缓冲区
	glEnable(GL_DEPTH_TEST);//启用之后,绘制时检查，当前像素前面是否有别的像素，如果别的像素挡道了它，那它就不会绘制，也就是说，OpenGL就只绘制最前面的一层。

							// window sizes
	int width = glutGet(GLUT_WINDOW_WIDTH);//得到窗口参数属性
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	// set perspective projection
	glMatrixMode(GL_PROJECTION); //将当前矩阵指定为投影矩阵
	glLoadIdentity(); //然后把矩阵设为单位矩阵
					  //?????????????????????????????
	double fieldOfView = 50.0;  //越小看到的范围越小
	double aspectRatio = (double)width / (double)height;//实际窗口的纵横比
	double nearPlane = 0.1;//近处的裁面
	double farPlane = 100.0;//远处的裁面
	gluPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);

	// set camera
	glMatrixMode(GL_MODELVIEW);//模型视景矩阵
	glLoadIdentity();
	glTranslated(0, 0, -8); //函数参数(x, y, z)
	glRotated(cameraAngle2, 1, 0, 0);//根据相机角度调整画面位置 //使图象沿X轴旋转
	glRotated(cameraAngle1, 0, 1, 0);//使图象沿Y轴旋转

									 // set light colour and position
	glEnable(GL_LIGHTING);//启用光照
	glEnable(GL_LIGHT0);//0号光照
	glEnable(GL_LIGHT1);//1号光照
						//最后一个参数为0，说明是方向性光源，非0则为位置性光源 （如台灯，考虑衰减） 
	GLfloat lightAmbient[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; //设置光源的环境光属性，默认值(0, 0, 0, 1)）
	GLfloat lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };//设置光源的散射光属性，默认值(1,1,1,1)
	GLfloat lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //设置光源的镜面反射光属性，默认值(1, 1, 1, 1)
	GLfloat lightPosition[4] = { 0.0f, 4.0f, 1.0f, 1.0f };//设置光源的位置，默认值(0,0,1,0)
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// normalise normals automatically
	glEnable(GL_NORMALIZE);//自动单位化法向量

						   // cause material colours to track current colour
	glEnable(GL_COLOR_MATERIAL);// 使用颜色材质 少了这一行会变成灰白
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//该命令使材质的特性色和当前颜色(由glColor*()设置)一致。
	//face参数可以是GL_FRONT, GL_BACK, 或GL_FRONT_AND_BACK(缺省值)，
	//mode参数可以是GL_AMBIENT, GL_DIFFUSE, GL_AMBIENT_AND_DIFFUSE(缺省值),
	//GL_SPECULAR, 或 GL_EMISSION。face代表哪个或哪些表面的材质特性色需要
	//改变为当前颜色，mode代表哪些材质特性色需要改变。

	table.draw();

	// refresh window
	glutSwapBuffers();//交换两个缓冲区指针
	glutPostRedisplay();//重绘图像，否则图像只有响应鼠标或键盘消息时才会更新图像
						//但当我们进行复杂的绘图操作时，画面便可能有明显的闪烁。解决这个问题的关键在
						//于使绘制的东西同时出现在屏幕上。所谓双缓冲技术， 是指使用两个缓冲区： 前台
						//缓冲和后台缓冲。前台缓冲即我们看到的屏幕，后台缓冲则在内存当中，对我们来说
						//是不可见的。每次的所有绘图操作都在后台缓冲中进行， 当绘制完成时， 把绘制的
						//最终结果复制到屏幕上， 这样， 我们看到所有GDI元素同时出现在屏幕上，从而解
						//决了频繁刷新导致的画面闪烁问题。
}

int main(int argc, char* argv[])
{
	// initialise GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);//显示模式
	glutInitWindowSize(1280, 720);//窗口大小	
	glutCreateWindow("Billiard Ball game"); //窗口名字

											// register callbacks 注册一些列回调函数
	glutKeyboardFunc(keyboard);//键盘功能
	glutSpecialFunc(special);//响应键盘方向控制键
	glutMouseFunc(mouse);//鼠标功能
	glutPassiveMotionFunc(passive);//相应鼠标没有被按下去时，移到鼠标的情形
	glutDisplayFunc(display);//显示当前窗口

	quadricObject = gluNewQuadric();
	glutMainLoop();//程序进入事件处理循环。该函数必须是main主函数的最后一条语句。
	return 0;
}
