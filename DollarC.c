#define GTCUI_DEBUG
#include <GtCui.h>

//#include <Windows.h>
#include "ConsoleUI.h"

#include <math.h>
#ifndef M_PI
#define M_PI	3.141592653589793238462643383
#endif
#include <time.h>

/*$off*/
GLubyte bDollar[] =
	"             "
	"             "
	"      #      "
	"    #####    "
	"    # #      "
	"    # #      "
	"    #####    "
	"      # #    "
	"      # #    "
	"    #####    "
	"      #      "
	"             "
	"             ";
GLubyte bC[] =
	"             "
	"             "
	"             "
	"    #####    "
	"    #        "
	"    #        "
	"    #        "
	"    #        "
	"    #        "
	"    #####    "
	"             "
	"             "
	"             ";
GLubyte bDolC[] =
	"             "
	"             "
	"   #         "
	" ##### ##### "
	" # #   #     "
	" # #   #     "
	" ##### #     "
	"   # # #     "
	"   # # #     "
	" ##### ##### "
	"   #         "
	"             "
	"             ";
const char	*DCVS =
	"#version 330 core\n"
	"layout (location = 0) in vec2 vPos;"
	"layout (location = 1) in vec2 vTex;"
	"out vec2 fPos;"
	"out vec2 fTex;"
	"void main(){"
		"gl_Position = vec4(vPos.x, vPos.y, 0, 1);"
		"fPos = vPos;"
		"fTex = vTex;"
	"}";
const char	*DCFS =
	"#version 330 core\n"
	"uniform vec4 Color;"
	"uniform sampler2D Texture;"
	"uniform bool TexMode = false;"
	"in vec2 fPos;"
	"in vec2 fTex;"
	"vec2 fTex2;"
	"out vec4 fColor;"
	"void main(){"
		"fColor = Color;"
		"if(TexMode)"
			"if(Color.r != Color.g || Color.g != Color.b)" //判定不是灰的东西
			"{"
				"fTex2.x = fTex.x;"
				"fTex2.y = 1 - fTex.y;"
				"fColor.rgb *= texture(Texture, fTex2).a;"
			"}"
		"if(abs(fPos.x) > 0.5)"
			"fColor.a *= 1 - (abs(fPos.x) - 0.5) * 2;"
		"if(abs(fPos.y) > 0.5)"
			"fColor.a *= 1 - (abs(fPos.y) - 0.5) * 2;"
	"}";
/*$on*/
GLFWwindow	*DCMain;
GLuint		tDollar, tC, tDolC;

void		dollarC();
void		Trapped(int Mx, int My);
void		DCdraw();
GLuint		LoadBt(int w, int h, GLubyte data[]);
void		DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4]);
void		framebuffer_size_callback(GLFWwindow *window, int w, int h);

int main()
{
	int	i;
	UIInit();
	Start();

	srand(time(NULL));

	// Prepare for glfw
	glfwInit();
	DCMain = glfwCreateWindow(600, 600, "Dollar C", NULL, NULL);
	glfwMakeContextCurrent(DCMain);
	glfwSetFramebufferSizeCallback(DCMain, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	Gt_init(DCVS, DCFS);

	//Gt_ScrSize(800, 600);    //不使用Gt_Input
	glViewport(0, 0, 600, 600);

	tDollar = LoadBt(13, 13, bDollar);
	tC = LoadBt(13, 13, bC);
	tDolC = LoadBt(13, 13, bDolC);

	dollarC();

	glfwTerminate();

	getch();

	// free(DCData);           //不要作死
	return 0;
}

/****** Main functions ******/
#define DCMap_H 64
#define DCMap_W 64

#define Front	0
#define Back	1
#define Left	2
#define Right	3
/*$off*/
const struct Pattrib
{
	int	Sx, Sy;
	GLfloat color[3];
}
Pattr[4] = {
	{ 0, 1, { 1, 1, 0} }, //Front
	{ 0, -1, { 0, 0, 1} },//Back
	{ -1, 0, { 1, 0, 0} }, //Left
	{ 1, 0, { 0, 1, 0} } //Right
};
/*$on*/
char	DCData[DCMap_H][DCMap_W];
double	Px, Py, Ptimer, Psp;			//Player pos, Ptimer, speed
char	Pdir;					//Player direction
GLfloat Pcol[4] = { 0, 0, 0, 1 };		//Player color
int	Ddelay, Drate, Dtimer = 0;		//Dollar
/**/
void dollarC()
{
	/* init */
	Px = Py = Ptimer = 0;
	Pdir = Front;
	memset(DCData, 0, sizeof(DCData));

	while(!glfwWindowShouldClose(DCMain))
	{
		printf("  Survival Time: + %5.2lf s\r", Ptimer);
		Ptimer += 1 / 60.0;
		Psp = 0.05 + Ptimer * 0.0005;	//Player speed
		Ddelay = 60 / (1 + Ptimer / 60);			//生成$用的时间
		Drate = 120 / (1 + Ptimer / 15);			//生成$的频率

		/* move */
		{
			//if(glfwGetKey(DCMain, GLFW_KEY_F)) Ptimer++;
			if(glfwGetKey(DCMain, GLFW_KEY_W)) Pdir = Front;
			if(glfwGetKey(DCMain, GLFW_KEY_S)) Pdir = Back;
			if(glfwGetKey(DCMain, GLFW_KEY_A)) Pdir = Left;
			if(glfwGetKey(DCMain, GLFW_KEY_D)) Pdir = Right;
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
							//Be Trapped By $
							Trapped(Mx, My);
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
	}
}

void Trapped(int Mx, int My)
{
	int	i, j;
	GtRect	gr[33];
	for(i = 0; i < 64; i++)
	{
		if(glfwWindowShouldClose(DCMain)) return;
		Px += (Mx + 0.5 - Px) * 0.05;
		Py += (My + 0.5 - Py) * 0.05;
		DCdraw();
		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}

	for(i = 0; i < 33; i++)
	{
		Gt_SetPos(gr[i], 0, 0, 0, 0);
		switch(i % 4)
		{
		case 0:
			Gt_SetColor(gr[i], 1, 1, 0, 0);
			break;

		case 1:
			Gt_SetColor(gr[i], 0, 0, 1, 0);
			break;

		case 2:
			Gt_SetColor(gr[i], 1, 0, 0, 0);
			break;

		case 3:
			Gt_SetColor(gr[i], 0, 1, 0, 0);
			break;
		}
	}

	for(i = 0; i < 256; i++)
	{
		int	t = i / 4;
		if(glfwWindowShouldClose(DCMain)) return;
		if(t > 32) t = 32;
		glClearColor(gr[t].r, gr[t].g, gr[t].b, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		DCdraw();

		for(j = 0; j <= t; j++)
		{
			gr[j].x = gr[j].y += ((-0.8 - gr[j].x) * 0.1);
			gr[j].w = gr[j].h = -gr[j].x * 2;
			gr[j].a += ((1 - gr[j].a) * 0.2);
		}

		Gt_EnableTex(tDolC);
		Gt_Draw(gr, t + 2);
		Gt_DisableTex();

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}

	Px = Mx;
	Py = My;
}

void DCdraw()
{
	GtRect	gr[17 * 17];
	int	x0 = (int) Px, y0 = (int) Py;
	int	x, y, Mx, My, Mdollar;

	for(x = 0; x < 17; x++)
	{
		for(y = 0; y < 17; y++)
		{
			Mx = x0 - 8 + x;
			My = y0 - 8 + y;
			Gt_SetPos(gr[x * 17 + y], (Mx - Px) / 8, (My - Py) / 8, 1 / 8.0, 1 / 8.0);
			Mdollar = DCData[(Mx + DCMap_H) % DCMap_H][(My + DCMap_W) % DCMap_W];
			if(Mdollar >= Ddelay)	//Have $
			{
				if(Ptimer - (int) Ptimer < 0.5)
					Gt_SetColor(gr[x * 17 + y], 0.9, 0.9, 0.1, 1);
				else
					Gt_SetColor(gr[x * 17 + y], 0.6, 0.6, 0.1, 1);
			}
			else if(Mdollar != 0)	//Create $
			{
				Gt_SetColor(gr[x * 17 + y], 0.2 + Mdollar / (float) Ddelay * 0.8, 0.2, 0.2, 1);
			}
			else			//No $
			{
				if((Mx + My) % 2 == 0)
					Gt_SetColor(gr[x * 17 + y], 0.1, 0.1, 0.1, 1);
				else
					Gt_SetColor(gr[x * 17 + y], 0.2, 0.2, 0.2, 1);
			}
		}
	}

	Gt_EnableTex(tDollar);
	Gt_Draw(gr, 17 * 17);
	Gt_EnableTex(tC);
	DrawCircle(0, 0, 0.4 / 8, Pcol);
}

GLuint LoadBt(int w, int h, GLubyte data[])
{
	GLuint	ret;
	int	i;
	for(i = 0; i < w * h; i++) data[i] = data[i] == ' ' ? '\xFF' : '\x80';
	glGenTextures(1, &ret);
	glBindTexture(GL_TEXTURE_2D, ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	return ret;
}

void DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4])
{
	int	i;
	GLfloat I[32][2];
	GLfloat Itex[32][2];
	for(i = 0; i < 32; i++)
	{
		I[i][0] = x + r * cos(2 * M_PI * i / 32);
		I[i][1] = y + r * sin(2 * M_PI * i / 32);
		Itex[i][0] = (cos(2 * M_PI * i / 32) + 1) / 2;
		Itex[i][1] = (sin(2 * M_PI * i / 32) + 1) / 2;
	}

	glUseProgram(Gt_GP);
	glUniform4f(Gt_lColor, c[0], c[1], c[2], c[3]);
	glBindVertexArray(Gt_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBOtex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Itex), Itex, GL_STREAM_DRAW);

	glBindTexture(GL_TEXTURE_2D, tC);	//Extra
	glDrawArrays(GL_POLYGON, 0, 32);
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, w, h);
	glfwSetWindowAspectRatio(window, 1, 1);
}
