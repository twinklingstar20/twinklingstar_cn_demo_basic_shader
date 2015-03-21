/************************************************************************		
\link	www.twinklingstar.cn
\author Twinkling Star
\date	2013/12/03
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>


float g_lightPos[4] = {1,0.5,1,0};

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0,0.0,0.0,-1.0,0.0f,1.0f,0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, g_lightPos);
	glutSolidTeapot(1);

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) 
		exit(0);
}

char* readShaderSource(const char *fileName) 
{
	FILE *fp;
	char *content = NULL;
	int count=0;

	if (fileName != NULL) 
	{
		fp = fopen(fileName,"rt");

		if (fp != NULL) 
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) 
			{
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = NULL;
			}
			fclose(fp);
		}
	}
	return content;
}

GLuint genShader(GLenum type,const char* fileName,char*& log)
{
	//������ɫ������
	GLuint shader = glCreateShader(type);
	//���ļ��ж�ȡ��ɫ����ʵ�ִ���
	char* shaderSource = readShaderSource(fileName);
	if( !shaderSource )
		return 0;
	const char* ptrShaderSource = shaderSource;
	//����ɫ����ʵ�ִ����봴������ɫ�������
	glShaderSource(shader,1,&ptrShaderSource,NULL);
	free(shaderSource);
	//������ɫ������
	glCompileShader(shader);
	GLint status = 0;
	//�鿴����״̬
	glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
	if( !status )
	{
		GLint length;
		//��ȡ��־��Ϣ�ĳ���
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
		log = (GLchar*)malloc(length);
		//��ȡ��־��Ϣ
		glGetShaderInfoLog(shader,length,&length,log);
#if 1
		printf("%s\n",log);
#endif
		//ɾ����ɫ������
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint linkProgram(GLuint* shader,int shaderNum,char*& log)
{
	//������ɫ������
	GLuint program = glCreateProgram();
	int i;
	//����ɫ�������м�����ɫ������
	for( i=0 ; i<shaderNum ; i++ )
		glAttachShader(program,shader[i]);
	//������ɫ������
	glLinkProgram(program);
	GLint status;
	//�鿴����״̬
	glGetProgramiv(program,GL_LINK_STATUS,&status);
	if( !status )
	{
		GLint length;
		//��ȡ��־��Ϣ�ĳ���
		glGetProgramiv(program,GL_INFO_LOG_LENGTH,&length);
		log = (GLchar*)malloc(length);
		//��ȡ��־��Ϣ
		glGetProgramInfoLog(program,length,&length,log);
#if 1
		printf("%s\n",log);
#endif
		//ɾ����ɫ������
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

void useProgram(GLuint program)
{
	//���д����ɹ�����ɫ������
	glUseProgram(program);
}

bool setShaders()
{
	char* log = NULL;
	//����һ��������ɫ������
	GLuint vertexShader = genShader(GL_VERTEX_SHADER,"toon.vert",log);
	if( !vertexShader )
	{
		free(log);
		return false;
	}
	//����һ��Ƭ����ɫ������
	GLuint fragmentShader = genShader(GL_FRAGMENT_SHADER,"toon.frag",log);
	if( !fragmentShader )
	{
		free(log);
		glDeleteShader(vertexShader);
		return false;
	}

	//�Ѵ����õĶ����Ƭ����ɫ���������ӵ���ɫ��������
	GLuint shader[2] = {vertexShader,fragmentShader};
	GLuint program = linkProgram(shader,2,log);
	if( !program )
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		free(log);
		return false;
	}
	//ʹ�ô����ɹ�����ɫ������
	useProgram(program);

	return true;
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Shader-Demo");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);

	
	glewInit();
	//�ж��Ƿ�֧��GLSL
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else
	{
		printf("No GLSL support\n");
		exit(1);
	}

	setShaders();
	glutMainLoop();

	return 0;
}

