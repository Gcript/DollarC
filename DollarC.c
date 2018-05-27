#define GTCUI_DEBUG
#include <GtCui.h>

//#include <Windows.h>
#include "ConsoleUI.h"

#include <math.h>
#ifndef M_PI
#define M_PI	3.141592653589793238462643383
#endif
#include <time.h>

#define MAP_SIZE	256

/*$off*/
GLubyte bDollar[] =
	"             "
	"             "
	"      !      "
	"    !!!!!    "
	"      ! !    "
	"      ! !    "
	"    !!!!!    "
	"    ! !      "
	"    ! !      "
	"    !!!!!    "
	"      !      "
	"             "
	"             ";
GLubyte bC[] =
	"             "
	"             "
	"   !         "
	" !!!!! !!!!! "
	"   ! ! !     "
	"   ! ! !     "
	" !!!!! !     "
	" ! !   !     "
	" ! !   !     "
	" !!!!! !!!!! "
	"   !         "
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
	"out vec4 fColor;"
	"void main(){"
		"fColor = Color;"
		"if(TexMode)"
			"fColor.rgb = Color.rgb * texture(Texture, fTex).a;"
		"if(abs(fPos.x) > 0.8)"
			"fColor = mix(fColor, vec4(0, 0, 0, 0), (abs(fPos.x) - 0.8) * 5);"
		"if(abs(fPos.y) > 0.8)"
			"fColor = mix(fColor, vec4(0, 0, 0, 0), (abs(fPos.y) - 0.8) * 5);"
	"}";
/*$on*/
GLFWwindow	*DCMain;
GLuint		tDollar, tC;

void		dollarC();
void		QIANSIP(int Mx, int My);
void		DCdraw();
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


	for(i = 0; i < sizeof(bDollar); i++) bDollar[i] = bDollar[i] == ' ' ? '\xFF' : '\x80';
	glGenTextures(1, &tDollar);
	glBindTexture(GL_TEXTURE_2D, tDollar);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 13, 13, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bDollar);

	for(i = 0; i < sizeof(bC); i++) bC[i] = bC[i] == ' ' ? '\xFF' : '\x80';
	glGenTextures(1, &tC);
	glBindTexture(GL_TEXTURE_2D, tC);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 13, 13, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bC);

	dollarC();

	glfwTerminate();

	// free(DCData);           //不要作死
	return 0;
}

#define DCMap_H 1024
#define DCMap_W 1024
char	DCData[DCMap_H][DCMap_W];
double	Px = 0, Py = 0, timer = 0, sp;		//Player
char	Sx = 0, Sy = 1;				//PlayerDirection
int	Ddelay, Drate, Dtimer = 0;		//Dollar
void dollarC()
{
	/* init */
	memset(DCData, 0, sizeof(DCData));

	while(!glfwWindowShouldClose(DCMain))
	{
		timer += 1 / 60.0;
		sp = 0.05 + timer * 0.0005;	//Player speed
		Ddelay = 60 / (1 + timer / 60); //$ 生成用的时间
		Drate = 120 / (1 + timer / 15); //$ 生成的频率

		/* move */
		{
			//if(glfwGetKey(DCMain, GLFW_KEY_F)) timer++;
			if(glfwGetKey(DCMain, GLFW_KEY_C)) Sx = 0, Sy = 1;
			if(glfwGetKey(DCMain, GLFW_KEY_B)) Sx = -1, Sy = 0;
			if(glfwGetKey(DCMain, GLFW_KEY_A)) Sx = 1, Sy = 0;
			if(glfwGetKey(DCMain, GLFW_KEY_D)) Sx = 0, Sy = -1;
			if(glfwGetKey(DCMain, GLFW_KEY_ESCAPE)) return;

			Px += Sx * sp;
			Py += Sy * sp;

			if(Px < 0) Px += MAP_SIZE;
			if(Py < 0) Py += MAP_SIZE;
			if(Px >= MAP_SIZE) Px -= MAP_SIZE;
			if(Py >= MAP_SIZE) Py -= MAP_SIZE;

			printf("Pos:%6.2lf,%6.2lf time:%6.2lf sp:%6.4lf ", Px, Py, timer, sp);
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
			Dtimer++;
			printf("$rate:%5d/%5d\r", Dtimer, Drate);
			if(Dtimer >= Drate)
			{
				h = rand() * 8 / RAND_MAX + 1;
				w = (rand() * h * 2) / RAND_MAX - h;
				x = (int) Px + h * Sx + w * Sy;
				y = (int) Py + w * (-Sx) + h * Sy;
				x = (x + MAP_SIZE) % MAP_SIZE;
				y = (y + MAP_SIZE) % MAP_SIZE;
				if(DCData[x][y] == 0) DCData[x][y] = 1; //Create $
				Dtimer = 0;
			}
		}

		/* step $ */
		{
			int	x, y;
			for(x = 0; x < MAP_SIZE; x++)
				for(y = 0; y < MAP_SIZE; y++)
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
					Mdollar = DCData[(Mx + MAP_SIZE) % MAP_SIZE][(My + MAP_SIZE) % MAP_SIZE];
					if(Mdollar >= Ddelay)
					{
						if
						(
							((Px - Pointx[x]) * (Px - Pointx[x])) +
								((Py - Pointy[y]) * (Py - Pointy[y])) < 0.16
						)
						{
							//Be Trapped By $
							QIANSIP(Mx, My);
							return;
						}
					}
				}
			}
		}

		/* draw */
		DCdraw();

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}
}

void QIANSIP(int Mx, int My)
{
	int	i, j;
	GtRect	gr[17];
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

	Gt_EnableTex(tC);

	for(i = 0; i < 17; i++)
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
		int	t = i / 8 + 1;
		if(glfwWindowShouldClose(DCMain)) return;
		if(t > 17) t = 17;
		for(j = 0; j < t; j++)
		{
			gr[j].x = gr[j].y += ((-0.8 - gr[j].x) * 0.1);
			gr[j].w = gr[j].h = -gr[j].x * 2;
			gr[j].a += ((1 - gr[j].a) * 0.05);
		}

		Gt_Draw(gr, t);
		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}

	Px = Mx;
	Py = My;
}

void DCdraw()
{
	GtRect	gr[17 * 17 + 1];
	int	x0 = (int) Px, y0 = (int) Py;
	int	x, y, Mx, My, Mdollar;

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	Gt_EnableTex(tDollar);

	for(x = 0; x < 17; x++)
	{
		for(y = 0; y < 17; y++)
		{
			Mx = x0 - 8 + x;
			My = y0 - 8 + y;
			Gt_SetPos(gr[x * 17 + y], (Mx - Px) / 8, (My - Py) / 8, 1 / 8.0, 1 / 8.0);
			Mdollar = DCData[(Mx + MAP_SIZE) % MAP_SIZE][(My + MAP_SIZE) % MAP_SIZE];
			if(Mdollar >= Ddelay)	//Have $
			{
				if(timer - (int) timer < 0.5)
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

	Gt_SetPos(gr[17 * 17], -0.4 / 8, -0.4 / 8, 0.8 / 8, 0.8 / 8);
	if(Sx == 0)
	{
		if(Sy == 1)
			Gt_SetColor(gr[17 * 17], 1, 1, 0.2, 1);
		else
			Gt_SetColor(gr[17 * 17], 0.2, 0.2, 1, 1);
	}
	else
	{
		if(Sx == -1)
			Gt_SetColor(gr[17 * 17], 1, 0.2, 0.2, 1);
		else
			Gt_SetColor(gr[17 * 17], 0.2, 1, 0.2, 1);
	}

	Gt_Draw(gr, 17 * 17);
	DrawCircle(0, 0, 0.4 / 8, &gr[17 * 17].r);
}

void DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4])
{
	int	i;
	GLfloat I[32][2];
	Gt_DisableTex();
	for(i = 0; i < 32; i++)
	{
		I[i][0] = x + r * cos(2 * M_PI * i / 32);
		I[i][1] = y + r * sin(2 * M_PI * i / 32);
	}

	glUseProgram(Gt_GP);
	glUniform4f(Gt_lColor, c[0], c[1], c[2], c[3]);
	glBindVertexArray(Gt_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(I), I, GL_STREAM_DRAW);
	glDrawArrays(GL_POLYGON, 0, 32);
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, w, h);
	glfwSetWindowAspectRatio(window, 1, 1);
}
