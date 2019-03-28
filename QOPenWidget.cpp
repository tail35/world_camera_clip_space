#include "QOPenWidget.h"
#include <iostream>
#include <qtimer.h>
#include<ctime>
#include <string.h>
#include <QImage>
#include <QGLWidget>//lib Qt5OpenGL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



QOPenWidget::QOPenWidget(QWidget *parent) :QOpenGLWidget(parent)
{
	setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);

	connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
	timer.start(0);
}
QOPenWidget::~QOPenWidget()
{

}

void QOPenWidget::initializeGL()
{
	initializeOpenGLFunctions();				//初始化
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);					//开启深度缓存

	//添加着色器
	CompileShader(&ID, "texture.vs", "texture.fs");

	float vertices[] = {
		// 位置             // 颜色				//texture
		0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f,0.0f,
	   -0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f,0.0f,
	   -0.5f, 0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f,1.0f,
		0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	1.0f,1.0f
	};
	//index is very important.
	unsigned int indices[] = {
		0, 1, 2, // first triangle
		2, 3, 0  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//绑定VAO
	glBindVertexArray(VAO);

	//绑定VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//把顶点数组复制到VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//解除绑定VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//解除绑定EBO
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//解除绑定VAO
	glBindVertexArray(0);

	//for textrue
	ReadImage();
	glUseProgram(ID); //don't forget to activate/use the shader before setting uniforms!
	glUniform1i(glGetUniformLocation(ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(ID, "texture2"), 1);

	

}



void QOPenWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void QOPenWidget::ReadImage()
{

	bool r1 = buf1.load("1.jpeg");
	bool r2 = buf2.load("2.jpeg");
	//bool r3 = buf1.load("3.jpeg");

	tex1 = QGLWidget::convertToGLFormat(buf1);
	tex2 = QGLWidget::convertToGLFormat(buf2);
	//tex3 = QGLWidget::convertToGLFormat(buf3);

	glGenTextures(2, texture);

	//texture 1	
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits());
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	//texture 2		
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex2.width(), tex2.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex2.bits());
	glGenerateMipmap(GL_TEXTURE_2D);
	

	//glBindTexture(GL_TEXTURE_2D, texture[2]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex3.width(), tex3.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex3.bits());

}

static float radias = 0.0;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void QOPenWidget::paintGL()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//激活着色器
	glUseProgram(ID);

	// create transformations
	glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// retrieve the matrix uniform locations
	unsigned int modelLoc = glGetUniformLocation(ID, "model");
	unsigned int viewLoc = glGetUniformLocation(ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(ID, "projection");
	// pass them to the shaders (3 different ways)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projectionLoc,1,GL_FALSE, &projection[0][0]);

	//绑定顶点缓存对象
	glBindVertexArray(VAO);
	//开始绘制
	//glDrawArrays(GL_TRIANGLES, 0, 3);//only one triangel
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//multi point.
	//解除绑定
	glBindVertexArray(0);
}


//加载着色器程序
void QOPenWidget::CompileShader(unsigned int *id, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;


	// 打开文件
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;
	// 读取文件的缓冲内容到数据流中
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();
	// 关闭文件处理器
	vShaderFile.close();
	fShaderFile.close();
	// 转换数据流到string
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();

	if (geometryPath != NULL)
	{
		gShaderFile.open(geometryPath);
		std::stringstream gShaderStream;
		gShaderStream << gShaderFile.rdbuf();
		gShaderFile.close();
		geometryCode = gShaderStream.str();
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. 编译着色器
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// 顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	//编译着色器
	glCompileShader(vertex);
	//检测编译时错误
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	//如果编译错误进入语句
	if (!success)
	{
		//获取错误信息
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		//打印错误
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	//片元着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	//检测错误
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//获取错误原因
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		//打印
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//几何着色器
	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
	}


	// 着色器程序
	*id = glCreateProgram();
	glAttachShader(*id, vertex);
	glAttachShader(*id, fragment);
	if (geometryPath != nullptr)
		glAttachShader(*id, geometry);
	glLinkProgram(*id);
	//检测错误
	glGetProgramiv(*id, GL_LINK_STATUS, &success);
	if (!success)
	{
		//获取错误信息
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
}