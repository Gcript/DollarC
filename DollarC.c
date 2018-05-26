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


	while (!glfwWindowShouldClose(DCMain))
	{
		//processInput(DCMain);

		glClearColor(0.075, 0.075,0.075, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(DCMain);
		glfwSwapInterval(1);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
