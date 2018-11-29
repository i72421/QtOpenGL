#pragma once

#include "loadShader.h"
#include "TextureLoader.h"

#include "gl\glew.h"
#include "GL\GL.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <vector>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#define  PI		3.1415926535898

typedef struct ElementCoord
{
	glm::vec3 Position;
	glm::vec2 TexCoord;

	ElementCoord(glm::vec3 p, glm::vec2 c)
	{
		Position = p;
		TexCoord = c;
	}
}ElementCoord;

class OGLWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OGLWidget(QWidget *parent = nullptr);
	 
	~OGLWidget();
	void setTexture(QString path);

protected:
	void initCylinder(float r = 5, float h = 10, int Horizontal = 50);
	void initCircle(float r = 50, int Horizontal = 50, int Vertical = 50);
	void reCalcMatrix();

	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int width, int height) override;

	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;


private:
	GLuint IndexBuffer;
	GLuint PosBuffer;
	GLuint UVBuffer;
	std::vector<GLuint> Indices;

	GLuint PosBuffer_Q;
	GLuint UVBuffer_Q;
	std::vector<glm::vec3> Pos;
	std::vector<glm::vec2> UV;

	GLuint VertexArrayID;
	GLuint programID;
	GLuint MatrixID;
	GLuint TextureID;
	GLuint PostionLocation;
	GLuint UVLocation;

	GLuint Texture;
	GLuint Texture_Q;


	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 modelMatrix;

	glm::vec3 position;
	float horizontalAngle;
	float verticalAngle;
	float initFov;

	float cameraSpeed;
	float mouseSpeed;

	QPoint mousePressPos;

	bool Cylinder;
};
