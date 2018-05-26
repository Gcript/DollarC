

void UIInit() {
	SetConsoleTitle("Dollar C");
	system("mode con cols=73 lines=15&color 8f");
}

void Start() {
	printf("\n"
		     "\n"
		     "                              Dollar C                                  \n"
			 "\n"
		     "\n"
			 "\n"
		     "\n"
		     "               C                                                        \n"
			 "   Control:   B+A                  Press any key to start               \n"
	   		 "               D                                                        \n"
	);
	system("pause>null");
	FreeConsole();
}

void Return() {
	AllocConsole();
}