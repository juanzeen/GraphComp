#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GL/glut.h"

#define MAXVERTEXS 30
#define NPOLYGON 6
#define PHI 3.141572

#define HERMITE 1
#define BEZIER 2
#define BSPLINE 3
#define CATMULLR 4

#define TRANSLACAO 1
#define ROTACAO 2
#define SCALA 3
#define CISALHA 4
#define ESPELHARX 5
#define ESPELHARY 6

GLenum doubleBuffer;

typedef struct spts
{
	float v[3];
} tipoPto;

tipoPto ptsCurva[MAXVERTEXS];
tipoPto ptsContrle[MAXVERTEXS];

int windW, windH;
int nPtsCtrole, nPtsCurva;

int jaCurva = 0;
int ptoSelect = -1;
int tipoCurva = 0;
int tipoTransforma = 0;

float M[4][4] =
		{{0.0, 0.0, 0.0, 0.0},
		 {0.0, 0.0, 0.0, 0.0},
		 {0.0, 0.0, 0.0, 0.0},
		 {0.0, 0.0, 0.0, 0.0}};

float Mh[4][4] =
		{{2.0, -2.0, 1.0, 1.0},
		 {-3.0, 3.0, -2.0, -1.0},
		 {0.0, 0.0, 1.0, 0.0},
		 {1.0, 0.0, 0.0, 0.0}};

float Mb[4][4] =
		{{-1.0, 3.0, -3.0, 1.0},
		 {3.0, -6.0, 3.0, 0.0},
		 {-3.0, 3.0, 0.0, 0.0},
		 {1.0, 0.0, 0.0, 0.0}};

float Ms[4][4] =
		{{-1.0, 3.0, -3.0, 1.0},
		 {3.0, -6.0, 3.0, 0.0},
		 {-3.0, 0.0, 3.0, 0.0},
		 {1.0, 4.0, 1.0, 0.0}};

float Mc[4][4] =
		{{-1.0, 3.0, -3.0, 1.0},
		 {2.0, -5.0, 4.0, -1.0},
		 {-1.0, 0.0, 1.0, 0.0},
		 {0.0, 2.0, 0.0, 0.0}};

float MCor[9][3] =
		{
				{1.0, 0.5, 0.0},
				{0.8, 0.7, 0.5},
				{0.5, 0.5, 0.5},
				{0.5, 1.0, 0.5},
				{0.5, 0.5, 1.0},
				{1.0, 0.5, 1.0},
				{0.0, 0.0, 1.0},
				{0.0, 1.0, 0.0},
				{1.0, 0.0, 0.0}};
int nCors = 9;

void multMat3Vec(const float A[3][3], const float v[3], float out[3])
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		out[i] = 0.0f;
		for (j = 0; j < 3; j++)
			out[i] += A[i][j] * v[j];
	}
}

void applyTransformToControl(const float T[3][3])
{
	int i;
	for (i = 0; i < nPtsCtrole; i++)
	{
		float vin[3] = {ptsContrle[i].v[0], ptsContrle[i].v[1], 1.0f};
		float vout[3];
		multMat3Vec(T, vin, vout);
		ptsContrle[i].v[0] = vout[0];
		ptsContrle[i].v[1] = vout[1];
	}
}

void translacao(float dx, float dy)
{
	float T[3][3] = {{1, 0, dx}, {0, 1, dy}, {0, 0, 1}};
	applyTransformToControl(T);
}

void escala_centro(float sx, float sy)
{

	float cx = 0, cy = 0;
	for (int i = 0; i < nPtsCtrole; i++)
	{
		cx += ptsContrle[i].v[0];
		cy += ptsContrle[i].v[1];
	}
	cx /= nPtsCtrole;
	cy /= nPtsCtrole;
	float T1[3][3] = {{1, 0, -cx}, {0, 1, -cy}, {0, 0, 1}};
	float S[3][3] = {{sx, 0, 0}, {0, sy, 0}, {0, 0, 1}};
	float T2[3][3] = {{1, 0, cx}, {0, 1, cy}, {0, 0, 1}};

	float tmp[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			tmp[i][j] = 0;
			for (int k = 0; k < 3; k++)
				tmp[i][j] += S[i][k] * T1[k][j];
		}
	float R[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = 0;
			for (int k = 0; k < 3; k++)
				R[i][j] += T2[i][k] * tmp[k][j];
		}
	applyTransformToControl(R);
}

void rotacao_centro(float ang_deg)
{
	float ang = ang_deg * (PHI / 180.0f);
	float c = cos(ang), s = sin(ang);
	float cx = 0, cy = 0;
	for (int i = 0; i < nPtsCtrole; i++)
	{
		cx += ptsContrle[i].v[0];
		cy += ptsContrle[i].v[1];
	}
	cx /= nPtsCtrole;
	cy /= nPtsCtrole;
	float T1[3][3] = {{1, 0, -cx}, {0, 1, -cy}, {0, 0, 1}};
	float Rm[3][3] = {{c, -s, 0}, {s, c, 0}, {0, 0, 1}};
	float T2[3][3] = {{1, 0, cx}, {0, 1, cy}, {0, 0, 1}};
	float tmp[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			tmp[i][j] = 0;
			for (int k = 0; k < 3; k++)
				tmp[i][j] += Rm[i][k] * T1[k][j];
		}
	float R[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = 0;
			for (int k = 0; k < 3; k++)
				R[i][j] += T2[i][k] * tmp[k][j];
		}
	applyTransformToControl(R);
}

void shear(float shx, float shy)
{
	float cx = 0, cy = 0;
	for (int i = 0; i < nPtsCtrole; i++)
	{
		cx += ptsContrle[i].v[0];
		cy += ptsContrle[i].v[1];
	}
	cx /= nPtsCtrole;
	cy /= nPtsCtrole;
	float T1[3][3] = {{1, 0, -cx}, {0, 1, -cy}, {0, 0, 1}};
	float Sh[3][3] = {{1, shx, 0}, {shy, 1, 0}, {0, 0, 1}};
	float T2[3][3] = {{1, 0, cx}, {0, 1, cy}, {0, 0, 1}};
	float tmp[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			tmp[i][j] = 0;
			for (int k = 0; k < 3; k++)
				tmp[i][j] += Sh[i][k] * T1[k][j];
		}
	float R[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = 0;
			for (int k = 0; k < 3; k++)
				R[i][j] += T2[i][k] * tmp[k][j];
		}
	applyTransformToControl(R);
}

void espelharY()
{
	float cx = 0, cy = 0;
	for (int i = 0; i < nPtsCtrole; i++)
	{
		cx += ptsContrle[i].v[0];
		cy += ptsContrle[i].v[1];
	}
	cx /= nPtsCtrole;
	cy /= nPtsCtrole;
	float T1[3][3] = {{1, 0, -cx}, {0, 1, -cy}, {0, 0, 1}};
	float Mx[3][3] = {{1, 0, 0}, {0, -1, 0}, {0, 0, 1}};
	float T2[3][3] = {{1, 0, cx}, {0, 1, cy}, {0, 0, 1}};
	float tmp[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			tmp[i][j] = 0;
			for (int k = 0; k < 3; k++)
				tmp[i][j] += Mx[i][k] * T1[k][j];
		}
	float R[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = 0;
			for (int k = 0; k < 3; k++)
				R[i][j] += T2[i][k] * tmp[k][j];
		}
	applyTransformToControl(R);
}

void espelharX()
{
	float cx = 0, cy = 0;
	for (int i = 0; i < nPtsCtrole; i++)
	{
		cx += ptsContrle[i].v[0];
		cy += ptsContrle[i].v[1];
	}
	cx /= nPtsCtrole;
	cy /= nPtsCtrole;
	float T1[3][3] = {{1, 0, -cx}, {0, 1, -cy}, {0, 0, 1}};
	float My[3][3] = {{-1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	float T2[3][3] = {{1, 0, cx}, {0, 1, cy}, {0, 0, 1}};
	float tmp[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			tmp[i][j] = 0;
			for (int k = 0; k < 3; k++)
				tmp[i][j] += My[i][k] * T1[k][j];
		}
	float R[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = 0;
			for (int k = 0; k < 3; k++)
				R[i][j] += T2[i][k] * tmp[k][j];
		}
	applyTransformToControl(R);
}

void ptoCurva(float t, int j, float pp[3])
{
	int i, ji;
	float cc;
	tipoPto ptsCont[4];

	pp[0] = pp[1] = pp[2] = 0.0;

	for (i = 0; i < 4; i++)
	{
		ji = (j + i) % nPtsCtrole;
		ptsCont[i].v[0] = ptsContrle[ji].v[0];
		ptsCont[i].v[1] = ptsContrle[ji].v[1];
		ptsCont[i].v[2] = ptsContrle[ji].v[2];
	}

	if (tipoCurva == HERMITE)
	{
		float px_minus = ptsContrle[(j + nPtsCtrole - 1) % nPtsCtrole].v[0];
		float py_minus = ptsContrle[(j + nPtsCtrole - 1) % nPtsCtrole].v[1];
		float px_plus2 = ptsContrle[(j + 2) % nPtsCtrole].v[0];
		float py_plus2 = ptsContrle[(j + 2) % nPtsCtrole].v[1];

		ptsCont[2].v[0] = (ptsCont[1].v[0] - px_minus) * 0.5f;
		ptsCont[2].v[1] = (ptsCont[1].v[1] - py_minus) * 0.5f;
		ptsCont[3].v[0] = (px_plus2 - ptsCont[0].v[0]) * 0.5f;
		ptsCont[3].v[1] = (py_plus2 - ptsCont[0].v[1]) * 0.5f;
	}

	for (i = 0; i < 4; i++)
	{
		cc = t * t * t * M[0][i] + t * t * M[1][i] + t * M[2][i] + M[3][i];
		pp[0] += cc * ptsCont[i].v[0];
		pp[1] += cc * ptsCont[i].v[1];
		pp[2] += cc * ptsCont[i].v[2];
	}
}

void init(void)
{
	int i;

	jaCurva = 0;

	nPtsCtrole = 0;
	nPtsCurva = 0;

	for (i = 0; i < MAXVERTEXS; i++)
	{
		ptsContrle[i].v[0] = 0.0;
		ptsContrle[i].v[1] = 0.0;
		ptsContrle[i].v[2] = 0.0;

		ptsCurva[i].v[0] = 0.0;
		ptsCurva[i].v[1] = 0.0;
		ptsCurva[i].v[2] = 0.0;
	}
}

static void Key(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
	}
}

void coord_line(void)
{
	glLineWidth(1);

	glColor3f(0.0, 0.0, 1.0);

	glBegin(GL_LINE_STRIP);
	glVertex2f(-windW, 0);
	glVertex2f(windW, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, -windH);
	glVertex2f(0, windH);
	glEnd();
}

void verticesDraw(int n, tipoPto vertices[MAXVERTEXS], float cr, float cg, float cb)
{
	int i;
	glColor3f(cb, cg, cr);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (i = 0; i < n; i++)
		glVertex2f(vertices[i].v[0], vertices[i].v[1]);
	glEnd();
}

void Poligono(int n, tipoPto vertices[MAXVERTEXS], float cb, float cg, float cr)
{
	int i;

	glColor3f(cb, cg, cr);

	if (n > 2)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_LINE_STRIP);
	for (i = 0; i < n; i++)
	{
		glVertex2fv(vertices[i].v);
	}
	glEnd();
}

void geraCurva(int j)
{
	float t = 0.0;
	float dt;

	if (nPtsCtrole > 3)
	{
		dt = 1.0f / ((float)(MAXVERTEXS - 1));
		for (nPtsCurva = 0; nPtsCurva < MAXVERTEXS; nPtsCurva++)
		{
			t = (float)(nPtsCurva)*dt;
			ptoCurva(t, j, ptsCurva[nPtsCurva].v);
		}
		jaCurva = 1;
	}
}

static void Draw(void)
{
	int c, j = 0;
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	coord_line();

	verticesDraw(nPtsCtrole, ptsContrle, 1.0, 0.0, 0.0);
	Poligono(nPtsCtrole, ptsContrle, 0.0, 0.0, 0.0);

	if (jaCurva)
		while (j < nPtsCtrole)
		{
			geraCurva(j);
			c = j % (nCors - 3);

			Poligono(nPtsCurva, ptsCurva, MCor[c][0], MCor[c][1], MCor[c][2]);
			j++;
		}

	if (doubleBuffer)
	{
		glutSwapBuffers();
	}
	else
	{
		glFlush();
	}
}

static void Args(int argc, char **argv)
{
	GLint i;

	doubleBuffer = GL_FALSE;

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-sb") == 0)
		{
			doubleBuffer = GL_FALSE;
		}
		else if (strcmp(argv[i], "-db") == 0)
		{
			doubleBuffer = GL_TRUE;
		}
	}
}

void processMenuCurvas(int option)
{
	int i, j;

	tipoCurva = option;
	if (nPtsCtrole > 3)
	{
		switch (option)
		{
		case HERMITE:
			jaCurva = 1;
			for (i = 0; i < 4; i++) //
				for (j = 0; j < 4; j++)
					M[i][j] = Mh[i][j];
			break;

		case BEZIER:
			jaCurva = 1;
			for (i = 0; i < 4; i++)
				for (j = 0; j < 4; j++)
					M[i][j] = Mb[i][j];
			break;

		case BSPLINE:
			jaCurva = 1;
			for (i = 0; i < 4; i++)
				for (j = 0; j < 4; j++)
					M[i][j] = Ms[i][j] / 6.0f;
			break;

		case CATMULLR:
			jaCurva = 1;
			for (i = 0; i < 4; i++)
				for (j = 0; j < 4; j++)
					M[i][j] = Mc[i][j] / 2.0f;
			break;
		}
	}
	glutPostRedisplay();
}

void processMenuTransforma(int option)
{
	printf("Transforma opcao %d \n", option);

	tipoTransforma = option;

	switch (option)
	{
	case TRANSLACAO:
		printf("\n Translacao....\n");
		translacao(20.0f, 15.0f);
		break;

	case ROTACAO:
		printf("\n Rotacao....\n");
		rotacao_centro(15.0f);
		break;

	case SCALA:
		printf("\n Scala....\n");
		escala_centro(1.1f, 1.1f);
		break;

	case CISALHA:
		printf("\n Cisalha....\n");
		shear(0.2f, 0.0f);
		break;

	case ESPELHARX:
		printf("\n Espelhar X....\n");
		espelharX();
		break;

	case ESPELHARY:
		printf("\n Espelhar Y....\n");
		espelharY();
		break;

		break;
	}

	jaCurva = 0;
	glutPostRedisplay();
}

void processMenuEvents(int option)
{
	printf("Menu opcao %d \n", option);

	switch (option)
	{
	case 1:
		Poligono(nPtsCtrole, ptsContrle, 0.0, 1.0, 0.0);
		break;
	case 2:
		init();
		break;
	}
	glutPostRedisplay();
}

void createGLUTMenus()
{
	int menu, submenu1, submenu2;

	submenu1 = glutCreateMenu(processMenuCurvas);
	glutAddMenuEntry("Hermite", HERMITE);
	glutAddMenuEntry("Bezier", BEZIER);
	glutAddMenuEntry("B-Spline", BSPLINE);
	glutAddMenuEntry("CatmullR", CATMULLR);

	submenu2 = glutCreateMenu(processMenuTransforma);
	glutAddMenuEntry("Translacao", TRANSLACAO);
	glutAddMenuEntry("Rotacao", ROTACAO);
	glutAddMenuEntry("Escala", SCALA);
	glutAddMenuEntry("Cisalha", CISALHA);
	glutAddMenuEntry("Espelhar X", ESPELHARX);
	glutAddMenuEntry("Espelhar Y", ESPELHARY);
	glutAddMenuEntry("Manipular ponto", 0);

	menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Poligono de Controle", 1);
	glutAddSubMenu("Tipo de Curva", submenu1);
	glutAddSubMenu("Transformacao", submenu2);
	glutAddMenuEntry("Limpar tudo...", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void motion(int x, int y)
{
	x = x - windW;
	y = windH - y;
	if (jaCurva)
		if (!tipoTransforma)
		{
			if (ptoSelect >= 0 && ptoSelect < nPtsCtrole)
			{
				ptsContrle[ptoSelect].v[0] = (float)x;
				ptsContrle[ptoSelect].v[1] = (float)y;
			}
		}
		else
		{
			printf(" transformando, ");
			switch (tipoTransforma)
			{
			case TRANSLACAO:
				printf(" Translacao, ");
				translacao(1.0f, 0.5f);
				break;

			case ROTACAO:
				printf(" Rotacao, ");
				rotacao_centro(1.0f);
				break;

			case SCALA:
				printf(" Escala Centro, ");
				escala_centro(1.01f, 1.01f);
				break;

			case CISALHA:
				printf(" Cisalha, ");
				shear(0.01f, 0.0f);
				break;

				break;
			}
		}

	glutPostRedisplay();
}

int buscaPuntoClick(int x, int y)
{
	int i;
	float dx, dy, dd;

	ptoSelect = -1;
	for (i = 0; i < nPtsCtrole; i++)
	{
		dx = ptsContrle[i].v[0] - (float)x;
		dy = ptsContrle[i].v[1] - (float)y;
		dd = sqrt(dx * dx + dy * dy);
		if (dd < 6.00)
		{
			ptoSelect = i;
			break;
		}
	}
	return ptoSelect;
}

void mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT)
		if (state == GLUT_DOWN)
		{
			x = x - windW;
			y = windH - y;
			if (!jaCurva)
			{

				glColor3f(0.0, 0.0, 1.0);
				glPointSize(3);
				glBegin(GL_POINTS);
				glVertex2i(x, y);
				glEnd();

				ptsContrle[nPtsCtrole].v[0] = (float)x;
				ptsContrle[nPtsCtrole].v[1] = (float)y;
				nPtsCtrole++;
			}
			else // todos os pontos de controle ja estao definidos
			{

				ptoSelect = buscaPuntoClick(x, y);
				if ((ptoSelect >= 0) && (ptoSelect < nPtsCtrole))
				{
					glColor3f(1.0, 0.0, 0.0);
					glPointSize(6);
					glBegin(GL_POINTS);
					glVertex2i(x, y);
					glEnd();
				}
			}
			glutPostRedisplay();
		}
}

static void Reshape(int width, int height)
{
	windW = width / 2;
	windH = height / 2;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-windW, windW, -windH, windH);

	glMatrixMode(GL_MODELVIEW);

	Draw();
}

int main(int argc, char **argv)
{
	GLenum type;

	glutInit(&argc, argv);
	Args(argc, argv);

	type = GLUT_RGB;
	type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;

	glutInitDisplayMode(type);
	glutInitWindowSize(600, 500);
	glutCreateWindow("Curvas Parametricas Fechadas - Exercicio");

	init();

	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	glutDisplayFunc(Draw);

	glutMotionFunc(motion);
	glutMouseFunc(mouse);

	createGLUTMenus();

	glutMainLoop();

	return (0);
}
