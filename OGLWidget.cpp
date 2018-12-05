#include "OGLWidget.h"

OGLWidget::OGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	modelMatrix = glm::mat4(1.0f);

	position = glm::vec3(0, 0, 0);
	horizontalAngle = -PI / 2;
	verticalAngle = 0.0f;
	initFov = 45.0f;

	cameraSpeed = 0.1f;
	mouseSpeed = 0.002f;

	resize(800, 450);
}

OGLWidget::~OGLWidget()
{
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void OGLWidget::initPlane(int width, int height)
{
	std::vector<glm::vec3> Vertex;
	std::vector<glm::vec2> UV;

		float x = -(float)width / 2;
		float y = -(float)height / 2;
		float z = -(float)height / 2 / tan(PI / 8);

		float u = 0;
		float v = 0;
		Vertex.push_back(glm::vec3(z, y, x));
		UV.push_back(glm::vec2(1-u, 1-v));

		y = -y;
		v = 1;
		Vertex.push_back(glm::vec3(z, y, x));
		UV.push_back(glm::vec2(1-u, 1-v));

		x = -x;
		y = -y;
		u = 1;
		v = 0;
		Vertex.push_back(glm::vec3(z, y, x));
		UV.push_back(glm::vec2(1-u, 1-v));

		y = -y;
		v = 1;
		Vertex.push_back(glm::vec3(z, y, x));
		UV.push_back(glm::vec2(1-u, 1-v));

	for (int i = 0; i < 2; i++)
	{
		Indices.push_back(2 * i);
		Indices.push_back(2 * i + 2);
		Indices.push_back(2 * i + 1);
		Indices.push_back(2 * i + 1);
		Indices.push_back(2 * i + 2);
		Indices.push_back(2 * i + 3);
	}

	glGenBuffers(1, &IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &PosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*Vertex.size(), &Vertex[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*UV.size(), &UV[0], GL_STATIC_DRAW);
}

void OGLWidget::initCircle(float r, int Horizontal, int Vertical)
{
	std::vector<ElementCoord> Vertex;
	std::vector<ElementCoord> DrawData;


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

			Vertex.push_back(ElementCoord(glm::vec3(x, y, z), glm::vec2(u, v)));
		}
	}

	for (int j = 0; j < Vertical; j++)
	{
		for (int i = 0; i < Horizontal; i++)
		{
			int num = j * (Horizontal + 1) + i;
			int numUp = num + (Horizontal + 1);

			DrawData.push_back(Vertex[num]);
			DrawData.push_back(Vertex[numUp]);
			DrawData.push_back(Vertex[num + 1]);

			DrawData.push_back(Vertex[numUp]);
			DrawData.push_back(Vertex[numUp + 1]);
			DrawData.push_back(Vertex[num + 1]);
		}
	}


	for (auto i : DrawData)
	{
		Pos.push_back(i.Position);
		UV.push_back(i.TexCoord);
	}

	glGenBuffers(1, &PosBuffer_Q);
	glBindBuffer(GL_ARRAY_BUFFER, PosBuffer_Q);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*Pos.size(), &Pos[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UVBuffer_Q);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer_Q);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*UV.size(), &UV[0], GL_STATIC_DRAW);
}

void OGLWidget::reCalcMatrix()
{
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - PI / 2.0f),
		0,
		cos(horizontalAngle - PI / 2.0f)
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

	initPlane();
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

	Texture = TextureLoader::LoadTexture("Resources/1.jpg");
	Texture_Q = TextureLoader::LoadTexture("Resources/7.png");
}

void OGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);

	//mvp
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix *modelMatrix;
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	//texture
	glActiveTexture(GL_TEXTURE0);
	
	// Draw
	if (Cylinder)
	{
		glBindTexture(GL_TEXTURE_2D, Texture);
		if (m_data)
		{
			m_mutex.lock();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
			m_mutex.unlock();
		}

		glUniform1i(TextureID, 0);//set to 0 because the texture is bound to GL_TEXTURE0

		glEnableVertexAttribArray(PostionLocation);
		glBindBuffer(GL_ARRAY_BUFFER, PosBuffer);
		glVertexAttribPointer(PostionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(UVLocation);
		glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
		glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
		glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Q);
		m_mutex.lock();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data);
		m_mutex.unlock();
		glUniform1i(TextureID, 0);//set to 0 because the texture is bound to GL_TEXTURE0

		glEnableVertexAttribArray(PostionLocation);
		glBindBuffer(GL_ARRAY_BUFFER, PosBuffer_Q);
		glVertexAttribPointer(PostionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(UVLocation);
		glBindBuffer(GL_ARRAY_BUFFER, UVBuffer_Q);
		glVertexAttribPointer(UVLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, Pos.size());
	}
	//
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

void OGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
	//ÇÐ»»Ô²ÖùºÍÇòÌå
	if (event->button() == Qt::LeftButton)
	{
		return;
	}

	Cylinder = !Cylinder;

	update();
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
	m_mutex.lock();
	glDeleteTextures(1, &Texture);
	Texture = TextureLoader::LoadTexture(path.toLocal8Bit());
	m_mutex.unlock();
	update();
}

void OGLWidget::setTexture(unsigned char * pixals, unsigned int px, unsigned int py)
{
	if (m_width != px || m_height != py)
	{
		delete[] m_data;
		m_data = nullptr;
	}

	if (!m_data)
	{
		m_data = new unsigned char[px * py * 3];
	}

	m_mutex.lock();
	memcpy(m_data, pixals, px * py * 3);
	m_width = px;
	m_height = py;
	m_mutex.unlock();

	update();
}
