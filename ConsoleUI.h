
void UIInit ()
{
	SetConsoleTitle("Dollar C");
	system("mode con cols=50 lines=20&color 8f");
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
	"    Press any key to start\n"
	/*$on*/
	);
	getch();
	FreeConsole();
}

void Return()
{
	AllocConsole();
}
