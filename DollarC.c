#include <GtCui.h>
#include <Windows.h>
#include "ConsoleUI.h"
#include "DCEngine.h"



int processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		return 1;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		return 2;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		return 3;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		return 4;
}

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

	int DCMap_H = 1024, DCMap_W = 1024;
	static GtRect	DC[20 * 20 + 1];
	static _Bool *DCData;	
	register int DCKey;
	DCData = (_Bool)malloc(DCMap_H * DCMap_W * sizeof(_Bool));
	int PosX=rand()*DCMap_W/16384, PosY= rand()*DCMap_H / 16384;
	int Dx=0, Dy=0;
	while (Dy < 20) {
		while (Dx < 21) {
			Gt_SetPos(DC[Dy * 20 + Dx], -1.0 + 0.1*Dx, -1.0 + 0.1*Dy, 0.095, 0.095);
			Gt_SetColor(DC[Dy * 20 + Dx], 0.080, 0.080, 0.080, 0.5);
		}
	}
	Gt_SetColor(DC[400], 0.87, 0.51, 0.15, 0.9);
	while (!glfwWindowShouldClose(DCMain))
	{
		DCKey = processInput(DCMain);
		Gt_SetPos(DC[400], 0.0, 0.0, 0.06, 0.06);
		glClearColor(0.075, 0.075,0.075, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		Gt_Draw(DC, 400);
		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}
	glfwTerminate();
	free(DCData);
	return 0;
}
