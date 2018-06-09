#include <time.h>
#include <math.h>
#ifndef M_PI
#define M_PI	3.141592653589793238462643383
#endif
#include <GtCui.h>
#include "DCFont.h"

/*$off*/
const char	*DCVS =
	"#version 330 core\n"
	"uniform vec2 xPos;"//坐标偏移

	"layout (location = 0) in vec2 vPos;"
	"layout (location = 1) in vec2 vTex;"
	"out vec2 fPos;"
	"out vec2 fTex;"

	"void main(){"
		"fPos = vPos + xPos;"
		"fTex = vTex;"
		"gl_Position = vec4(fPos.x, fPos.y, 0, 1);"
	"}";
const char	*DCFS =
	"#version 330 core\n"

	"uniform vec4 Color;"
	"uniform sampler2D Texture;"

	"uniform bool TexMode = false;" //贴图使用开关
	"uniform int drawMode;"		//贴图绘制模式
	"uniform float shadSize;"	//四周阴影深度

	"in vec2 fPos;"
	"in vec2 fTex;"
	"out vec4 fColor;"
	"vec2 fTex2;"

	"void main(){"
		"fColor = Color;"
		"if(TexMode)"
		"{"
			"fTex2.x = fTex.x;"
			"fTex2.y = 1 - fTex.y;"
			"if(drawMode == 0)"//正常绘制
				"fColor.a *= texture(Texture, fTex2).a;"
			"if(drawMode == 1)"//反色绘制
			"{"
				"fColor.rgb *= 1 - texture(Texture, fTex2).a;"
				"if(abs(fPos.x) > 1-shadSize)"
					"fColor.a *= 1 - (abs(fPos.x) + shadSize - 1) / shadSize;"
				"if(abs(fPos.y) > 1-shadSize)"
					"fColor.a *= 1 - (abs(fPos.y) + shadSize - 1) / shadSize;"
			"}"
		"}"
	"}";
/*$on*/
GLFWwindow	*DCMain;
GLuint		lxPos, ldrawMode, lshadSize;

void		DCMenu();
void		DCPlay();
void		DCTrap(int Mx, int My);
void		DCdraw();
void		DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4]);
void		framebuffer_size_callback(GLFWwindow *window, int w, int h);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	glfwInit();
	//glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	DCMain = glfwCreateWindow(600, 600, "Dollar C", NULL, NULL);
	glfwMakeContextCurrent(DCMain);
	glfwSetFramebufferSizeCallback(DCMain, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	Gt_init(DCVS, DCFS);

	glUseProgram(Gt_GP);

	lxPos = glGetUniformLocation(Gt_GP, "xPos");
	ldrawMode = glGetUniformLocation(Gt_GP, "drawMode");
	lshadSize = glGetUniformLocation(Gt_GP, "shadSize");

	glUniform2f(lxPos, 0, 0);			//坐标偏移:(0, 0)
	glUniform1i(ldrawMode, 0);			//贴图模式:正常绘制
	glUniform1f(lshadSize, 0);			//阴影深度:0

	//Gt_ScrSize(800, 600);				//不使用Gt_Input
	glViewport(0, 0, 600, 600);

	LoadAllBt();

	Gt_EnableTex();
	DCMenu();
	Gt_DisableTex();

	glfwTerminate();

	exit(0);
}

void DCMenu()
{
	GtRect	gr[2];
	while(1)
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Gt_SetPos(gr[0], -0.6, -0.3, 1.2, 1.2);
		Gt_SetColor(gr[0], 0.9, 0.9, 0.1, 1);
		Gt_SetTexture(gr[0], tDolC);

		Gt_SetPos(gr[1], -0.47, -0.5, 0.94, 0.14);
		Gt_SetColor(gr[1], 1.0, 0.75, 0.0, 1);
		Gt_SetTexture(gr[1], tTip);

		Gt_Draw(gr, 2);

		glfwSwapBuffers(DCMain);
		glfwWaitEvents();
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
		if(glfwGetKey(DCMain, GLFW_KEY_ENTER))
		{
			DCPlay();
			glUniform1i(ldrawMode, 0);	//贴图模式:正常绘制
			glUniform1f(lshadSize, 0);	//阴影深度:0
		}
	}
}

/****** Main functions ******/
#define DCMap_H		256
#define DCMap_W		256

#define Front		0
#define Back		1
#define Left		2
#define Right		3

#define PaRGB(R, G, B)	{ R / 255.0, G / 255.0, B / 255.0 }
/*$off*/
const struct Pattrib
{
	int	Sx, Sy;
	GLfloat color[3];
}
Pattr[4] = {
	{ 0, 1, PaRGB(255, 185, 0) },  //Front
	{ 0, -1, PaRGB(0, 120, 215) }, //Back
	{ -1, 0, PaRGB(232, 17, 35) }, //Left
	{ 1, 0, PaRGB(0, 204, 106) }   //Right
};
/*$on*/
char	DCData[DCMap_H][DCMap_W];
double	Px, Py, Ptimer, Psp;			//Player pos, Ptimer, speed
char	Pdir;					//Player direction
GLfloat Pcol[4] = { 0, 0, 0, 1 };		//Player color
int	Ddelay = 255, Drate, Dtimer = 0;	//Dollar
/**/
void DCBegin()
{
	int	i;

	memcpy(Pcol, Pattr[0].color, 3 * sizeof(GLfloat));
	for(i = 0; i < 64; i++)
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Pcol[3] = i / 64.0;

		DrawCircle(0, 0, 1.5 - (1.5 - 0.4 / 8) * i / 64, Pcol);

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
	}

	Pcol[3] = 1;

	for(i = 0; i < 128; i++)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		if(i < 64)
		{
			glClearColor(0, 0, 0, 1);
			glUniform2f(lxPos, rand() * 0.04 / RAND_MAX - 0.02, rand() * 0.04 / RAND_MAX - 0.02);
		}
		else
		{
			double	k = (i - 64) / 64.0;
			glClearColor(Pcol[0] * k, Pcol[1] * k, Pcol[2] * k, 1);
			glUniform2f(lxPos, 0, 0);
		}

		glUniform1f(lshadSize, 1.5 - i / 128.0);
		DCdraw();

		glUniform1f(lshadSize, 0);
		DrawCircle(0, 0, 0.4 / 8, Pcol);

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
	}
}

void DCPlay()
{
	/* init */
	glUniform1i(ldrawMode, 1);		//贴图模式:反色绘制
	srand(time(NULL));
	Px = Py = Ptimer = 0;
	Pdir = Front;
	memset(DCData, 0, sizeof(DCData));

	/* Begin Animate */
	DCBegin();
	glUniform1f(lshadSize, 0.5);		//阴影深度:0.5
	while(!glfwWindowShouldClose(DCMain))
	{
		/*
		char	title[64];
		sprintf(title, "Dollar C [Time:%5.2lfs]", Ptimer);
		glfwSetWindowTitle(DCMain, title);
		*/
		Ptimer += 1 / 60.0;
		Psp = 0.05 + Ptimer * 0.0005;	//Player speed
		Ddelay = 60 / (1 + Ptimer / 60);			//生成$用的时间
		Drate = 120 / (1 + Ptimer / 15);			//生成$的频率

		

		/* move */
		{
			if(
				glfwGetKey(DCMain, GLFW_KEY_W)|| glfwGetKey(DCMain, GLFW_KEY_UP)
				) Pdir = Front;
			if(
				glfwGetKey(DCMain, GLFW_KEY_S) || glfwGetKey(DCMain, GLFW_KEY_DOWN)
				) Pdir = Back;
			if(
				glfwGetKey(DCMain, GLFW_KEY_A) || glfwGetKey(DCMain, GLFW_KEY_LEFT)
				) Pdir = Left;
			if(
				glfwGetKey(DCMain, GLFW_KEY_D) || glfwGetKey(DCMain, GLFW_KEY_RIGHT)
				) Pdir = Right;
			if(glfwGetKey(DCMain, GLFW_KEY_ESCAPE)) return;

			memcpy(Pcol, Pattr[Pdir].color, 3 * sizeof(GLfloat));

			Px += Pattr[Pdir].Sx * Psp;
			Py += Pattr[Pdir].Sy * Psp;

			if(Px < 0) Px += DCMap_H;
			if(Py < 0) Py += DCMap_W;
			if(Px >= DCMap_H) Px -= DCMap_H;
			if(Py >= DCMap_W) Py -= DCMap_W;
		}

		/* put $ */
		{
			/*
			       w(-)   w(+)
			      ___^__  ___^__
			     /      \/      \
			     $------.-------$
			            |\
			            | > h
			            |/
			            O(正方向^)
			*/
			int	h, w, x, y;
			int	Psx = Pattr[Pdir].Sx, Psy = Pattr[Pdir].Sy;
			Dtimer++;
			if(Dtimer >= Drate)
			{
				h = rand() * 8 / RAND_MAX + 1;
				w = (rand() * h * 2) / RAND_MAX - h;
				x = (int) Px + h * Psx + w * Psy;
				y = (int) Py + w * (-Psx) + h * Psy;
				x = (x + DCMap_H) % DCMap_H;
				y = (y + DCMap_W) % DCMap_W;
				if(DCData[x][y] == 0) DCData[x][y] = 1; //Create $
				Dtimer = 0;
			}
		}

		/* step $ */
		{
			int	x, y;
			for(x = 0; x < DCMap_H; x++)
				for(y = 0; y < DCMap_W; y++)
					if(0 < DCData[x][y] && DCData[x][y] < Ddelay) DCData[x][y]++;	//增加 $ 出现的时间
		}

		/* check $ */
		{
			int	x0 = (int) Px, y0 = (int) Py;
			int	x, y, Mx, My, Mdollar;
			double	Pointx[3] = { x0, Px, x0 + 1 };
			double	Pointy[3] = { y0, Py, y0 + 1 };
			for(x = 0; x < 3; x++)
			{
				for(y = 0; y < 3; y++)
				{
					Mx = x0 - 1 + x;
					My = y0 - 1 + y;
					Mdollar = DCData[(Mx + DCMap_H) % DCMap_H][(My + DCMap_W) % DCMap_W];
					if(Mdollar >= Ddelay)
					{
						if
						(
							((Px - Pointx[x]) * (Px - Pointx[x])) +
								((Py - Pointy[y]) * (Py - Pointy[y])) < 0.16
						)
						{
							//Be Trap By $
							DCTrap(Mx, My);
							return;
						}
					}
				}
			}
		}

		/* draw */
		{
			glClearColor(Pcol[0], Pcol[1], Pcol[2], Pcol[3]);
			glClear(GL_COLOR_BUFFER_BIT);
			DCdraw();
		}

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
	}
}

void DCTrap(int Mx, int My)
{
	int	i, j;
	GtRect	gr[41], gt[6];

	// gr
	for(i = 0; i < 41; i++)
	{
		const GLfloat	*c = Pattr[i % 4].color;
		Gt_SetPos(gr[i], 0, 0, 0, 0);
		Gt_SetColor(gr[i], (int) (c[0] + 0.5), (int) (c[1] + 0.5), (int) (c[2] + 0.5), 0);
		Gt_SetTexture(gr[i], tDolC);
	}

	// gt
	for(i = 0; i < 6; i++)
	{
		Gt_SetPos(gt[i], i * 0.05 - 0.17, -0.7, 0.04, 0.07);
		Gt_SetColor(gt[i], 0, 0, 0, 1);
		Gt_SetTexture(gt[i], tNothing);
	}

	Gt_SetTexture(gt[3], tN[10]);
	if(Ptimer >= 100) Gt_SetTexture(gt[0], tN[(int) (Ptimer / 100) % 10]);
	if(Ptimer >= 10) Gt_SetTexture(gt[1], tN[(int) (Ptimer / 10) % 10]);
	Gt_SetTexture(gt[2], tN[(int) (Ptimer) % 10]);
	Gt_SetTexture(gt[4], tN[(int) (Ptimer * 10) % 10]);
	Gt_SetTexture(gt[5], tN[(int) (Ptimer * 100) % 10]);

	// trapped
	for(i = 0; i < 64; i++)
	{
		Px += (Mx + 0.5 - Px) * 0.05;
		Py += (My + 0.5 - Py) * 0.05;

		DCdraw();

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
	}

	// $c
	for(i = 0; i < 450; i++)
	{
		int	t = i / 4 + 1, t0;
		if(t > 41)
		{
			t = 41;
			glUniform2f(lxPos, 0, 0);
		}
		else
			glUniform2f(lxPos, rand() * 0.04 / RAND_MAX - 0.02, rand() * 0.04 / RAND_MAX - 0.02);

		t0 = t - 8;
		if(t0 < 0) t0 = 0;

		glClearColor(gr[t - 1].r, gr[t - 1].g, gr[t - 1].b, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		DCdraw();

		for(j = 0; j < t; j++)
		{
			gr[j].x += ((-0.8 - gr[j].x) * 0.1);
			gr[j].y += ((-0.8 - gr[j].y) * 0.1);
			gr[j].w += ((1.6 - gr[j].w) * 0.1);
			gr[j].h += ((1.6 - gr[j].h) * 0.1);
			gr[j].a += ((1 - gr[j].a) * 0.2);
		}

		Gt_Draw(&gr[t0], t - t0);

		if(i > 300)
		{
			glUniform1i(ldrawMode, 0);	//贴图模式:正常模式
			Gt_Draw(gt, 6);
			glUniform1i(ldrawMode, 1);	//贴图模式:反色绘制
			if(glfwGetKey(DCMain, GLFW_KEY_ESCAPE)) return;
		}

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();

		if(glfwWindowShouldClose(DCMain)) return;
	}

	Px = Mx;
	Py = My;
}

void DCdraw()
{
	GtRect	gr[17 * 17], gc, gt[6];
	int	x0 = (int) Px, y0 = (int) Py;
	int	x, y, Mx, My, Mdollar;

	for(x = 0; x < 17; x++)
	{
		for(y = 0; y < 17; y++)
		{
			Mx = x0 - 8 + x;
			My = y0 - 8 + y;
			Mdollar = DCData[(Mx + DCMap_H) % DCMap_H][(My + DCMap_W) % DCMap_W];

			Gt_SetPos(gr[x * 17 + y], (Mx - Px) / 8, (My - Py) / 8, 1 / 8.0, 1 / 8.0);
			if(Mdollar >= Ddelay)		//Have $
			{
				if(Ptimer - (int) Ptimer < 0.5)
					Gt_SetColor(gr[x * 17 + y], 0.9, 0.9, 0.1, 1);
				else
					Gt_SetColor(gr[x * 17 + y], 0.4, 0.4, 0.1, 1);
				Gt_SetTexture(gr[x * 17 + y], tDollar);
			}
			else if(Mdollar != 0)		//Create $
			{
				Gt_SetColor(gr[x * 17 + y], 0.2 + Mdollar / (float) Ddelay * 0.8, 0.2, 0.2, 1);
				Gt_SetTexture(gr[x * 17 + y], tDollar);
			}
			else			//No $
			{
				if((Mx + My) % 2 == 0)
					Gt_SetColor(gr[x * 17 + y], 0.1, 0.1, 0.1, 1);
				else
					Gt_SetColor(gr[x * 17 + y], 0.2, 0.2, 0.2, 1);
				Gt_SetTexture(gr[x * 17 + y], tNothing);
			}
		}
	}

	Gt_Draw(gr, 17 * 17);

	DrawCircle(0, 0, 0.4 / 8, Pcol);

	glUniform1i(ldrawMode, 0);		//贴图模式:正常绘制
	Gt_SetPos(gc, -0.4 / 8, -0.4 / 8, 0.8 / 8, 0.8 / 8);
	Gt_SetColor(gc, 0, 0, 0, 1);
	Gt_SetTexture(gc, tC);
	Gt_Draw(&gc, 1);

	for(x = 0; x < 6; x++)
	{
		Gt_SetPos(gt[x], x * 0.05 - 0.17, -0.5, 0.04, 0.07);
		Gt_SetColor(gt[x], Pcol[0], Pcol[1], Pcol[2], 1);
		Gt_SetTexture(gt[x], tNothing);
	}

	Gt_SetTexture(gt[3], tN[10]);
	if(Ptimer >= 100) Gt_SetTexture(gt[0], tN[(int) (Ptimer / 100) % 10]);
	if(Ptimer >= 10) Gt_SetTexture(gt[1], tN[(int) (Ptimer / 10) % 10]);
	Gt_SetTexture(gt[2], tN[(int) (Ptimer) % 10]);
	Gt_SetTexture(gt[4], tN[(int) (Ptimer * 10) % 10]);
	Gt_SetTexture(gt[5], tN[(int) (Ptimer * 100) % 10]);

	Gt_Draw(gt, 6);
	glUniform1i(ldrawMode, 1);		//贴图模式:反色绘制
}

void DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4])
{
	int	i;
	GLfloat I[64][2];
	GLfloat Itex[64][2];
	for(i = 0; i < 64; i++)
	{
		I[i][0] = x + r * cos(2 * M_PI * i / 64);
		I[i][1] = y + r * sin(2 * M_PI * i / 64);
		Itex[i][0] = (cos(2 * M_PI * i / 64) + 1) / 2;
		Itex[i][1] = (sin(2 * M_PI * i / 64) + 1) / 2;
	}

	glUniform4f(Gt_lColor, c[0], c[1], c[2], c[3]);
	glBindVertexArray(Gt_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBOtex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Itex), Itex, GL_STREAM_DRAW);

	glBindTexture(GL_TEXTURE_2D, tC);	//Extra
	glDrawArrays(GL_POLYGON, 0, 64);
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, w, h);
	glfwSetWindowAspectRatio(window, 1, 1);
}
