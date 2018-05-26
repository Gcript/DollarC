#include <stdio.h>
#include <conio.h>

void UIInit()
{
	SetConsoleTitle("Dollar C");
	system("mode con cols=30 lines=17&color 8f");
}

void Start()
{
	printf(/*$off*/
	"\n"
	"        |\n"
	"    ,---+----    ,--------\n"
	"    |   |        |\n"
	"    |   |        |\n"
	"    `---+---.    |\n"
	"        |   |    |\n"
	"        |   |    |\n"
	"    ----+---'    `--------\n"
	"        |\n"
	"\n"
	"Control:      C\n"
	"            B-+-A\n"
	"              D\n"
	"\n"
	"    Press any key to start"
	/*$on*/
	);
	getch();
	FreeConsole();
}

void Return()
{
	AllocConsole();
}
