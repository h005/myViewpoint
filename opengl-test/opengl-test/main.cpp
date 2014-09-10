#include <GL/glut.h>
#include <stdio.h>

void printFloatv(int mode, char *title) {
	GLfloat v[16];
	printf("%s\n", title);
	glGetFloatv(mode, v);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", v[j * 4 + i]);
		}
		printf("\n");
	}
	printf("\n");
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity();             /* clear the matrix */
	/* viewing transformation  */
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// 等价于
	//glTranslatef(0, 0, -5);
	printFloatv(GL_MODELVIEW_MATRIX, "GL_MODELVIEW_MATRIX");
	glScalef(1.0, 2.0, 1.0);      /* modeling transformation */
	printFloatv(GL_MODELVIEW_MATRIX, "GL_MODELVIEW_MATRIX");
	glutWireCube(1.0);
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}