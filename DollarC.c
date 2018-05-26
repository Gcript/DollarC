#include <GtCui.h>
#include <Windows.h>
#include "ConsoleUI.h"
#include "DCEngine.h"




int main() {
	UIInit();
	Start();
	glfwInit();
	GLFWwindow	*DCMain = glfwCreateWindow(600, 600, "Dollar C", NULL, NULL);
	glfwMakeContextCurrent(DCMain);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	Gt_init(NULL, NULL);
	Gt_ScrSize(600, 600);
	glViewport(0, 0, 600, 600);

#define DCMap_H 1024
#define DCMap_W 1024
	static GtRect	DC[20 * 20 + 1];
	static _Bool *DCData[DCMap_H*DCMap_W];
	memset(DCData, 0, sizeof(DCData));
	register int DCKey;
	GLfloat PosX=rand()*DCMap_W/16384, PosY= rand()*DCMap_H / 16384;
	int Dx=0, Dy=0;
	int Wx, Wy;
	GLfloat Vx=0, Vy=0;
	while (Dy < 20) {
		while (Dx < 20) {
			Gt_SetPos(DC[Dy * 20 + Dx], -1.0 + 0.1*Dx, -1.0 + 0.1*Dy, 0.095, 0.095);
			Gt_SetColor(DC[Dy * 20 + Dx], 0.080, 0.080, 0.080, 0.5);
			Dx++;
		}
		Dx = 0;
		Dy++;
	}
	Gt_SetColor(DC[400], 0.87, 0.51, 0.15, 0.9);


	while (!glfwWindowShouldClose(DCMain))
	{
		{
			if (glfwGetKey(DCMain, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(DCMain, GLFW_TRUE);
			if (glfwGetKey(DCMain, GLFW_KEY_C) == GLFW_PRESS & PosY < (GLfloat)DCMap_H) {
				PosY += 0.05;
				if(Vy < 0.1)Vy += 0.004;
			}
			if (glfwGetKey(DCMain, GLFW_KEY_B) == GLFW_PRESS & PosX>(GLfloat)0){
				PosX -= 0.05; 
				if (Vx > -0.1)Vx -= 0.004;
			}
				
			if (glfwGetKey(DCMain, GLFW_KEY_A) == GLFW_PRESS & PosX < (GLfloat)DCMap_W) {
				PosX += 0.05;
				if (Vx < 0.1)Vx += 0.004;
			}
			if (glfwGetKey(DCMain, GLFW_KEY_D) == GLFW_PRESS & PosY > (GLfloat)0) {
				PosY -= 0.05;
				if (Vy > -0.1)Vy -= 0.004;
			}
		}//Key
		
		Wx = (int)PosX;
		Wy = (int)PosY;
		Dx = 0, Dy = 0;
		while (Dy < 20) {
			while (Dx < 20) {
				if (0);
				Gt_SetColor(DC[Dy * 20 + Dx], 0.080, 0.080, 0.080, 0.5);
				Dx++;
			}
			Dx = 0;
			Dy++;
		}

		Gt_SetPos(DC[400],Vx , Vy, 0.06, 0.06);
		glClearColor(0.075, 0.075,0.075, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		Gt_Draw(DC, 401);
		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}
	glfwTerminate();
	free(DCData);
	return 0;
}