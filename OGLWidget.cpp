#include "OGLWidget.h"

OGLWidget::OGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

OGLWidget::~OGLWidget()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void OGLWidget::initCylinder(float r, float h, int Horizontal)
{
	std::vector<ElementCoord> vertexData;
	std::vector<ElementCoord> m_DrawData;

	float step = 2 * PI / Horizontal;

	for (int i = 0; i <= Horizontal; i++)
	{
		float angle = i * step;

		float x = r * cos(angle);
		float y = -h / 2;
		float z = r * sin(angle);

		float u = 1 - (float)i / Horizontal;
		float v = 0;
		vertexData.push_back(ElementCoord(glm::vec3(x, y, z), glm::vec2(u, v)));

		y = h / 2;
		v = 1;
		vertexData.push_back(ElementCoord(glm::vec3(x, y, z), glm::vec2(u, v)));
	}

	for (int i = 0; i < Horizontal; i++)
	{
		m_DrawData.push_back(vertexData[2 * i]);
		m_DrawData.push_back(vertexData[2 * i + 2]);
		m_DrawData.push_back(vertexData[2 * i + 1]);

		m_DrawData.push_back(vertexData[2 * i + 1]);
		m_DrawData.push_back(vertexData[2 * i + 2]);
		m_DrawData.push_back(vertexData[2 * i + 3]);
	}

	for (auto i : m_DrawData)
	{
		Position.push_back(i.Position);
		TexCoord.push_back(i.TexCoord);
	}
}

void OGLWidget::initCircle(float r, int Horizontal, int Vertical)
{
	std::vector<ElementCoord> vertexData;
	std::vector<ElementCoord> m_DrawData;

	double jd_step = PI * 2 / Horizontal;
	double wd_step = PI / Vertical;

	for (int j = 0; j <= Vertical; j++)
	{
		for (int i = 0; i <= Horizontal; i++)
		{
			int iTemp = i;
			int jTemp = j;

			float wd = wd_step * jTemp;
			float jd = jd_step * iTemp;

			float x = r * sin(wd) * cos(jd);
			float z = r * sin(wd) * sin(jd);
			float y = r * cos(wd);

			float u = (float)i / Horizontal;
			float v = (float)j / Vertical;

			vertexData.push_back(ElementCoord(glm::vec3(x, y, z), glm::vec2(1 - u, 1 - v)));
		}
	}

	for (int j = 0; j < Vertical; j++)
	{
		for (int i = 0; i < Horizontal; i++)
		{
			int num = j * (Horizontal + 1) + i;
			int numUp = num + (Horizontal + 1);

			m_DrawData.push_back(vertexData[num]);
			m_DrawData.push_back(vertexData[numUp]);
			m_DrawData.push_back(vertexData[num + 1]);

			m_DrawData.push_back(vertexData[numUp]);
			m_DrawData.push_back(vertexData[numUp + 1]);
			m_DrawData.push_back(vertexData[num + 1]);
		}
	}

	for (auto i : m_DrawData)
	{
		Position.push_back(i.Position);
		TexCoord.push_back(i.TexCoord);
	}
}

void OGLWidget::reCalcMatrix()
{
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
	glm::vec3 up = glm::cross(right, direction);

	ProjectionMatrix = glm::perspective(glm::radians(initFov), (float)width() / height(), 0.1f, 100.0f);
	ViewMatrix = glm::lookAt(position, position + direction, up);

	update();
}

void OGLWidget::initializeGL()
{
	if (glewInit() != GLEW_OK)
	{
		return;
	}

	initCircle();
	reCalcMatrix();

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDepthFunc(GL_LESS);		// Accept fragment if it closer to the camera than the former one

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	programID = LoadShaders("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");
	MatrixID = glGetUniformLocation(programID, "MVP");
	TextureID = glGetUniformLocation(programID, "myTexture");
	PostionLocation = glGetAttribLocation(programID, "vertexPosition");
	UVLocation = glGetAttribLocation(programID, "vertexUV");

	Texture = TextureLoader::LoadTexture("C:\\Users\\Administrator\\Pictures\\7.png");

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Position.size(), &Position[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * TexCoord.size(), &TexCoord[0], GL_STATIC_DRAW);
}

void OGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);

	//control
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix *modelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);

	// Draw
	glEnableVertexAttribArray(PostionLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(PostionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(UVLocation);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, Position.size());

	glDisableVertexAttribArray(PostionLocation);
	glDisableVertexAttribArray(UVLocation);

	glFinish();
}

void OGLWidget::resizeGL(int w, int h)
{
	reCalcMatrix();
}

void OGLWidget::keyPressEvent(QKeyEvent * event)
{
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
	glm::vec3 up = glm::cross(right, direction);


	switch (event->key())
	{
	case Qt::Key_Right:
		position += cameraSpeed * right;
		break;
	case Qt::Key_Left:
		position -= cameraSpeed * right;
		break;
	case Qt::Key_Up:
		position += cameraSpeed * direction;
		break;
	case Qt::Key_Down:
		position -= cameraSpeed * direction;
		break;
	default:
		break;
	}

	reCalcMatrix();
}

void OGLWidget::mousePressEvent(QMouseEvent * event)
{
	mousePressPos = event->globalPos();
}

void OGLWidget::mouseMoveEvent(QMouseEvent * event)
{
	QPoint delta = event->globalPos() - mousePressPos;
	mousePressPos = event->globalPos();

	horizontalAngle += mouseSpeed * delta.x();
	verticalAngle += mouseSpeed * delta.y();

	reCalcMatrix();
}

void OGLWidget::wheelEvent(QWheelEvent * event)
{
	float delta = event->delta() / 90;
	initFov -= delta;
	if (initFov < 5.0f || initFov > 85)
	{
		initFov += delta;
	}

	reCalcMatrix();
}

void OGLWidget::setTexture(QString path)
{
	Texture = TextureLoader::LoadTexture(path.toLocal8Bit());

	update();
}
