#ifndef _GTCUI_H_
#define _GTCUI_H_

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb_easy_font.h>
#include <stb_image.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

/* TYPES */
typedef struct stGtRect
{
	GLfloat x, y, w, h;
	GLfloat r, g, b, a;
} GtRect;

/* VERIABLES */
GLuint	Gt_VAO, Gt_VBO, Gt_VBOtex, Gt_GP, Gt_Tex;
int	Gt_TexMode;
GLuint	Gt_lColor, Gt_lTexMode;
double	Gt_ScrW, Gt_ScrH;

/* FUNCTIONS & MACROS */

// Set screen size for Gt_input
#define Gt_ScrSize(W, H)	(Gt_ScrW = (W), Gt_ScrH = (H))

// Set GtRect position (X, Y) and size (W, H)
#define Gt_SetPos(_, X, Y, W, H)	((_).x = (X), (_).y = (Y), (_).w = (W), (_).h = (H))

// Set GtRect color (R, G, B) and alpha (A)
#define Gt_SetColor(_, R, G, B, A)	((_).r = (R), (_).g = (G), (_).b = (B), (_).a = (A))

// Enable texture drawing
#define Gt_EnableTex(Tex)	(Gt_TexMode = 1, Gt_Tex = Tex)

// Disable texture drawing (default)
#define Gt_DisableTex() (Gt_TexMode = 0)

// VSs and FSs are shader program source (GLSL)
void		Gt_init(const char *VSs, const char *FSs);

// Draw GtRect arrays
void		Gt_Draw(GtRect *g, int n);

// Check cursor in GtRect arrays
// return value: -1 - None 0-(n-1) - GtRect[ret];
int		Gt_Input(GtRect *g, int n, double xpos, double ypos);

/*$off*/
const char	*Gt_DefVSs =
	"#version 330 core\n"
	"layout (location = 0) in vec2 vPos;"//GtRect position
	"layout (location = 1) in vec2 vTex;"//Texture coord (0, 0)-(1, 1)
	"out vec2 fTex;"
	"void main(){"
		"gl_Position = vec4(vPos.x, vPos.y, 0, 1);"
		"fTex = vTex;"
	"}";
const char	*Gt_DefFSs =
	"#version 330 core\n"
	"uniform vec4 Color = vec4(0, 0, 0, 0);"//GtRect color
	"uniform sampler2D Texture;"//Always 0
	"uniform bool TexMode = false;"//Is Gt_Texture enabled
	"in vec2 fTex;"//Texture coord
	"out vec4 fColor;"
	"void main(){"
		"if(TexMode)"//Is Gt_Texture enabled?
			"fColor = texture(Texture, fTex);"//draw texture color
		"else "
			"fColor = Color;"//draw GtRect color
	"}";
/*$on*/
void Gt_init(const char *VSs, const char *FSs)
{
	if(VSs == NULL) VSs = Gt_DefVSs;
	if(FSs == NULL) FSs = Gt_DefFSs;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	{
		GLuint	GVS, GFS;
#ifdef GTCUI_DEBUG
		int	Status;
		char	infoLog[512];
#endif
		GVS = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(GVS, 1, &VSs, NULL);
		glCompileShader(GVS);
#ifdef GTCUI_DEBUG
		glGetShaderiv(GVS, GL_COMPILE_STATUS, &Status);
		if(!Status)
		{
			glGetShaderInfoLog(GVS, 512, NULL, infoLog);
			printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
		}
#endif
		GFS = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(GFS, 1, &FSs, NULL);
		glCompileShader(GFS);

#ifdef GTCUI_DEBUG
		glGetShaderiv(GFS, GL_COMPILE_STATUS, &Status);
		if(!Status)
		{
			glGetShaderInfoLog(GFS, 512, NULL, infoLog);
			printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
		}
#endif
		Gt_GP = glCreateProgram();
		glAttachShader(Gt_GP, GVS);
		glAttachShader(Gt_GP, GFS);
		glLinkProgram(Gt_GP);
#ifdef GTCUI_DEBUG
		glGetProgramiv(Gt_GP, GL_LINK_STATUS, &Status);
		if(!Status)
		{
			glGetProgramInfoLog(Gt_GP, 512, NULL, infoLog);
			printf("ERROR::PROGRAM::LINK_FAILED\n%s\n", infoLog);
		}
#endif
		glDeleteShader(GVS);
		glDeleteShader(GFS);

		Gt_lColor = glGetUniformLocation(Gt_GP, "Color");
		Gt_lTexMode = glGetUniformLocation(Gt_GP, "TexMode");
	}

	glUniform1i(glGetUniformLocation(Gt_GP, "Texture"), 0);
	glGenVertexArrays(1, &Gt_VAO);
	glBindVertexArray(Gt_VAO);

	glGenBuffers(1, &Gt_VBO);
	glGenBuffers(1, &Gt_VBOtex);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBOtex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(1);
}

void Gt_Draw(GtRect *g, int n)
{
	int		i;
	const GLfloat	Vt[][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };

	glBindBuffer(GL_ARRAY_BUFFER, Gt_VBOtex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vt), Vt, GL_DYNAMIC_DRAW);

	glUniform1i(Gt_lTexMode, Gt_TexMode);
	if(Gt_TexMode)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Gt_Tex);
	}

	for(i = 0; i < n; i++)
	{
		GLfloat V[][2] =
		{
			{ g[i].x, g[i].y },
			{ g[i].x + g[i].w, g[i].y },
			{ g[i].x, g[i].y + g[i].h },
			{ g[i].x + g[i].w, g[i].y + g[i].h }
		};

		glUniform4f(Gt_lColor, g[i].r, g[i].g, g[i].b, g[i].a);
		glUseProgram(Gt_GP);
		glBindVertexArray(Gt_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, Gt_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(V), V, GL_STREAM_DRAW);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}

int Gt_Input(GtRect *g, int n, double xpos, double ypos)
{
	int	i;
	xpos = xpos * 2 / Gt_ScrW - 1;
	ypos = 1 - ypos * 2 / Gt_ScrH;
	for(i = 0; i < n; i++)
		if(g[i].x <= xpos && xpos <= g[i].x + g[i].w && g[i].y <= ypos && ypos <= g[i].y + g[i].h) return i;
	return -1;
}
#endif /*_GTCUI_H_*/
