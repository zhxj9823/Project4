
#include <math.h>
#include <GL/glut.h>
#include "ball.h"
#include "table.h"
extern bool who_play;
unsigned int currentNumber = 16;
bool canchange = false;
bool is_foul = false;
bool start_play = false;
#define MAX_CHAR 128
// table constructor
Table::Table()
{
	// set ball colours 16个小球的颜色都不同 
	balls[0].setColour(0.9, 0.9, 0.9); // white
	balls[1].setColour(1.0, 0.9, 0.2); // yellow
	balls[2].setColour(0.1, 0.2, 0.7); // dark blue
	balls[3].setColour(1.0, 0.0, 0.0); // red
	balls[4].setColour(0.4, 0.0, 0.5); // dark purple
	balls[5].setColour(1.0, 0.5, 0.3); // light orange
	balls[6].setColour(0.2, 0.8, 0.4); // green
	balls[7].setColour(1.0, 0.5, 0.8); // light purple
	balls[8].setColour(0.0, 0.0, 0.0); // black
	balls[9].setColour(0.6, 0.5, 0.2); // dark yellow
	balls[10].setColour(0.2, 0.5, 1.0); // blue
	balls[11].setColour(1.0, 0.2, 0.3); // light red
	balls[12].setColour(0.7, 0.0, 0.7); // purple
	balls[13].setColour(0.8, 0.3, 0.0); // orange
	balls[14].setColour(0.0, 0.3, 0.2); // dark green
	balls[15].setColour(0.5, 0.0, 0.0); // dark red

										// initialise
	time = 0;
	stickAngle = 90;
	reset();
}

// change stick angle
void Table::setStickAngle(double angle)
{
	stickAngle = angle;
}

// reset game state
void Table::reset()
{
	start_play = false;
	// initial position for cue ball
	balls[0].setPosition(2, 0);
	balls[0].setSpeed(0, 0);
	balls[0].setVisible(true);

	// initial position for other balls
	int k = 1;
	for (int i = 1; i < 6; i++)
	{
		for (int j = 1; j < i + 1; j++)
		{
			// use triangle shape
			double r = balls[k].getRadius();
			double x = (6 - i * 2) * r - 2;
			double z = (j * 2 - i - 1) * r;
			balls[k].setPosition(x, z);
			balls[k].setSpeed(0, 0);
			balls[k].setVisible(true);
			k++;
		}
	}
}

// return number of visible balls
int Table::visibleNumber()
{
	int number = 0;
	for (int i = 0; i < 16; i++)
	{
		if (balls[i].getVisible()) number++;
	}
	return number;
}

// return balls motion state
bool Table::moving()
{
	// detect moving balls
	for (int i = 0; i < 16; i++)
	{
		if (balls[i].moving()) return true;
	}

	// no balls are moving
	return false;
}

// shoot cue ball
void Table::shoot()
{
	start_play = true;
	const double s = 18; // shooting speed
	const double pi = 3.14159265358979;
	double a = stickAngle * pi / 180 + pi;
	//canchange = true;
	// set cue ball speed along the stick
	balls[0].setSpeed(s * sin(a), s * cos(a));
}

// update balls
void Table::update(int currentTime)
{
	double timeStep = 0.001; // update interval in seconds
	int i, j;
	// update the time until it catches up with current time
	while (time < currentTime)
	{
		// collision detection for each ball
		for (i = 0; i < 16; i++)   //遍历所有球
		{
			// collide ball with holes and cushions
			balls[i].collideHoles();
			balls[i].collideCushions();
			//if (balls[i].getVisible() == false) {//遇到小球不可见
			//	flag = true;
			//}
			// collide ball with other balls
			for (j = 0; j < i; j++)
			{
				balls[i].collideBall(balls[j]);
			}
		}

		// update ball speeds and positions
		for (i = 0; i < 16; i++)
		{
			balls[i].updateSpeed(timeStep);
			balls[i].updatePosition(timeStep);
		}

		// if cue ball is hidden, place it at the centre
		if (!moving() && !balls[0].getVisible())
		{
			balls[0].setPosition(0, 0);
			balls[0].setSpeed(0, 0);
			balls[0].setVisible(true);
		}

		// if all balls are hidden, reset the game
		if (visibleNumber() == 1) reset();

		time++;
	}
}


//___________________________________绘制字体
void glDrawString(unsigned char *str)
{
	GLYPHMETRICSFLOAT pgmf[1];

	HDC hDC = wglGetCurrentDC();
	HFONT hFont;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 1;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | TMPF_TRUETYPE | FF_MODERN;
	lstrcpy(lf.lfFaceName, "宋体");
	hFont = CreateFontIndirect(&lf);
	//设置当前字体
	SelectObject(wglGetCurrentDC(), hFont);


	DWORD dwChar;
	int ListNum;
	for (size_t i = 0; i<strlen((char *)str); i++)
	{
		if (IsDBCSLeadByte(str[i]))
		{
			dwChar = (DWORD)((str[i] << 8) | str[i + 1]);
			i++;
		}
		else
			dwChar = str[i];
		ListNum = glGenLists(1);
		wglUseFontOutlines(hDC, dwChar, 1, ListNum, 0.0, 0.1f, WGL_FONT_POLYGONS, pgmf);
		glCallList(ListNum);
		glDeleteLists(ListNum, 1);
	}
}


//___________________________




void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) { // 如果是第一次调用，执行初始化
					   // 为每一个ASCII字符产生一个显示列表
		isFirstCall = 0;

		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_CHAR);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void drawPlayer() {

	if (who_play == true) {
		glPushMatrix();
		selectFont(24, ANSI_CHARSET, "Comic Sans MS");
		//glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.5f, 0.5f, 0.5f);
		glRasterPos3f(-5.0f, 0.5f, 0.5f);// 高度
		glTranslated(-3, 0, 0);
		//glTranslated(0,0, 0);
		drawString("Player 1");
		glPopMatrix();

		glPushMatrix();
		selectFont(24, ANSI_CHARSET, "Comic Sans MS");
		//glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(-5.0f, 1.0f, 0.5f);
		//glTranslated(0,0, 0);
		drawString("Player 2");
		glPopMatrix();
	}
	else if (who_play == 0) {
		glPushMatrix();
		selectFont(24, ANSI_CHARSET, "Comic Sans MS");
		//glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(-5.0f, 0.5f, 0.5f);// 高度
		glTranslated(-3, 0, 0);
		//glTranslated(0,0, 0);
		drawString("Player 1");
		glPopMatrix();

		glPushMatrix();
		selectFont(24, ANSI_CHARSET, "Comic Sans MS");
		//glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.5f, 0.5f, 0.5f);
		glRasterPos3f(-5.0f, 1.0f, 0.5f);
		//glTranslated(0,0, 0);
		drawString("Player 2");
		glPopMatrix();
	}

}
// draw table
void Table::draw()
{
	// draw floor
	glColor3d(0.5, 0.5, 0.4); // grey colour
	setShininess(0.0); // not shiny
	glPushMatrix();
	glTranslated(0, -2, 0);
	drawCuboid(20, 0.1, 20);
	glPopMatrix();

	// draw walls
	glColor3d(0.4, 0.5, 0.4); // light green colour
	glPushMatrix();
	glTranslated(-10, 0, 0); drawCuboid(0.1, 20, 20);
	glTranslated(+20, 0, 0); drawCuboid(0.1, 20, 20);
	glColor3d(0.3, 0.4, 0.3); // dark green colour
	glTranslated(-10, 0, -10); drawCuboid(20, 20, 0.1);
	glTranslated(0, 0, +20); drawCuboid(20, 20, 0.1);
	glPopMatrix();

	//TODO

	// 绘制计分板
	glColor3d(0.0, 0.0, 0.0); // black
	glPushMatrix();
	glTranslated(-8, 0, 0);
	drawCuboid(0.1, 4, 6.5);//厚度 高度 宽度
	glPopMatrix();

	glColor3d(0.9, 0.9, 0.9); // white
	glPushMatrix();
	glTranslated(-7.9, 0.25, 0);
	drawCuboid(0.1, 2, 6);//厚度 高度 宽度
	glPopMatrix();

	glPushMatrix();
	glTranslated(-8, 2, 3.3);//越小越远， 越大越高，  越大越左
	glRotated(90, 0, 1, 0);
	unsigned char cstr[] = { "Billiards Game" };
	glDrawString(cstr);
	glPopMatrix();


	drawPlayer();



	// draw 3 lamps
	for (int i = -1; i <= 1; i++)
	{
		glPushMatrix();

		// lamp shade
		glColor3d(1.0, 0.3, 0.2); // red colour
		setShininess(0.0); // not shiny
		glTranslated(i * 2.3, 2.8, 0);
		glRotated(-90, 1, 0, 0);
		drawCylinder(0.8, 0, 1.1);

		// light bulb
		glColor3d(2.0, 2.0, 2.0); // white colour
		drawSphere(0.25);

		// lamp mount
		setShininess(1.0); // shiny
		glTranslated(0, 0, 0.8);
		glColor3d(1.0, 0.8, 0.4); // yellow colour
		drawCylinder(0.2, 0.2, 0.2);
		glTranslated(0, 0, 0.2);
		glColor3d(0.9, 0.7, 0.4); // yellow colour
		drawCylinder(0.2, 0.04, 0.1);
		glColor3d(0.8, 0.6, 0.4); // yellow colour
		drawCylinder(0.04, 0.04, 10);

		glPopMatrix();
	}

	double hsize = 0.5; // hole size

						// draw table surface
	glColor3d(0.3, 0.5, 0.6); // light blue colour
	setShininess(0.0); // not shiny
	glPushMatrix();
	glTranslated(0, -0.1, 0);
	drawCuboid(8.4, 0.2, 4.4);
	glPopMatrix();

	// draw table cushions
	glColor3d(0.1, 0.3, 0.5); // dark blue colour
	glPushMatrix();
	glTranslated(-4, 0, +0); drawCuboid(0.1, 0.2, 4.0 - hsize);
	glTranslated(+8, 0, +0); drawCuboid(0.1, 0.2, 4.0 - hsize);
	glTranslated(-2, 0, -2); drawCuboid(4.0 - hsize, 0.2, 0.1);
	glTranslated(-4, 0, +0); drawCuboid(4.0 - hsize, 0.2, 0.1);
	glTranslated(+0, 0, +4); drawCuboid(4.0 - hsize, 0.2, 0.1);
	glTranslated(+4, 0, +0); drawCuboid(4.0 - hsize, 0.2, 0.1);
	glPopMatrix();

	// draw table edges
	glColor3d(0.3, 0.2, 0.2); // brown colour
	setShininess(1.0); // shiny
	glPushMatrix();
	glTranslated(-4.2, 0, +0.0); drawCuboid(0.3, 0.3, 4.7);
	glTranslated(+8.4, 0, +0.0); drawCuboid(0.3, 0.3, 4.7);
	glTranslated(-4.2, 0, -2.2); drawCuboid(8.2, 0.3, 0.3);
	glTranslated(+0.0, 0, +4.4); drawCuboid(8.2, 0.3, 0.3);
	glPopMatrix();

	// draw table holes
	glColor3d(0.0, 0.0, 0.0); // black colour
	setShininess(0.0); // not shiny
	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glTranslated(+4, 2, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glTranslated(-4, 0, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glTranslated(-4, 0, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glTranslated(0, -4, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glTranslated(+4, 0, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glTranslated(+4, 0, 0); glutSolidCone(hsize * 0.4, 0.01, 20, 1);
	glPopMatrix();

	// draw table legs
	glColor3d(0.2, 0.2, 0.2); // black colour
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(+3.5, +1.5, 0.1); drawCylinder(0.2, 0.2, 2);
	glTranslated(+0.0, -3.0, 0.0); drawCylinder(0.2, 0.2, 2);
	glTranslated(-7.0, +0.0, 0.0); drawCylinder(0.2, 0.2, 2);
	glTranslated(+0.0, +3.0, 0.0); drawCylinder(0.2, 0.2, 2);
	glPopMatrix();

	// draw balls
	for (int b = 0; b < 16; b++)
	{
		balls[b].draw();
	}

	// draw stick
	if (!moving())
	{
		glColor3d(0.7, 0.6, 0.5); // yellow colour
		glPushMatrix();
		glTranslated(balls[0].getX(), 0, balls[0].getZ());
		glRotated(stickAngle, 0, 1, 0);
		glTranslated(0, 0.15, 0.4);
		glRotated(-5, 1, 0, 0);
		drawCylinder(0.02, 0.06, 5.0);
		glPopMatrix();
	}

	if (!moving()) {//所有球静止了

		if (currentNumber == visibleNumber() && start_play&&canchange) {//犯规情况下，球个数不变；没打进，球个数不变这些情况都要交换球权
			who_play = !who_play;//交换球权
		}
		else {
			currentNumber = visibleNumber();//更新当前数量的球	
		}
		canchange = false;//防止重复更新
	}
	else {
		canchange = true;
	}
}
