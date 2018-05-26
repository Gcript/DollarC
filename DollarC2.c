#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <math.h>
#ifndef M_PI
#define M_PI	3.141592653589793238462643383
#endif
#include <time.h>
#define GTCUI_DEBUG
#include "GtCui.h"

#define MAP_SIZE	256

/*$off*/
const char	*DCVS =
	"#version 330 core\n"
	"layout (location = 0) in vec2 aPos;"
	"out vec2 Pos;"
	"void main(){"
		"gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);"
		"Pos = aPos;"
	"}";
const char	*DCFS =
	"#version 330 core\n"
	"in vec2 Pos;"
	"uniform vec4 Color;"
	"out vec4 FragColor;"
	"void main(){"
		"FragColor = Color;"
		"if(abs(Pos.x) > 0.8)"
			"FragColor = mix(FragColor, vec4(0, 0, 0, 0), (abs(Pos.x) - 0.8) * 5);"
		"if(abs(Pos.y) > 0.8)"
			"FragColor = mix(FragColor, vec4(0, 0, 0, 0), (abs(Pos.y) - 0.8) * 5);"
	"}";
/*$on*/
GLFWwindow	*window;

void		dollarC();
void		QIANSIP(int Mx, int My);
void		DCdraw();

void DrawCircle(GLfloat x, GLfloat y, GLfloat r, GLfloat c[4])
{
	int	i;
	GLfloat I[32][2];
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

int main()
{
	UIinit();
	srand(time(NULL));
	{		// Prepare for glfw
		glfwInit();
		window = glfwCreateWindow(600, 600, "$C", NULL, NULL);
		if(window == NULL)
		{
			puts("Failed to create GLFW window");
			goto glfwError;
		}

		glfwMakeContextCurrent(window);
		if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		{
			puts("Failed to initialize GLAD");
			goto glfwError;
		}

		glViewport(0, 0, 600, 600);
	}

	Gt_init(DCVS, DCFS);

	//Gt_ScrSize(800, 600);    //Gt_Input
	dollarC();

	glfwTerminate();
	return 0;

glfwError:
	glfwTerminate();
	return 1;
}

/*
Control:

      C
      ^
      |
A <---O---> B
      |
      v
      D
*/
char	map[MAP_SIZE][MAP_SIZE];
double	Px = 0, Py = 0, timer = 0, sp;
int	Ddelay, Drate, Dtimer = 0;
char	Sx = 0, Sy = 1; // C
void dollarC()
{
	/* init */
	memset(map, 0, sizeof(map));

	while(!glfwWindowShouldClose(window))
	{
		timer += 1 / 60.0;
		sp = 0.05 + timer * 0.0005;	//Player speed
		Ddelay = 60 / (1 + timer / 60); //$ 生成用的时间
		Drate = 120 / (1 + timer / 15); //$ 生成的频率

		/* move */
		{
			//if(glfwGetKey(window, GLFW_KEY_F)) timer++;
			if(glfwGetKey(window, GLFW_KEY_C)) Sx = 0, Sy = 1;
			if(glfwGetKey(window, GLFW_KEY_B)) Sx = 1, Sy = 0;
			if(glfwGetKey(window, GLFW_KEY_A)) Sx = -1, Sy = 0;
			if(glfwGetKey(window, GLFW_KEY_D)) Sx = 0, Sy = -1;
			if(glfwGetKey(window, GLFW_KEY_ESCAPE)) return;

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
				if(map[x][y] == 0) map[x][y] = 1;	//Create $
				Dtimer = 0;
			}
		}

		/* step $ */
		{
			int	x, y;
			for(x = 0; x < MAP_SIZE; x++)
				for(y = 0; y < MAP_SIZE; y++)
					if(0 < map[x][y] && map[x][y] < Ddelay) map[x][y]++;	//增加 $ 出现的时间
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
					Mdollar = map[(Mx + MAP_SIZE) % MAP_SIZE][(My + MAP_SIZE) % MAP_SIZE];
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

		glfwSwapBuffers(window);
		glfwSwapInterval(1);
		glfwPollEvents();
	}
}

void QIANSIP(int Mx, int My)
{
	int	i, j;
	double	Px0 = Px, Py0 = Py;
	GtRect	gr[17];
	for(i = 0; i < 64; i++)
	{
		Px = Px0 + (Mx + 0.5 - Px0) * i / 64;
		Py = Py0 + (My + 0.5 - Py0) * i / 64;
		DCdraw();
		glfwSwapBuffers(window);
		glfwSwapInterval(1);
		glfwPollEvents();
	}

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
		if(t > 17) t = 17;
		for(j = 0; j < t; j++)
		{
			gr[j].x = gr[j].y = -0.8 - ((-0.8 - gr[j].x) * 0.9);
			gr[j].w = gr[j].h = -gr[j].x * 2;
			gr[j].a = 1 - ((1 - gr[j].a) * 0.95);
		}

		Gt_Draw(gr, t);
		glfwSwapBuffers(window);
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

	for(x = 0; x < 17; x++)
	{
		for(y = 0; y < 17; y++)
		{
			Mx = x0 - 8 + x;
			My = y0 - 8 + y;
			Gt_SetPos(gr[x * 17 + y], (Mx - Px) / 8, (My - Py) / 8, 1 / 8.0, 1 / 8.0);
			Mdollar = map[(Mx + MAP_SIZE) % MAP_SIZE][(My + MAP_SIZE) % MAP_SIZE];
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
