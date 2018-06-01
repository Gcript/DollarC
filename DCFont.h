void		LoadAllBt();
GLuint		LoadBt(int w, int h, const GLubyte *data);

/*$off*/
const GLubyte bNothing[] =
	" ";
const GLubyte bDollar[] =
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
const GLubyte bC[] =
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
const GLubyte bDolC[] =
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
const GLubyte bTip[] =						// Press ENTER to play
	"####                          #### #  # #### #### ####                           #            "
	"#  #                          #    ## #  #   #    #  #                           #            "
	"#  # #### #### #### ####      #    ## #  #   #    #  #       #             ####  #    ### #  #"
	"#### #  # #  # #    #         #### # ##  #   #### ####      ###  ####      #  #  #      # #  #"
	"#    #    #### #### ####      #    # ##  #   #    ##         #   #  #      #  #  #   #### ####"
	"#    #    #       #    #      #    #  #  #   #    # #        #   #  #      ####  #   #  #    #"
	"#    #    #### #### ####      #### #  #  #   #### #  #       ##  ####      #     #   #### ####";
const GLubyte *bN[11] = {
	"####"//0
	"#  #"
	"#  #"
	"#  #"
	"#  #"
	"#  #"
	"####",

	" ## "//1
	"  # "
	"  # "
	"  # "
	"  # "
	"  # "
	"  # ",

	" ###"//2
	"   #"
	"   #"
	"####"
	"#   "
	"#   "
	"### ",

	"####"//3
	"   #"
	"   #"
	" ###"
	"   #"
	"   #"
	"####",

	"# # "//4
	"# # "
	"# # "
	"####"
	"  # "
	"  # "
	"  # ",

	"### "//5
	"#   "
	"#   "
	"####"
	"   #"
	"   #"
	" ###",

	"### "//6
	"#   "
	"#   "
	"####"
	"#  #"
	"#  #"
	"####",

	"####"//7
	"   #"
	"   #"
	"  # "
	" #  "
	" #  "
	" #  ",

	"####"//8
	"#  #"
	"#  #"
	"####"
	"#  #"
	"#  #"
	"####",

	"####"//9
	"#  #"
	"#  #"
	"####"
	"   #"
	"   #"
	" ###",

	"    "//.
	"    "
	"    "
	"    "
	"    "
	"    "
	" #  ",
};
/*$on*/
GLuint		tNothing, tDollar, tDolC, tC, tTip;
GLuint		tN[11];
void LoadAllBt()
{
	int	i;
	tNothing = LoadBt(1, 1, bNothing);
	tDollar = LoadBt(13, 13, bDollar);
	tC = LoadBt(13, 13, bC);
	tDolC = LoadBt(13, 13, bDolC);
	tTip = LoadBt(94, 7, bTip);
	for(i = 0; i < 11; i++) tN[i] = LoadBt(4, 7, bN[i]);
}

GLuint LoadBt(int w, int h, const GLubyte *data)
{
	GLuint	ret;
	int	i;
	GLubyte *data0 = (GLubyte *) malloc(w * h * sizeof(GLubyte));
	for(i = 0; i < w * h; i++) data0[i] = data[i] == ' ' ? 0 : 0xFF;
	glGenTextures(1, &ret);
	glBindTexture(GL_TEXTURE_2D, ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data0);
	free(data0);
	return ret;
}
