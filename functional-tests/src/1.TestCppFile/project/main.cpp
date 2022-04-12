// Официальный сайт GLFW с документацией и примерами: https://www.glfw.org/docs/latest/index.html.
// Серия на хабре по OpenGL: https://habr.com/en/post/310790/ (ссылки на все части под спойлером «Содержание»).

#include <iostream>     // cout
#include <stdexcept>    // runtime_error
#include <GL/glew.h>    // Библиотека GLEW и OpenGL
#include <GLFW/glfw3.h> // Библиотека GLFW
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

// Convert number to string
template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

// For choising in main menu
int read_int(const char* prompt, int min, int max) {
	while (true) {
		printf("%s", prompt);
		char _str[16];
		fgets(_str, sizeof(_str), stdin);
		if (strchr(_str, '\n') == NULL) {
			bool f = true;
			int c;
			while ((c = fgetc(stdin)) != '\n' && c != EOF) {
				if (!isspace(c)) {
					f = false;
				}
			}
			if (f == false) {
				printf("The entered string is too large\n");
				continue;
			}
		}
		char* endptr = NULL;
		errno = 0;
		int n = strtol(_str, &endptr, 10);
		if (errno == ERANGE) {
			printf("The entered number is too large\n");
			continue;
		}
		if (endptr == _str) {//abc
			printf("Could not find the number\n");
			continue;
		}
		bool correct = true;
		for (char* p = endptr; *p != 0; ++p) {
			if (!isspace(*p)) {
				correct = false;
				break;
			}
		}
		if (correct == false) {
			printf("Simbols are not correct\n");
			continue;
		}
		if (n < min || n > max) {
			printf("The number out the range. Valid range is from %d to %d\n", min, max);
			continue;
		}
		return (int)n;
	}
}

// Вспомогательная функция, приводящая входное значение к указанному диапазону.
// Она реализована в C++17, но в VS 2019 по умолчанию используется только С++14.
double clamp(const double x, const double min, const double max) {
	if (x < min)
		return min;
	if (max < x)
		return max;
	return x;
}

// Функция, которая будет обрабатывать ошибки GLFW.
void ErrorCallback(const int code, const char* const error) {
	throw runtime_error(error);
}

// Функция, которая будет обрабатывать события клавиатуры.
//   window - Окно, в котором произошло событие.
//      key - GLFW-код клавиши (кроссплатформенный), которая вызвала событие.
//            Список кодов клавиш: https://www.glfw.org/docs/latest/group__keys.html.
// scancode - Системный код клавиши (некроссплатформенный), которая вызвала событие.
//   action - Вид события (GLFW_PRESS, GLFW_RELEASE или GLFW_REPEAT).
//     mods - Битовая маска, определяющая нажатые контрольные клавиши (Alt, Ctrl и т. п.).
//            Список кодов контрольных клавиш: https://www.glfw.org/docs/latest/group__mods.html.
void KeyCallback(GLFWwindow* const window, const int key, const int scancode, const int action, const int mods) {
	// При нажатии на Escape устанавливаем у окна флаг, что оно должно быть закрыто.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Глобальные переменные, в которые будут сохраняться последние координаты курсора мыши.
// Для удобства положим, что значения должны лежать в диапазоне [0; 1].
double X, Y;

// Функция, которая будет обрабатывать движение курсора.
// window - Окно, в котором произошло событие.
//      x - Новое значение координаты X курсора мыши.
//      y - Новое значение координаты Y курсора мыши.
void CursorPositionCallback(GLFWwindow* const window, const double x, const double y) {
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	X = clamp(x / width, 0, 1);
	Y = clamp(y / height, 0, 1);
}

// Global parameter in which we save action from mouse (0 - left button released, 1 - left button pressed) 
int onMouse = 0;

// Function which will process events from mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		onMouse = 1;

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		onMouse = 0;
}

int main() {
	// Устанавливаем функцию обработки ошибок GLFW.
	glfwSetErrorCallback(ErrorCallback);

	try {
		// Инициализируем GLFW.
		if (glfwInit() == GLFW_FALSE)
			throw runtime_error("Failed to initialize GLFW!");


		// Устанавливаем параметры, которые будут использоваться при создании окна:
		// 1) версия OpenGL 4.3;
		// 2) использовать современный режим Core Profile без поддержки устаревших функций;
		// 3) окно должно иметь фиксированный размер.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


		// SETTINGS
		string title;
		size_t wHeight = 900,  wWidth = 900;
		int choise = -1;
		// Создаем объект вершинного шейдера
		while (1) {
			std::cout << "1. app-task1-no-animation (colored points);" << std::endl;
			std::cout << "2. app-task2-one-shot-animation (via worlds static);" << std::endl;
			std::cout << "3. app-task3-repeatable-animation (via worlds drawing);" << std::endl;
			std::cout << "4. app-task4-surprise-your-teacher (...);" << std::endl;
			std::cout << "0. app-exit-from-program (exit without bad consequences)" << std::endl;

			// Choose the title
			choise = read_int("Choose: ", 0, 4);
			switch (choise) {
			case 1: {
				title = "Colored points";
				break;
			}
			case 2: {
				title = "Via worlds";
				break;
			}
			case 3: {
				title = "Via infinity worlds";
				break;
			}
			case 4: {
				title = "Calculator";
				break;
			}
			case 0: {
				exit(0);
			}
			}

			// Создаём окно размера 640x480 с заголовком "Hello".
			GLFWwindow* const window = glfwCreateWindow(wWidth, wHeight, title.c_str(), nullptr, nullptr);
			if (window == nullptr)
				throw runtime_error("Failed to create window!");

			// Устанавливаем функции обработки событий клавиатуры и мыши.
			glfwSetKeyCallback(window, KeyCallback);
			glfwSetCursorPosCallback(window, CursorPositionCallback);
			glfwSetMouseButtonCallback(window, mouse_button_callback);

			// Делаем созданное окно текущим.
			// Все дальнейшие вызываемые функции OpenGL будут относиться к данному окну.
			glfwMakeContextCurrent(window);

			// Инициализируем GLEW.
			// Происходит инициализация функций OpenGL.
			{
				auto error = glewInit();
				if (error != GLEW_OK) {
					auto message = reinterpret_cast<const char*>(glewGetErrorString(error));
					throw runtime_error(string("Failed to initialize GLEW: ") + message);
				}
			}

			{
				// Получаем размеры кадра для созданного окна.
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				// Указываем для OpenGL область окна, в которой будет происходить отрисовка.
				glViewport(0, 0, width, height);

			}

			switch (choise) {
			
			case 1: {
				{
					// SPECIAL SETTING
					glEnable(GL_PROGRAM_POINT_SIZE);

					// SETTINGS
					const size_t N = 10000;

					auto indVertexShader = glCreateShader(GL_VERTEX_SHADER);
					auto indFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

					// Check for errors
					{
						if (indVertexShader == 0)
							throw runtime_error("Can't create vertex shader!");

						if (indFragmentShader == 0)
							throw runtime_error("Can't create fragment shader!");
					}

					char* vertexShaderRsc[] = {
						"#version 430 core\n",
						"in uint rand;\n",
						"out vec3 color;\n",

						"uint xxhash(uint x, uint y) {\n",
						"x = x*3266489917 + 374761393;\n",
						"x = (x << 17) | (x >> 15);\n",
						"x += y * 3266489917;\n",
						"x *= 668265263;\n",
						"x ^= x >> 15;\n",
						"x *= 2246822519;\n",
						"x ^= x >> 13;\n",
						"x *= 3266489917;\n",
						"x ^= x >> 16;\n",
						"return x;}\n",
						"uint xorshiftState = xxhash(rand, rand);\n",
						"float xorshift() {\n",
						"uint x = xorshiftState;\n",
						"x ^= x << 13;\n",
						"x ^= x >> 17;\n",
						"x ^= x << 5;\n",
						"xorshiftState = x;\n",
						"uint m = 0x00ffffff;\n",
						"return (x & m) * (1.0f / 0x1000000);\n",
						"}\n",



						"void main() {\n",
						"float x = -1 + xorshift() * 2;\n",
						"float y = -1 + xorshift() * 2;\n",
						"gl_Position = vec4(x,y, 0, 1);\n",
						"gl_PointSize = 3.0;\n",
						"color = vec3(xorshift(), xorshift(), xorshift());\n",
						"}\n" };

					char* fragmentShaderRsc[] = {
						"#version 430 core\n",
						"in vec3 color;\n",
						"out vec4 endColor;\n",
						"void main() {\n",
						"endColor = vec4(color, 1);\n",
						"}\n" };


					glShaderSource(indVertexShader, 31, vertexShaderRsc, nullptr);
					glShaderSource(indFragmentShader, 6, fragmentShaderRsc, nullptr);

					glCompileShader(indVertexShader);
					glCompileShader(indFragmentShader);

					// Check for errors
					{

						GLint vertexCompiled;
						GLint fragmentCompiled;

						glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &vertexCompiled);
						glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &fragmentCompiled);

						if (vertexCompiled == GL_FALSE) {

							GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indVertexShader, 1024, &log_length, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile vertex shader!");
						}

						if (fragmentCompiled == GL_FALSE) {

							//	GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indVertexShader, 1024, nullptr, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile fragment shader!");
						}
					}

					auto shaderProgram = glCreateProgram();

					// Check for errors
					{
						if (shaderProgram == 0)
							throw runtime_error("Couldn't create the shader program");
					}

					glAttachShader(shaderProgram, indVertexShader);
					glAttachShader(shaderProgram, indFragmentShader);

					glLinkProgram(shaderProgram);

					// Check for errors
					{
						GLint programLinked;

						glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programLinked);

						if (programLinked != GL_TRUE)
						{
							GLchar message[1024];
							glGetProgramInfoLog(shaderProgram, 1024, nullptr, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't link shader program!");
						}
					}

					glUseProgram(shaderProgram);

					// Creating VBO
					GLuint vbo;
					glGenBuffers(1, &vbo);

					glBindBuffer(GL_ARRAY_BUFFER, vbo);

					const auto PI = 3.14159f;

					GLint* data = new GLint[N];

					for (auto i = 0; i < N; ++i)
						data[i] = rand();

					// Загружаем данные в буфер для входных данных вершинного шейдера
					glBufferData(GL_ARRAY_BUFFER, sizeof(int)*N, data, GL_STATIC_DRAW);

					// Creating vao
					GLuint vao;
					glGenVertexArrays(1, &vao);

					glBindVertexArray(vao);

					const auto randPos = glGetAttribLocation(shaderProgram, "rand");

					glVertexAttribPointer(randPos, 1, GL_INT, GL_FALSE, sizeof(GLuint), nullptr);

					glEnableVertexAttribArray(randPos);

					std::cout << "OK!" << endl;
					glClearColor(0, 0, 0, 1);

					while (!glfwWindowShouldClose(window)) {
						glClear(GL_COLOR_BUFFER_BIT);
			
						glDrawArrays(GL_POINTS, 0, N);

						glfwSwapBuffers(window);
						glfwPollEvents();
					}
				}
				break;
			}
			case 2: {

				// SPECIAL SETTING
				glEnable(GL_PROGRAM_POINT_SIZE);

				{

					/* ==========================================
									INITIALIZATION
					===========================================*/


					// Model of a single point:
					// float t0 - start time
					// vec2 p0-initial coordinates
					// vec2 v-speed vector

					const size_t N = 500;
					const GLfloat MIN_SPEED = 0.2f;
					const GLfloat MAX_SPEED = 0.5f;
					const GLfloat POINT_SIZE = 3.0f;
					const GLfloat BACKGROUND_COLOR[] = { 0.5f, 0, 0.5f };
					const GLfloat POINT_COLOR[] = { 1,0.5f,1 };
					const auto STRIDE = 6 * sizeof(GLfloat);

					/* ==========================================
								RESOURCES FOR SHADERS
					===========================================*/

					char* initComputeShaderRsc[] = {
						"#version 430 core\n",
						"layout(local_size_x = 1) in;\n",
						"uniform float minSpeed;\n",
						"uniform float maxSpeed;\n",
						"uniform float t;\n",
						"struct Point{\n",
						"float t;\n",
						"vec2 p;\n",
						"vec2 v;\n",
						"};\n",
						"layout(std430, binding = 0) buffer ssbo {\n",
						"Point points[];\n",
						"};\n",
						"uint xxhash(uint x, uint y) {\n",
						"x = x*3266489917 + 374761393;\n",
						"x = (x << 17) | (x >> 15);\n",
						"x += y * 3266489917;\n",
						"x *= 668265263;\n",
						"x ^= x >> 15;\n",
						"x *= 2246822519;\n",
						"x ^= x >> 13;\n",
						"x *= 3266489917;\n",
						"x ^= x >> 16;\n",
						"return x;}\n",
						"uint xorshiftState = xxhash(floatBitsToUint(t), gl_GlobalInvocationID.x);\n",
						"float xorshift() {\n",
						"uint x = xorshiftState;\n",
						"x ^= x << 13;\n",
						"x ^= x >> 17;\n",
						"x ^= x << 5;\n",
						"xorshiftState = x;\n",
						"uint m = 0x00ffffff;\n",
						"return (x & m) * (1.0f / 0x1000000);\n",
						"}\n",
						"const float PI = 3.14159;\n",
						"void main() {\n",
						"Point point = points[gl_GlobalInvocationID.x];\n",
						"float speed = minSpeed + xorshift() * (maxSpeed - minSpeed);\n",
						"float angle = 2 * PI * xorshift();\n",
						"point.t = t;\n",
						"point.p.x = 0;\n",
						"point.p.y = 0;\n",
						"point.v.x = speed * cos(angle);\n",
						"point.v.y = speed * sin(angle);\n",
						"points[gl_GlobalInvocationID.x] = point;\n",
						"}\n"
					};

					char* vertexShaderRsc[] = {
						"#version 430 core\n",
						"uniform float pointSize;\n",
						"uniform float t;\n",
						"in float t0;\n",
						"in vec2 p0;\n",
						"in vec2 v;\n",
						"void main(){\n",
						"vec2 position = p0 + v * (t-t0);\n",
						"gl_Position = vec4(position,0,1);\n",
						"gl_PointSize = pointSize;\n",
						"}\n",
					};

					char* fragmentShaderRsc[] = {
						"#version 430 core\n",
						"uniform vec3 color;\n",
						"out vec4 outColor;\n",
						"void main() {\n",
						"outColor = vec4(color, 1);\n",
						"}\n" };

					/* ==========================================
								CREATING SHADER PROGRAMS
					===========================================*/

					auto indinitShader = glCreateShader(GL_COMPUTE_SHADER);

					auto indVertexShader = glCreateShader(GL_VERTEX_SHADER);

					auto indFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

					// Check for errors
					{
						if (indinitShader == 0)
							throw runtime_error("Can't create initialization compute shader!");

						if (indVertexShader == 0)
							throw runtime_error("Can't create vertex shader!");

						if (indFragmentShader == 0)
							throw runtime_error("Can't create fragment shader!");
					}

					std::cout << "Compute, vertex and fragment shaders was created!" << endl;

					/* ==========================================
							ПРИВЯЗКА РЕСУРСОВ К ШЕЙДЕРАМ
					===========================================*/

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indinitShader, 46, initComputeShaderRsc, nullptr);

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indVertexShader, 11, vertexShaderRsc, nullptr);

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indFragmentShader, 6, fragmentShaderRsc, nullptr);

					/* ==========================================
								КОМПИЛЯЦИЯ ШЕЙДЕРОВ
					===========================================*/

					glCompileShader(indinitShader);

					glCompileShader(indVertexShader);

					glCompileShader(indFragmentShader);

					// Тест на ошибки
					{
						try {
							GLint shaderCompiled;
							glGetShaderiv(indinitShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;

							glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;

							glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;
						}
						catch (int) {
							GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indinitShader, 1024, &log_length, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile shader!");
						}
					}

					// удаление шейдеров??

					/* ==========================================
								СОЗДАНИЕ ШЕЙДЕРНЫХ ПРОГРАММ
					===========================================*/

					auto initProgram = glCreateProgram();
					auto renderProgram = glCreateProgram();

					// Тест на ошибки
					{
						if (initProgram == 0)
							throw runtime_error("Couldn't create initialization shader program");
						if (renderProgram == 0)
							throw runtime_error("Couldn't create rendering shader program");

					}

					/* ==========================================
							ПРИВЯЗКА ШЕЙДЕРОВ К ПРОГРАММАМ
					===========================================*/

					glAttachShader(initProgram, indinitShader);

					glAttachShader(renderProgram, indVertexShader);
					glAttachShader(renderProgram, indFragmentShader);

					/* ==========================================
								ЛИНКОВКА ПРОГРАММ
					===========================================*/

					glLinkProgram(initProgram);
					glLinkProgram(renderProgram);

					// Тест на ошибки
					{
						try {
							GLint programLinked;
							glGetProgramiv(renderProgram, GL_LINK_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;

							glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;

							glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;
						}
						catch (int) {
							GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indinitShader, 1024, &log_length, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't link shader!");
						}
					}

					/* ==========================================
					Получение идентификаторов uniform-переменных
					===========================================*/

					const auto initMinSpeedLocation = glGetUniformLocation(initProgram, "minSpeed");
					const auto initMaxSpeedLocation = glGetUniformLocation(initProgram, "maxSpeed");
					const auto initTimeLocation = glGetUniformLocation(initProgram, "t");
					const auto renderPointSizeLocation = glGetUniformLocation(renderProgram, "pointSize");
					const auto renderTLocation = glGetUniformLocation(renderProgram, "t");
					const auto renderColorLocation = glGetUniformLocation(renderProgram, "color");
					// ПЕРЕДАЧА ПАРАМЕТРОВ В ШЕЙДЕРЫ

					/* ==========================================
							Создание и инициализация VBO
					===========================================*/

					// Создаем объект VBO
					GLuint vbo;
					{
						glGenBuffers(1, &vbo);
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo); // Привязка VBO к вычислительному шейдеру
						glBindBuffer(GL_ARRAY_BUFFER, vbo);					// Привязка VBO к вершинному шейдеру
						vector<GLbyte> data(N * STRIDE);
						glBufferData(GL_ARRAY_BUFFER, N * STRIDE, data.data(), GL_DYNAMIC_COPY);
					}

					/* ==========================================
							Запуск вычислительного шейдера
					===========================================*/

					glUseProgram(initProgram);
					glUniform1f(initMinSpeedLocation, MIN_SPEED);
					glUniform1f(initMaxSpeedLocation, MAX_SPEED);
					glUniform1f(initTimeLocation, static_cast<GLfloat>(glfwGetTime()));
					glDispatchCompute(N, 1, 1);
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
					glUseProgram(0);
					glDeleteProgram(initProgram);

					/* ==========================================
							Создание и инициализация VAO
					===========================================*/

					GLuint vao;
					glGenVertexArrays(1, &vao);
					glBindVertexArray(vao);
					{
						const auto locationT0 = glGetAttribLocation(renderProgram, "t0");
						const auto locationP0 = glGetAttribLocation(renderProgram, "p0");
						const auto locationV = glGetAttribLocation(renderProgram, "v");

						glVertexAttribPointer(locationT0, 1, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLvoid*>(0 * sizeof(GLfloat)));
						glVertexAttribPointer(locationP0, 2, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));
						glVertexAttribPointer(locationV, 2, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLvoid*>(4 * sizeof(GLfloat)));

						glEnableVertexAttribArray(locationT0);
						glEnableVertexAttribArray(locationP0);
						glEnableVertexAttribArray(locationV);
					}

					/* ==========================================
							ПРЕДВАРИТЕЛЬНЫЕ ДАННЫЕ
					===========================================*/

					glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1);
					glUseProgram(renderProgram);
					glUniform1f(renderPointSizeLocation, POINT_SIZE);
					glUniform3fv(renderColorLocation, 1, POINT_COLOR);
					glUseProgram(0);

					while (!glfwWindowShouldClose(window)) {
						glClear(GL_COLOR_BUFFER_BIT);

						const auto time = static_cast<GLfloat>(glfwGetTime());

						glDispatchCompute(N, 1, 1);
						glUseProgram(0);

						glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

						glUseProgram(renderProgram);
						glUniform1f(renderTLocation, time);
						glDrawArrays(GL_POINTS, 0, N);
						glUseProgram(0);

						glfwSwapBuffers(window);
						glfwPollEvents();
					}
				}

				break;
			}
			case 3: {

				/* ==========================================
							СОЗДАНИЕ ПРОГРАММЫ
				===========================================*/

				glEnable(GL_PROGRAM_POINT_SIZE);

				// Обработка
				{

					/* ==========================================
									ИНИЦИАЛИЗАЦИЯ
					===========================================*/


					// Модель одной точки:
					// float t0 - время начала движения
					// vec2 p0 - начальные координаты
					// vec2 v - вектор скорости

					const size_t N = 1000;
					const GLfloat MIN_SPEED = 0.5f;
					const GLfloat MAX_SPEED = 0.8f;
					const GLfloat POINT_SIZE = 5.0f;
					const GLfloat BACKGROUND_COLOR[] = { 0, 0, 0 };
					const GLfloat POINT_COLOR[] = { rand(), rand(), rand() };
					const auto STRIDE = 8 * sizeof(GLfloat);

					/* ==========================================
								РЕСУРСЫ ДЛЯ ШЕЙДЕРОВ
					===========================================*/

					// Создадим ресурс инициализирующего вычислительного шейдера
					char* initComputeShaderRsc[] = {
						"#version 430 core\n",
						"layout(local_size_x = 1) in;\n",
						"uniform float minSpeed;\n",
						"uniform float maxSpeed;\n",
						"uniform float t;\n",

						"struct Point{\n",
							"vec3 color;\n",
							"float t;\n",
							"vec2 p;\n",
							"vec2 v;\n",
						"};\n",

						"layout(std430, binding = 0) buffer ssbo {\n",
							"Point points[];\n",
						"};\n",

						"uint xxhash(uint x, uint y) {\n",
						"x = x*3266489917 + 374761393;\n",
						"x = (x << 17) | (x >> 15);\n",
						"x += y * 3266489917;\n",
						"x *= 668265263;\n",
						"x ^= x >> 15;\n",
						"x *= 2246822519;\n",
						"x ^= x >> 13;\n",
						"x *= 3266489917;\n",
						"x ^= x >> 16;\n",
						"return x;}\n",

						"uint xorshiftState = xxhash(floatBitsToUint(t), gl_GlobalInvocationID.x);\n",

						"float xorshift() {\n",
						"uint x = xorshiftState;\n",
						"x ^= x << 13;\n",
						"x ^= x >> 17;\n",
						"x ^= x << 5;\n",
						"xorshiftState = x;\n",
						"uint m = 0x00ffffff;\n",
						"return (x & m) * (1.0f / 0x1000000);\n",
						"}\n",

						"const float PI = 3.14159;\n",
						"void main() {\n",

						"Point point = points[gl_GlobalInvocationID.x];\n",

						"float speed = minSpeed + xorshift() * (maxSpeed - minSpeed);\n",
						"float angle = 2 * PI * xorshift();\n",

						"point.t = t;\n",

						"point.color = vec3(xorshift(), xorshift(), xorshift());\n",

						"point.p.x = -1 + xorshift() * 2;\n",
						"point.p.y = -1 + xorshift() * 2;\n",

						"point.v.x = speed * cos(angle);\n",
						"point.v.y = speed * sin(angle);\n",

						"points[gl_GlobalInvocationID.x] = point;\n",
						"}\n"
					};

					// Создадим ресурс отражающего вычислительного шейдера
					char* reflComputeShaderRsc[] = {
						"#version 430 core\n",
						"layout(local_size_x = 1) in;\n",

						"uniform float t;\n",
						"struct Point{\n",
						"vec3 color;\n",
						"float t0;\n",
						"vec2 p0;\n",
						"vec2 v;\n",
						"};\n",

						"layout(std430, binding = 0) buffer ssbo {\n",
							"Point points[];\n",
						"};\n",

						"uint xxhash(uint x, uint y) {\n",
						"x = x*3266489917 + 374761393;\n",
						"x = (x << 17) | (x >> 15);\n",
						"x += y * 3266489917;\n",
						"x *= 668265263;\n",
						"x ^= x >> 15;\n",
						"x *= 2246822519;\n",
						"x ^= x >> 13;\n",
						"x *= 3266489917;\n",
						"x ^= x >> 16;\n",
						"return x;}\n",

						"uint xorshiftState = xxhash(floatBitsToUint(t), gl_GlobalInvocationID.x);\n",

						"float xorshift() {\n",
						"uint x = xorshiftState;\n",
						"x ^= x << 13;\n",
						"x ^= x >> 17;\n",
						"x ^= x << 5;\n",
						"xorshiftState = x;\n",
						"uint m = 0x00ffffff;\n",
						"return (x & m) * (1.0f / 0x1000000);\n",
						"}\n",

						"void main() {\n",
							"Point point = points[gl_GlobalInvocationID.x];\n",
							"vec2 p = point.p0 + point.v * (t-point.t0);\n",
							"bool changed = false;\n",

							"if (p.x <= -1 && point.v.x < 0 || p.x >=1 && point.v.x > 0) {\n",
								"point.v.x = -point.v.x;\n",
								"point.color = vec3(xorshift(),xorshift(),xorshift());\n",
								"changed = true;\n",
							"}\n",

							"if (p.y <= -1 && point.v.y < 0 || p.y >= 1 && point.v.y > 0) {\n",
								"point.v.y = -point.v.y;\n",
								"point.color = vec3(xorshift(),xorshift(),xorshift());\n",
								"changed = true;\n",
							"}\n",

							"if (changed){\n",
								"point.t0 = t;\n",
								"point.p0 = vec2(0,0);\n",
								"point.v = vec2(xorshift()*cos(2*3.14*xorshift()), xorshift()*sin(2*3.14*xorshift()));\n",
								"points[gl_GlobalInvocationID.x] = point;\n",
							"}\n",

						"}\n",
					};

					// Создадим ресурс вершинного шейдера
					char* vertexShaderRsc[] = {
						"#version 430 core\n",
						"uniform float pointSize;\n",
						"uniform float t;\n",

						"in vec3 color;\n",
						"in float t0;\n",
						"in vec2 p0;\n",
						"in vec2 v;\n",
						"out vec3 color0;\n",

						"void main(){\n",
							"vec2 position = p0 + v * (t-t0);\n",
							"gl_Position = vec4(position,0,1);\n",
							"gl_PointSize = pointSize;\n",
							"color0 = color;\n",
						"}\n",
					};

					// Создадим ресурс фрагментного шейдера
					char* fragmentShaderRsc[] = {
						"#version 430 core\n",
						"in vec3 color0;\n",
						"out vec4 outColor;\n",
						"void main() {\n",
						"outColor = vec4(color0, 1);\n",
						"}\n" };

					/* ==========================================
								СОЗДАНИЕ ШЕЙДЕРНЫХ ПРОГРАММ
					===========================================*/

					// Создаем объект вычислительного инициализирующего шейдера
					auto indinitShader = glCreateShader(GL_COMPUTE_SHADER);

					// Создаем объект вычислительного отражающего шейдера
					auto indReflectionShader = glCreateShader(GL_COMPUTE_SHADER);

					// Создаем объект вершинного шейдера
					auto indVertexShader = glCreateShader(GL_VERTEX_SHADER);

					// Создаем объект фрагментного шейдера
					auto indFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

					// Тест на ошибки
					{
						if (indinitShader == 0)
							throw runtime_error("Can't create initialization compute shader!");

						if (indReflectionShader == 0)
							throw runtime_error("Can't create reflection compute shader!");

						if (indVertexShader == 0)
							throw runtime_error("Can't create vertex shader!");

						if (indFragmentShader == 0)
							throw runtime_error("Can't create fragment shader!");
					}

					std::cout << "Compute, vertex and fragment shaders was created!" << endl;

					/* ==========================================
							ПРИВЯЗКА РЕСУРСОВ К ШЕЙДЕРАМ
					===========================================*/

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indinitShader, 48, initComputeShaderRsc, nullptr);

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indReflectionShader, 54, reflComputeShaderRsc, nullptr);

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indVertexShader, 14, vertexShaderRsc, nullptr);

					// Отдаем наш код вычислительному шейдеру
					glShaderSource(indFragmentShader, 6, fragmentShaderRsc, nullptr);

					/* ==========================================
								КОМПИЛЯЦИЯ ШЕЙДЕРОВ
					===========================================*/

					glCompileShader(indinitShader);

					glCompileShader(indReflectionShader);

					glCompileShader(indVertexShader);

					glCompileShader(indFragmentShader);

					// Тест на ошибки
					{
						try {
							GLint shaderCompiled;
							glGetShaderiv(indinitShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;

							glGetShaderiv(indReflectionShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;

							glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;

							glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &shaderCompiled);
							if (shaderCompiled == GL_FALSE)
								throw 0;
						}
						catch (int) {
							GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indinitShader, 1024, &log_length, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile shader!");
						}
					}

					// удаление шейдеров??

					/* ==========================================
								СОЗДАНИЕ ШЕЙДЕРНЫХ ПРОГРАММ
					===========================================*/

					auto initProgram = glCreateProgram();
					auto reflectProgram = glCreateProgram();
					auto renderProgram = glCreateProgram();

					// Тест на ошибки
					{
						if (initProgram == 0)
							throw runtime_error("Couldn't create initialization shader program");
						if (reflectProgram == 0)
							throw runtime_error("Couldn't create reflection shader program");
						if (renderProgram == 0)
							throw runtime_error("Couldn't create rendering shader program");

					}

					/* ==========================================
							ПРИВЯЗКА ШЕЙДЕРОВ К ПРОГРАММАМ
					===========================================*/

					glAttachShader(initProgram, indinitShader);

					glAttachShader(reflectProgram, indReflectionShader);

					glAttachShader(renderProgram, indVertexShader);
					glAttachShader(renderProgram, indFragmentShader);

					/* ==========================================
								ЛИНКОВКА ПРОГРАММ
					===========================================*/

					glLinkProgram(initProgram);
					glLinkProgram(reflectProgram);
					glLinkProgram(renderProgram);

					// Тест на ошибки
					{
						try {
							GLint programLinked;
							glGetProgramiv(renderProgram, GL_LINK_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;

							glGetShaderiv(indReflectionShader, GL_COMPILE_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;

							glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;

							glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &programLinked);
							if (programLinked == GL_FALSE)
								throw 0;
						}
						catch (int) {
							GLsizei log_length = 0;
							GLchar message[1024];
							glGetShaderInfoLog(indinitShader, 1024, &log_length, message);
							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't link shader!");
						}
					}

					/* ==========================================
					Получение идентификаторов uniform-переменных
					===========================================*/

					const auto initMinSpeedLocation = glGetUniformLocation(initProgram, "minSpeed");
					const auto initMaxSpeedLocation = glGetUniformLocation(initProgram, "maxSpeed");
					const auto initTimeLocation = glGetUniformLocation(initProgram, "t");
					const auto reflectTimeLocation = glGetUniformLocation(reflectProgram, "t");
					const auto renderPointSizeLocation = glGetUniformLocation(renderProgram, "pointSize");
					const auto renderTLocation = glGetUniformLocation(renderProgram, "t");
					//const auto renderColorLocation = glGetUniformLocation(renderProgram, "color");
					// ПЕРЕДАЧА ПАРАМЕТРОВ В ШЕЙДЕРЫ

					/* ==========================================
							Создание и инициализация VBO
					===========================================*/

					// Создаем объект VBO
					GLuint vbo;
					{
						glGenBuffers(1, &vbo);
						glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo); // Привязка VBO к вычислительному шейдеру
						glBindBuffer(GL_ARRAY_BUFFER, vbo);					// Привязка VBO к вершинному шейдеру
						vector<GLbyte> data(N * STRIDE);
						glBufferData(GL_ARRAY_BUFFER, N * STRIDE, data.data(), GL_DYNAMIC_COPY);
					}

					/* ==========================================
							Запуск вычислительного шейдера
					===========================================*/

					glUseProgram(initProgram);
					glUniform1f(initMinSpeedLocation, MIN_SPEED);
					glUniform1f(initMaxSpeedLocation, MAX_SPEED);
					glUniform1f(initTimeLocation, static_cast<GLfloat>(glfwGetTime()));
					glDispatchCompute(N, 1, 1);
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
					glUseProgram(0);
					glDeleteProgram(initProgram);

					/* ==========================================
							Создание и инициализация VAO
					===========================================*/

					GLuint vao;
					glGenVertexArrays(1, &vao);
					glBindVertexArray(vao);
					{
						const auto locationColor0 = glGetAttribLocation(renderProgram, "color");
						const auto locationT0 = glGetAttribLocation(renderProgram, "t0");
						const auto locationP0 = glGetAttribLocation(renderProgram, "p0");
						const auto locationV = glGetAttribLocation(renderProgram, "v");

						glVertexAttribPointer(locationColor0, 3, GL_FLOAT, GL_FALSE, STRIDE, nullptr);
						glVertexAttribPointer(locationT0, 1, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLfloat*>(3 * sizeof(GLfloat)));
						glVertexAttribPointer(locationP0, 2, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLfloat*>(4 * sizeof(GLfloat)));
						glVertexAttribPointer(locationV, 2, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<GLfloat*>(6 * sizeof(GLfloat)));

						glEnableVertexAttribArray(locationColor0);
						glEnableVertexAttribArray(locationT0);
						glEnableVertexAttribArray(locationP0);
						glEnableVertexAttribArray(locationV);
					}

					/* ==========================================
							ПРЕДВАРИТЕЛЬНЫЕ ДАННЫЕ
					===========================================*/

					glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], 1);
					glUseProgram(renderProgram);
					glUniform1f(renderPointSizeLocation, POINT_SIZE);
					//glUniform3fv(renderColorLocation, 1, POINT_COLOR);
					glUseProgram(0);

					while (!glfwWindowShouldClose(window)) {
						glClear(GL_COLOR_BUFFER_BIT);

						const auto time = static_cast<GLfloat>(glfwGetTime());

						glUseProgram(reflectProgram);
						glUniform1f(reflectTimeLocation, time);
						glDispatchCompute(N, 1, 1);
						glUseProgram(0);

						glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

						glUseProgram(renderProgram);
						glUniform1f(renderTLocation, time);
						glDrawArrays(GL_POINTS, 0, N);
						glUseProgram(0);
						glfwSwapBuffers(window);
						glfwPollEvents();
					}
				}

				break;
			}
			case 4: {

				{
					auto indVertexShader = glCreateShader(GL_VERTEX_SHADER);
					auto indFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

					// Тест на ошибки
					{
						if (indVertexShader == 0)
							throw runtime_error("Can't create vertex shader!");
						if (indFragmentShader == 0)
							throw runtime_error("Can't create fragment shader!");
					}

					// Resource of vertex shader
					char* vertexShaderRsc[] = {
						"#version 430 core\n",

						"uniform float stride;\n",

						"in vec2 xy;\n",
						"in vec3 rgb;\n",
						"out vec3 color;\n",

						"void main() {\n",
							"vec2 newXY = vec2(xy.x - 0.15*stride, xy.y*0.8-0.2);\n",
							"gl_Position = vec4(newXY, 0, 1);\n",
							"color = rgb;\n",
						"}\n" };

					// Resource of fragment shader
					char* fragmentShaderRsc[] = {
						"#version 430 core\n",

						"in vec3 color;\n",
						"out vec4 endColor;\n",

						"void main() {\n",
							"endColor = vec4(color, 1);\n",
						"}\n" };

					glShaderSource(indVertexShader, 10, vertexShaderRsc, nullptr);
					glShaderSource(indFragmentShader, 6, fragmentShaderRsc, nullptr);

					glCompileShader(indVertexShader);
					glCompileShader(indFragmentShader);

					// Тест на ошибки
					{
						GLint vertexCompiled;
						glGetShaderiv(indVertexShader, GL_COMPILE_STATUS, &vertexCompiled);

						if (vertexCompiled == GL_FALSE) {

							GLchar message[1024];

							glGetShaderInfoLog(indVertexShader, 1024, nullptr, message);

							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile vertex shader!");
						}

						GLint fragmentCompiled;
						glGetShaderiv(indFragmentShader, GL_COMPILE_STATUS, &fragmentCompiled);

						if (fragmentCompiled == GL_FALSE) {

							GLchar message[1024];

							glGetShaderInfoLog(indVertexShader, 1024, nullptr, message);

							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't compile vertex shader!");
						}
					}

					auto shaderProgram = glCreateProgram();

					// Test for errors
					{
						if (shaderProgram == 0)
							throw runtime_error("Couldn't create the shader program");

					}

					glAttachShader(shaderProgram, indVertexShader);
					glAttachShader(shaderProgram, indFragmentShader);

					glLinkProgram(shaderProgram);

					// Test for errors
					{
						GLint programLinked;
						glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programLinked);
						if (programLinked != GL_TRUE)
						{
							GLchar message[1024];

							glGetProgramInfoLog(shaderProgram, 1024, nullptr, message);

							std::cout << "ERROR: " << message << endl;
							throw runtime_error("Can't link shader program!");
						}
					}

					
					glUseProgram(shaderProgram);

					GLuint vbo[3];
					glGenBuffers(3, vbo);

					GLuint vao[3];

					glGenVertexArrays(3, vao);

					// BUTTONS WITHOUT NUMS		(COLOR = 0.27, 0.51, 0.71)
					// OPERATION BUTTONS		(COLOR = 1, 0.27, 0.28)
					// BUTTON "AC"				(COLOR = 0.13, 0.55, 0.13)
					{
	
						glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

						double stride = 0.5;
						double insideStride = 0.02;

						// Do not open!!!
						GLfloat data[] = {

							// result

							// 7
							-0.9, 0.9 + 0.5, 0.0, 0.0, 0.0,
							0.9, 0.9 + 0.5, 0.0, 0.0, 0.0,
							0.9, 0.6 + 0.5, 0.0, 0.0, 0.0,

							-0.9, 0.9 + 0.5, 0.0, 0.0, 0.0,
							-0.9, 0.6 + 0.5, 0.0, 0.0, 0.0,
							0.9, 0.6 + 0.5, 0.0, 0.0, 0.0,

							-0.9 + insideStride, 0.9 + 0.5 - insideStride, 0.27, 0.51, 0.71,
							0.9 - insideStride, 0.9 + 0.5 - insideStride, 0.27, 0.51, 0.71,
							0.9 - insideStride, 0.6 + 0.5 + insideStride, 0.27, 0.51, 0.71,

							-0.9 + insideStride, 0.9 + 0.5 - insideStride, 0.27, 0.51, 0.71,
							-0.9 + insideStride, 0.6 + 0.5 + insideStride, 0.27, 0.51, 0.71,
							0.9 - insideStride, 0.6 + 0.5 + insideStride, 0.27, 0.51, 0.71,

							// +

							-0.9, -0.1, 0.0, 0.0, 0.0,
							-0.6, -0.1, 0.0, 0.0, 0.0,
							-0.6, -0.4, 0.0, 0.0, 0.0,

							-0.9, -0.1, 0.0, 0.0, 0.0,
							-0.9, -0.4, 0.0, 0.0, 0.0,
							-0.6, -0.4, 0.0, 0.0, 0.0,

							-0.9 + insideStride, -0.1 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, -0.1 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, -0.4 + insideStride, 1.0, 0.27, 0.28,

							-0.9 + insideStride, -0.1 - insideStride, 1.0, 0.27, 0.28,
							-0.9 + insideStride, -0.4 + insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, -0.4 + insideStride, 1.0, 0.27, 0.28,

							// -

							-0.9, 0.4, 0.0, 0.0, 0.0,
							-0.6, 0.4, 0.0, 0.0, 0.0,
							-0.6, 0.1, 0.0, 0.0, 0.0,

							-0.9, 0.4, 0.0, 0.0, 0.0,
							-0.9, 0.1, 0.0, 0.0, 0.0,
							-0.6, 0.1, 0.0, 0.0, 0.0,

							-0.9 + insideStride, 0.4 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.4 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.1 + insideStride, 1.0, 0.27, 0.28,

							-0.9 + insideStride, 0.4 - insideStride, 1.0, 0.27, 0.28,
							-0.9 + insideStride, 0.1 + insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.1 + insideStride, 1.0, 0.27, 0.28,

							// =

							-0.9, 0.9, 0.0, 0.0, 0.0,
							-0.6, 0.9, 0.0, 0.0, 0.0,
							-0.6, 0.6, 0.0, 0.0, 0.0,

							-0.9, 0.9, 0.0, 0.0, 0.0,
							-0.9, 0.6, 0.0, 0.0, 0.0,
							-0.6, 0.6, 0.0, 0.0, 0.0,

							-0.9 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							-0.9 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.9 + insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,
							-0.6 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							// *

							-0.9 + 0.5, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 0.5, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 0.5, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 0.5, 0.9, 0.0, 0.0, 0.0,
							-0.9 + 0.5, 0.6, 0.0, 0.0, 0.0,
							-0.6 + 0.5, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 0.5 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 + 0.5 - insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 + 0.5 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							-0.9 + 0.5 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.9 + 0.5 + insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,
							-0.6 + 0.5 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							// \

							-0.9 + 1.0, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 1.0, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 1.0, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 1.0, 0.9, 0.0, 0.0, 0.0,
							-0.9 + 1.0, 0.6, 0.0, 0.0, 0.0,
							-0.6 + 1.0, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 1.0 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 + 1.0 - insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.6 + 1.0 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							-0.9 + 1.0 + insideStride, 0.9 - insideStride, 1.0, 0.27, 0.28,
							-0.9 + 1.0 + insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,
							-0.6 + 1.0 - insideStride, 0.6 + insideStride, 1.0, 0.27, 0.28,

							// AC

							-0.9 + 1.5, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 1.5, 0.9, 0.0, 0.0, 0.0,
							-0.6 + 1.5, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 1.5, 0.9, 0.0, 0.0, 0.0,
							-0.9 + 1.5, 0.6, 0.0, 0.0, 0.0,
							-0.6 + 1.5, 0.6, 0.0, 0.0, 0.0,

							-0.9 + 1.5 + insideStride, 0.9 - insideStride, 0.13, 0.55, 0.13,
							-0.6 + 1.5 - insideStride, 0.9 - insideStride, 0.13, 0.55, 0.13,
							-0.6 + 1.5 - insideStride, 0.6 + insideStride, 0.13, 0.55, 0.13,

							-0.9 + 1.5 + insideStride, 0.9 - insideStride, 0.13, 0.55, 0.13,
							-0.9 + 1.5 + insideStride, 0.6 + insideStride, 0.13, 0.55, 0.13,
							-0.6 + 1.5 - insideStride, 0.6 + insideStride, 0.13, 0.55, 0.13,

							// 0

							-0.9, -0.6, 0.0, 0.0, 0.0,
							-0.6, -0.6, 0.0, 0.0, 0.0,
							-0.6, -0.9, 0.0, 0.0, 0.0,

							-0.9, -0.6, 0.0, 0.0, 0.0,
							-0.9, -0.9, 0.0, 0.0, 0.0,
							-0.6, -0.9, 0.0, 0.0, 0.0,

							-0.9 + insideStride, -0.6 - insideStride, 0.27, 0.51, 0.71,
							-0.6 - insideStride, -0.6 - insideStride, 0.27, 0.51, 0.71,
							-0.6 - insideStride, -0.9 + insideStride, 0.27, 0.51, 0.71,

							-0.9 + insideStride, -0.6 - insideStride, 0.27, 0.51, 0.71,
							-0.9 + insideStride, -0.9 + insideStride, 0.27, 0.51, 0.71,
							-0.6 - insideStride, -0.9 + insideStride, 0.27, 0.51, 0.71,


							// 7
							-0.4, 0.4, 0.0, 0.0, 0.0,
							-0.1, 0.4, 0.0, 0.0, 0.0,
							-0.1, 0.1, 0.0, 0.0, 0.0,

							-0.4, 0.4, 0.0, 0.0, 0.0,
							-0.4, 0.1, 0.0, 0.0, 0.0,
							-0.1, 0.1, 0.0, 0.0, 0.0,

							-0.4 + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							-0.4 + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.4 + insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							// 8

							-0.4 + stride, 0.4, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.4, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1, 0.0, 0.0, 0.0,

							-0.4 + stride, 0.4, 0.0, 0.0, 0.0,
							-0.4 + stride, 0.1, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1, 0.0, 0.0, 0.0,

							-0.4 + stride + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							-0.4 + stride + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.4 + stride + insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							// 9

							-0.4 + 2 * stride, 0.4, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.4, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.1, 0.0, 0.0, 0.0,

							-0.4 + 2 * stride, 0.4, 0.0, 0.0, 0.0,
							-0.4 + 2 * stride, 0.1, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.1, 0.0, 0.0, 0.0,

							-0.4 + 2 * stride + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							-0.4 + 2 * stride + insideStride, 0.4 - insideStride, 0.27, 0.51, 0.71,
							-0.4 + 2 * stride + insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.1 + insideStride, 0.27, 0.51, 0.71,

							// 4
							-0.4, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.4, 0.1 - stride, 0.0, 0.0, 0.0,
							-0.1, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4 + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							-0.4 + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.4 + insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							// 5

							-0.4 + stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4 + stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.4 + stride, 0.1 - stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4 + stride + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							-0.4 + stride + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.4 + stride + insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,
							-0.1 + stride - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							// 6

							-0.4 + 2 * stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4 + 2 * stride, 0.4 - stride, 0.0, 0.0, 0.0,
							-0.4 + 2 * stride, 0.1 - stride, 0.0, 0.0, 0.0,
							-0.1 + 2 * stride, 0.1 - stride, 0.0, 0.0, 0.0,

							-0.4 + 2 * stride + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							-0.4 + 2 * stride + insideStride, 0.4 - stride - insideStride, 0.27, 0.51, 0.71,
							-0.4 + 2 * stride + insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,
							-0.1 + 2 * stride - insideStride, 0.1 - stride + insideStride, 0.27, 0.51, 0.71,

							// 1
							-0.4, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

							-0.4, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.4, 0.1 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

							-0.4 + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,

							-0.4 + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
							-0.4 + insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,
							-0.1 - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,

							// 2

							-0.4 + stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

							-0.4 + stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
							-0.4 + stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,
							-0.1 + stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

								-0.4 + stride + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
								-0.1 + stride - insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
								-0.1 + stride - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,

								-0.4 + stride + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
								-0.4 + stride + insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,
								-0.1 + stride - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,

								// 3

								-0.4 + 2 * stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
								-0.1 + 2 * stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
								-0.1 + 2 * stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

								-0.4 + 2 * stride, 0.4 - 2 * stride, 0.0, 0.0, 0.0,
								-0.4 + 2 * stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,
								-0.1 + 2 * stride, 0.1 - 2 * stride, 0.0, 0.0, 0.0,

									-0.4 + 2 * stride + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
									-0.1 + 2 * stride - insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
									-0.1 + 2 * stride - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,

									-0.4 + 2 * stride + insideStride, 0.4 - 2 * stride - insideStride, 0.27, 0.51, 0.71,
									-0.4 + 2 * stride + insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,
									-0.1 + 2 * stride - insideStride, 0.1 - 2 * stride + insideStride, 0.27, 0.51, 0.71,
						};

						glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

						glBindVertexArray(vao[0]);

						auto xyPos = glGetAttribLocation(shaderProgram, "xy");
						auto rgbPos = glGetAttribLocation(shaderProgram, "rgb");

						glVertexAttribPointer(xyPos, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
						glVertexAttribPointer(rgbPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));

						glEnableVertexAttribArray(xyPos);
						glEnableVertexAttribArray(rgbPos);

						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);

						{
							if (xyPos == -1)
								throw runtime_error("Could not find \"xy\"");
							if (rgbPos == -1)
								throw runtime_error("Could not find \"rgb\"");
						}
					}

					// NUMERIC
					{
						glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

						
						double stride = 0.5;
						double insideStride = 0.02;

						// SEALED!!
						GLfloat data[] = {

							// =
							-0.85, 0.70, 1, 1, 1,
							-0.65, 0.70, 1, 1, 1,
							-0.65, 0.68, 1, 1, 1,

							-0.85, 0.70, 1, 1, 1,
							-0.85, 0.68, 1, 1, 1,
							-0.65, 0.68, 1, 1, 1,

							-0.85, 0.82, 1, 1, 1,
							-0.65, 0.82, 1, 1, 1,
							-0.65, 0.80, 1, 1, 1,

							-0.85, 0.82, 1, 1, 1,
							-0.85, 0.8, 1, 1, 1,
							-0.65, 0.8, 1, 1, 1,

							// *
							-0.26, 0.26 + 0.5, 1, 1, 1,
							-0.24, 0.26 + 0.5, 1, 1, 1,
							-0.24, 0.24 + 0.5, 1, 1, 1,

							-0.26, 0.26 + 0.5, 1, 1, 1,
							-0.26, 0.24 + 0.5, 1, 1, 1,
							-0.24, 0.24 + 0.5, 1, 1, 1,


							// /
							-0.26 + 0.5, 0.26 + 0.5 + 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.26 + 0.5 + 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.24 + 0.5 + 0.04, 1, 1, 1,

							-0.26 + 0.5, 0.26 + 0.5 + 0.04, 1, 1, 1,
							-0.26 + 0.5, 0.24 + 0.5 + 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.24 + 0.5 + 0.04, 1, 1, 1,


							-0.2 + 0.5, 0.26 + 0.5, 1, 1, 1,
							-0.3 + 0.5, 0.26 + 0.5, 1, 1, 1,
							-0.3 + 0.5, 0.24 + 0.5, 1, 1, 1,

							-0.2 + 0.5, 0.26 + 0.5, 1, 1, 1,
							-0.2 + 0.5, 0.24 + 0.5, 1, 1, 1,
							-0.3 + 0.5, 0.24 + 0.5, 1, 1, 1,


							-0.26 + 0.5, 0.26 + 0.5 - 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.26 + 0.5 - 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.24 + 0.5 - 0.04, 1, 1, 1,

							-0.26 + 0.5, 0.26 + 0.5 - 0.04, 1, 1, 1,
							-0.26 + 0.5, 0.24 + 0.5 - 0.04, 1, 1, 1,
							-0.24 + 0.5, 0.24 + 0.5 - 0.04, 1, 1, 1,

							// AC

							// A
							0.7 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.72 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.72 - 0.06, 0.14 + 0.5, 1, 1, 1,

							0.7 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.7 - 0.06, 0.14 + 0.5, 1, 1, 1,
							0.72 - 0.06, 0.14 + 0.5, 1, 1, 1,

							0.78 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.14 + 0.5, 1, 1, 1,

							0.78 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.78 - 0.06, 0.14 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.14 + 0.5, 1, 1, 1,

							// HORIZONTAL

							0.7 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.34 + 0.5, 1, 1, 1,

							0.7 - 0.06, 0.36 + 0.5, 1, 1, 1,
							0.7 - 0.06, 0.34 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.34 + 0.5, 1, 1, 1,

							0.7 - 0.06, 0.26 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.26 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.24 + 0.5, 1, 1, 1,

							0.7 - 0.06, 0.26 + 0.5, 1, 1, 1,
							0.7 - 0.06, 0.24 + 0.5, 1, 1, 1,
							0.8 - 0.06, 0.24 + 0.5, 1, 1, 1,

							// C
							0.7 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.72 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.72 + 0.06, 0.14 + 0.5, 1, 1, 1,

							0.7 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.7 + 0.06, 0.14 + 0.5, 1, 1, 1,
							0.72 + 0.06, 0.14 + 0.5, 1, 1, 1,

							// HORIZONTAL

							0.7 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.34 + 0.5, 1, 1, 1,

							0.7 + 0.06, 0.36 + 0.5, 1, 1, 1,
							0.7 + 0.06, 0.34 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.34 + 0.5, 1, 1, 1,

							0.7 + 0.06, 0.16 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.16 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.14 + 0.5, 1, 1, 1,

							0.7 + 0.06, 0.16 + 0.5, 1, 1, 1,
							0.7 + 0.06, 0.14 + 0.5, 1, 1, 1,
							0.8 + 0.06, 0.14 + 0.5, 1, 1, 1,

							// -
							-0.85, 0.26, 1, 1, 1,
							-0.65, 0.26, 1, 1, 1,
							-0.65, 0.24, 1, 1, 1,

							-0.85, 0.26, 1, 1, 1,
							-0.85, 0.24, 1, 1, 1,
							-0.65, 0.24, 1, 1, 1,

							// +
							-0.76, -0.15, 1, 1, 1,
							-0.74, -0.15, 1, 1, 1,
							-0.74, -0.35, 1, 1, 1,

							-0.76, -0.15, 1, 1, 1,
							-0.76, -0.35, 1, 1, 1,
							-0.74, -0.35, 1, 1, 1,

							-0.85, -0.24, 1, 1, 1,
							-0.65, -0.24, 1, 1, 1,
							-0.65, -0.26, 1, 1, 1,

							-0.85, -0.24, 1, 1, 1,
							-0.85, -0.26, 1, 1, 1,
							-0.65, -0.26, 1, 1, 1,

							// 0
							-0.8, -0.65, 1, 1, 1,
							-0.7, -0.65, 1, 1, 1,
							-0.7, -0.85, 1, 1, 1,

							-0.8, -0.65, 1, 1, 1,
							-0.8, -0.85, 1, 1, 1,
							-0.7, -0.85, 1, 1, 1,

							-0.8 + insideStride, -0.65 - insideStride, 0.27, 0.51, 0.71,
							-0.7 - insideStride, -0.65 - insideStride, 0.27, 0.51, 0.71,
							-0.7 - insideStride, -0.85 + insideStride, 0.27, 0.51, 0.71,

							-0.8 + insideStride, -0.65 - insideStride, 0.27, 0.51, 0.71,
							-0.8 + insideStride, -0.85 + insideStride, 0.27, 0.51, 0.71,
							-0.7 - insideStride, -0.85 + insideStride, 0.27, 0.51, 0.71,

							// 1
							-0.24, -0.65, 1, 1, 1,
							-0.26, -0.65, 1, 1, 1,
							-0.26, -0.85, 1, 1, 1,

							-0.24, -0.65, 1, 1, 1,
							-0.24, -0.85, 1, 1, 1,
							-0.26, -0.85, 1, 1, 1,

							// 2

							// HORIZONTAL
							0.2, -0.64, 1, 1, 1,
							0.3, -0.64, 1, 1, 1,
							0.3, -0.66, 1, 1, 1,

							0.2, -0.64, 1, 1, 1,
							0.2, -0.66, 1, 1, 1,
							0.3, -0.66, 1, 1, 1,

							0.2, -0.74, 1, 1, 1,
							0.3, -0.74, 1, 1, 1,
							0.3, -0.76, 1, 1, 1,

							0.2, -0.74, 1, 1, 1,
							0.2, -0.76, 1, 1, 1,
							0.3, -0.76, 1, 1, 1,

							0.2, -0.84, 1, 1, 1,
							0.3, -0.84, 1, 1, 1,
							0.3, -0.86, 1, 1, 1,

							0.2, -0.84, 1, 1, 1,
							0.2, -0.86, 1, 1, 1,
							0.3, -0.86, 1, 1, 1,

							// VERTICAL
							0.28, -0.64, 1, 1, 1,
							0.3, -0.64, 1, 1, 1,
							0.3, -0.76, 1, 1, 1,

							0.28, -0.64, 1, 1, 1,
							0.28, -0.76, 1, 1, 1,
							0.3, -0.76, 1, 1, 1,

							0.2, -0.74, 1, 1, 1,
							0.22, -0.74, 1, 1, 1,
							0.22, -0.86, 1, 1, 1,


							0.2, -0.74, 1, 1, 1,
							0.2, -0.86, 1, 1, 1,
							0.22, -0.86, 1, 1, 1,

							// 3

							// HORIZONTAL
							0.7, -0.64, 1, 1, 1,
							0.8, -0.64, 1, 1, 1,
							0.8, -0.66, 1, 1, 1,

							0.7, -0.64, 1, 1, 1,
							0.7, -0.66, 1, 1, 1,
							0.8, -0.66, 1, 1, 1,

							0.7, -0.74, 1, 1, 1,
							0.8, -0.74, 1, 1, 1,
							0.8, -0.76, 1, 1, 1,

							0.7, -0.74, 1, 1, 1,
							0.7, -0.76, 1, 1, 1,
							0.8, -0.76, 1, 1, 1,

							0.7, -0.84, 1, 1, 1,
							0.8, -0.84, 1, 1, 1,
							0.8, -0.86, 1, 1, 1,

							0.7, -0.84, 1, 1, 1,
							0.7, -0.86, 1, 1, 1,
							0.8, -0.86, 1, 1, 1,

							// VERTICAL
							0.78, -0.64, 1, 1, 1,
							0.8, -0.64, 1, 1, 1,
							0.8, -0.86, 1, 1, 1,

							0.78, -0.64, 1, 1, 1,
							0.78, -0.86, 1, 1, 1,
							0.8, -0.86, 1, 1, 1,

							// 4
							-0.3, -0.14, 1, 1, 1,
							-0.28, -0.14, 1, 1, 1,
							-0.28, -0.26, 1, 1, 1,

							-0.3, -0.14, 1, 1, 1,
							-0.3, -0.26, 1, 1, 1,
							-0.28, -0.26, 1, 1, 1,

							-0.22, -0.14, 1, 1, 1,
							-0.2, -0.14, 1, 1, 1,
							-0.2, -0.36, 1, 1, 1,

							-0.22, -0.14, 1, 1, 1,
							-0.22, -0.36, 1, 1, 1,
							-0.2, -0.36, 1, 1, 1,

							-0.3, -0.24, 1, 1, 1,
							-0.2, -0.24, 1, 1, 1,
							-0.2, -0.26, 1, 1, 1,

							-0.3, -0.24, 1, 1, 1,
							-0.3, -0.26, 1, 1, 1,
							-0.2, -0.26, 1, 1, 1,

							// 5

							0.2, -0.14, 1, 1, 1,
							0.22, -0.14, 1, 1, 1,
							0.22, -0.26, 1, 1, 1,

							0.2, -0.14, 1, 1, 1,
							0.2, -0.26, 1, 1, 1,
							0.22, -0.26, 1, 1, 1,

							0.28, -0.24, 1, 1, 1,
							0.3, -0.24, 1, 1, 1,
							0.3, -0.36, 1, 1, 1,

							0.28, -0.24, 1, 1, 1,
							0.28, -0.36, 1, 1, 1,
							0.3, -0.36, 1, 1, 1,

							// HORIZONTAL

							0.2, -0.14, 1, 1, 1,
							0.3, -0.14, 1, 1, 1,
							0.3, -0.16, 1, 1, 1,

							0.2, -0.14, 1, 1, 1,
							0.2, -0.16, 1, 1, 1,
							0.3, -0.16, 1, 1, 1,

							0.2, -0.24, 1, 1, 1,
							0.3, -0.24, 1, 1, 1,
							0.3, -0.26, 1, 1, 1,

							0.2, -0.24, 1, 1, 1,
							0.2, -0.26, 1, 1, 1,
							0.3, -0.26, 1, 1, 1,

							0.2, -0.34, 1, 1, 1,
							0.3, -0.34, 1, 1, 1,
							0.3, -0.36, 1, 1, 1,

							0.2, -0.34, 1, 1, 1,
							0.2, -0.36, 1, 1, 1,
							0.3, -0.36, 1, 1, 1,

							// 6

							0.7, -0.14, 1, 1, 1,
							0.72, -0.14, 1, 1, 1,
							0.72, -0.36, 1, 1, 1,

							0.7, -0.14, 1, 1, 1,
							0.7, -0.36, 1, 1, 1,
							0.72, -0.36, 1, 1, 1,

							0.78, -0.24, 1, 1, 1,
							0.8, -0.24, 1, 1, 1,
							0.8, -0.36, 1, 1, 1,

							0.78, -0.24, 1, 1, 1,
							0.78, -0.36, 1, 1, 1,
							0.8, -0.36, 1, 1, 1,

							// HORIZONTAL

							0.7, -0.14, 1, 1, 1,
							0.8, -0.14, 1, 1, 1,
							0.8, -0.16, 1, 1, 1,

							0.7, -0.14, 1, 1, 1,
							0.7, -0.16, 1, 1, 1,
							0.8, -0.16, 1, 1, 1,

							0.7, -0.24, 1, 1, 1,
							0.8, -0.24, 1, 1, 1,
							0.8, -0.26, 1, 1, 1,

							0.7, -0.24, 1, 1, 1,
							0.7, -0.26, 1, 1, 1,
							0.8, -0.26, 1, 1, 1,

							0.7, -0.34, 1, 1, 1,
							0.8, -0.34, 1, 1, 1,
							0.8, -0.36, 1, 1, 1,

							0.7, -0.34, 1, 1, 1,
							0.7, -0.36, 1, 1, 1,
							0.8, -0.36, 1, 1, 1,

							// 7

							-0.22, 0.36, 1, 1, 1,
							-0.2, 0.36, 1, 1, 1,
							-0.2, 0.14, 1, 1, 1,

							-0.22, 0.36, 1, 1, 1,
							-0.22, 0.14, 1, 1, 1,
							-0.2, 0.14, 1, 1, 1,

							// HORIZONTAL

							-0.3, 0.34, 1, 1, 1,
							-0.2, 0.34, 1, 1, 1,
							-0.2, 0.36, 1, 1, 1,

							-0.3, 0.34, 1, 1, 1,
							-0.3, 0.36, 1, 1, 1,
							-0.2, 0.36, 1, 1, 1,

							// 8

							// HORIZONTAL
							0.2, 0.36, 1, 1, 1,
							0.3, 0.36, 1, 1, 1,
							0.3, 0.34, 1, 1, 1,

							0.2, 0.36, 1, 1, 1,
							0.2, 0.34, 1, 1, 1,
							0.3, 0.34, 1, 1, 1,

							0.2, 0.26, 1, 1, 1,
							0.3, 0.26, 1, 1, 1,
							0.3, 0.24, 1, 1, 1,

							0.2, 0.26, 1, 1, 1,
							0.2, 0.24, 1, 1, 1,
							0.3, 0.24, 1, 1, 1,

							0.2, 0.16, 1, 1, 1,
							0.3, 0.16, 1, 1, 1,
							0.3, 0.14, 1, 1, 1,

							0.2, 0.16, 1, 1, 1,
							0.2, 0.14, 1, 1, 1,
							0.3, 0.14, 1, 1, 1,

							// VERTICAL
							0.28, 0.36, 1, 1, 1,
							0.3, 0.36, 1, 1, 1,
							0.3, 0.14, 1, 1, 1,

							0.28, 0.36, 1, 1, 1,
							0.28, 0.14, 1, 1, 1,
							0.3, 0.14, 1, 1, 1,

							0.2, 0.36, 1, 1, 1,
							0.22, 0.36, 1, 1, 1,
							0.22, 0.14, 1, 1, 1,


							0.2, 0.36, 1, 1, 1,
							0.2, 0.14, 1, 1, 1,
							0.22, 0.14, 1, 1, 1,

							// 9

							0.7, 0.36, 1, 1, 1,
							0.72, 0.36, 1, 1, 1,
							0.72, 0.24, 1, 1, 1,

							0.7, 0.36, 1, 1, 1,
							0.7, 0.24, 1, 1, 1,
							0.72, 0.24, 1, 1, 1,

							0.78, 0.36, 1, 1, 1,
							0.8, 0.36, 1, 1, 1,
							0.8, 0.14, 1, 1, 1,

							0.78, 0.36, 1, 1, 1,
							0.78, 0.14, 1, 1, 1,
							0.8, 0.14, 1, 1, 1,

							// HORIZONTAL

							0.7, 0.36, 1, 1, 1,
							0.8, 0.36, 1, 1, 1,
							0.8, 0.34, 1, 1, 1,

							0.7, 0.36, 1, 1, 1,
							0.7, 0.34, 1, 1, 1,
							0.8, 0.34, 1, 1, 1,

							0.7, 0.26, 1, 1, 1,
							0.8, 0.26, 1, 1, 1,
							0.8, 0.24, 1, 1, 1,

							0.7, 0.26, 1, 1, 1,
							0.7, 0.24, 1, 1, 1,
							0.8, 0.24, 1, 1, 1,

							0.7, 0.16, 1, 1, 1,
							0.8, 0.16, 1, 1, 1,
							0.8, 0.14, 1, 1, 1,

							0.7, 0.16, 1, 1, 1,
							0.7, 0.14, 1, 1, 1,
							0.8, 0.14, 1, 1, 1,
						};

						glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

						glBindVertexArray(vao[1]);

						auto xyPos = glGetAttribLocation(shaderProgram, "xy");
						auto rgbPos = glGetAttribLocation(shaderProgram, "rgb");

						glVertexAttribPointer(xyPos, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
						glVertexAttribPointer(rgbPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));

						glEnableVertexAttribArray(xyPos);
						glEnableVertexAttribArray(rgbPos);

						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);

						{
							if (xyPos == -1)
								throw runtime_error("Could not find \"xy\"");
							if (rgbPos == -1)
								throw runtime_error("Could not find \"rgb\"");
						}
					}

					// For indents in result's line
					const auto stridePos = glGetUniformLocation(shaderProgram, "stride");
					glUniform1f(stridePos, 0);
					
					// RESULT NUMS
					{
						glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

						double stride = 0.5;
						double insideStride = 0.02;

						// VERY DANGEROUS FOR LIFE AND THE PSYCHE!!
						GLfloat data[] = {
							// 0: 0-12; 1: 12-6; 2: 18-30; 3: 48-24; 4: 72-18; 5: 90-30; 6: 120-30; 7: 150-12; 8: 162-30; 9: 192-30: E:222-24
						
							// 0
							-0.8 + 1.5, -0.65 + 2.0, 1, 1, 1,
							-0.7 + 1.5, -0.65 + 2.0, 1, 1, 1,
							-0.7 + 1.5, -0.85 + 2.0, 1, 1, 1,

							-0.8 + 1.5, -0.65 + 2.0, 1, 1, 1,
							-0.8 + 1.5, -0.85 + 2.0, 1, 1, 1,
							-0.7 + 1.5, -0.85 + 2.0, 1, 1, 1,

							-0.8 + 1.5 + insideStride, -0.65 - insideStride + 2.0, 0.27, 0.51, 0.71,
							-0.7 + 1.5 - insideStride, -0.65 - insideStride + 2.0, 0.27, 0.51, 0.71,
							-0.7 + 1.5 - insideStride, -0.85 + insideStride + 2.0, 0.27, 0.51, 0.71,

							-0.8 + 1.5 + insideStride, -0.65 - insideStride + 2.0, 0.27, 0.51, 0.71,
							-0.8 + 1.5 + insideStride, -0.85 + insideStride + 2.0, 0.27, 0.51, 0.71,
							-0.7 + 1.5 - insideStride, -0.85 + insideStride + 2.0, 0.27, 0.51, 0.71,

							// 1
									-0.24 + 1.0, -0.65 + 2.0, 1, 1, 1,
									-0.26 + 1.0, -0.65 + 2.0, 1, 1, 1,
									-0.26 + 1.0, -0.85 + 2.0, 1, 1, 1,

									-0.24 + 1.0, -0.65 + 2.0, 1, 1, 1,
									-0.24 + 1.0, -0.85 + 2.0, 1, 1, 1,
									-0.26 + 1.0, -0.85 + 2.0, 1, 1, 1,

									// 2

									/* HORIZONTAL*/
									0.2 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.66 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.2 + 0.5, -0.66 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.66 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.76 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.2 + 0.5, -0.76 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.76 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.84 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.84 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.86 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.84 + 2.0, 1, 1, 1,
									0.2 + 0.5, -0.86 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.86 + 2.0, 1, 1, 1,

									// VERTICAL
									0.28 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.76 + 2.0, 1, 1, 1,

									0.28 + 0.5, -0.64 + 2.0, 1, 1, 1,
									0.28 + 0.5, -0.76 + 2.0, 1, 1, 1,
									0.3 + 0.5, -0.76 + 2.0, 1, 1, 1,

									0.2 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.22 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.22 + 0.5, -0.86 + 2.0, 1, 1, 1,


									0.2 + 0.5, -0.74 + 2.0, 1, 1, 1,
									0.2 + 0.5, -0.86 + 2.0, 1, 1, 1,
									0.22 + 0.5, -0.86 + 2.0, 1, 1, 1,

									// 3

									// HORIZONTAL
									0.7, -0.64 + 2.0, 1, 1, 1,
									0.8, -0.64 + 2.0, 1, 1, 1,
									0.8, -0.66 + 2.0, 1, 1, 1,

									0.7, -0.64 + 2.0, 1, 1, 1,
									0.7, -0.66 + 2.0, 1, 1, 1,
									0.8, -0.66 + 2.0, 1, 1, 1,

									0.7, -0.74 + 2.0, 1, 1, 1,
									0.8, -0.74 + 2.0, 1, 1, 1,
									0.8, -0.76 + 2.0, 1, 1, 1,

									0.7, -0.74 + 2.0, 1, 1, 1,
									0.7, -0.76 + 2.0, 1, 1, 1,
									0.8, -0.76 + 2.0, 1, 1, 1,

									0.7, -0.84 + 2.0, 1, 1, 1,
									0.8, -0.84 + 2.0, 1, 1, 1,
									0.8, -0.86 + 2.0, 1, 1, 1,

									0.7, -0.84 + 2.0, 1, 1, 1,
									0.7, -0.86 + 2.0, 1, 1, 1,
									0.8, -0.86 + 2.0, 1, 1, 1,

									// VERTICAL
									0.78, -0.64 + 2.0, 1, 1, 1,
									0.8, -0.64 + 2.0, 1, 1, 1,
									0.8, -0.86 + 2.0, 1, 1, 1,

									0.78, -0.64 + 2.0, 1, 1, 1,
									0.78, -0.86 + 2.0, 1, 1, 1,
									0.8, -0.86 + 2.0, 1, 1, 1,

									// 4
									-0.3 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.28 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.28 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,

									-0.3 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.3 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,
									-0.28 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,

									-0.22 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.36 + 1.0 + 0.5, 1, 1, 1,

									-0.22 + 1.0, -0.14 + 1.0 + 0.5, 1, 1, 1,
									-0.22 + 1.0, -0.36 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.36 + 1.0 + 0.5, 1, 1, 1,

									-0.3 + 1.0, -0.24 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.24 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,

									-0.3 + 1.0, -0.24 + 1.0 + 0.5, 1, 1, 1,
									-0.3 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,
									-0.2 + 1.0, -0.26 + 1.0 + 0.5, 1, 1, 1,

									// 5

									0.2 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.22 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.22 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.2 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,
									0.22 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.28 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.28 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.28 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// HORIZONTAL

									0.2 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.2 + 0.5, -0.16 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.2 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.2 + 0.5, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.2 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.3 + 0.5, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// 6

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.78, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.78, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.78, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// HORIZONTAL

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.16 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.26 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// 7

									-0.22 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.14 + 0.5 + 0.5, 1, 1, 1,

									-0.22 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,
									-0.22 + 1, 0.14 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.14 + 0.5 + 0.5, 1, 1, 1,

									// HORIZONTAL

									-0.3 + 1, 0.34 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.34 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,

									-0.3 + 1, 0.34 + 0.5 + 0.5, 1, 1, 1,
									-0.3 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,
									-0.2 + 1, 0.36 + 0.5 + 0.5, 1, 1, 1,

									// 8

									/* HORIZONTAL*/
									0.7, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.84 + 0.5, 1, 1, 1,

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.7, 0.84 + 0.5, 1, 1, 1,
									0.8, 0.84 + 0.5, 1, 1, 1,

									0.7, 0.76 + 0.5, 1, 1, 1,
									0.8, 0.76 + 0.5, 1, 1, 1,
									0.8, 0.74 + 0.5, 1, 1, 1,

									0.7, 0.76 + 0.5, 1, 1, 1,
									0.7, 0.74 + 0.5, 1, 1, 1,
									0.8, 0.74 + 0.5, 1, 1, 1,

									0.7, 0.66 + 0.5, 1, 1, 1,
									0.8, 0.66 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									0.7, 0.66 + 0.5, 1, 1, 1,
									0.7, 0.64 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									// VERTICAL
									0.78, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									0.78, 0.86 + 0.5, 1, 1, 1,
									0.78, 0.64 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.72, 0.86 + 0.5, 1, 1, 1,
									0.72, 0.64 + 0.5, 1, 1, 1,


									0.7, 0.86 + 0.5, 1, 1, 1,
									0.7, 0.64 + 0.5, 1, 1, 1,
									0.72, 0.64 + 0.5, 1, 1, 1,

									// 9

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.72, 0.86 + 0.5, 1, 1, 1,
									0.72, 0.74 + 0.5, 1, 1, 1,

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.7, 0.74 + 0.5, 1, 1, 1,
									0.72, 0.74 + 0.5, 1, 1, 1,

									0.78, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									0.78, 0.86 + 0.5, 1, 1, 1,
									0.78, 0.64 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									// HORIZONTAL

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.86 + 0.5, 1, 1, 1,
									0.8, 0.84 + 0.5, 1, 1, 1,

									0.7, 0.86 + 0.5, 1, 1, 1,
									0.7, 0.84 + 0.5, 1, 1, 1,
									0.8, 0.84 + 0.5, 1, 1, 1,

									0.7, 0.76 + 0.5, 1, 1, 1,
									0.8, 0.76 + 0.5, 1, 1, 1,
									0.8, 0.74 + 0.5, 1, 1, 1,

									0.7, 0.76 + 0.5, 1, 1, 1,
									0.7, 0.74 + 0.5, 1, 1, 1,
									0.8, 0.74 + 0.5, 1, 1, 1,

									0.7, 0.66 + 0.5, 1, 1, 1,
									0.8, 0.66 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									0.7, 0.66 + 0.5, 1, 1, 1,
									0.7, 0.64 + 0.5, 1, 1, 1,
									0.8, 0.64 + 0.5, 1, 1, 1,

									// ERROR

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.72, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// HORIZONTAL

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.14 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.16 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.16 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.26 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.34 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.36 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.36 + 1.0 + 0.5, 1, 1, 1,

									// MINUS

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1,

									0.7, -0.24 + 1.0 + 0.5, 1, 1, 1,
									0.7, -0.26 + 1.0 + 0.5, 1, 1, 1,
									0.8, -0.26 + 1.0 + 0.5, 1, 1, 1

						};

						glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

						glBindVertexArray(vao[2]);

						auto xyPos = glGetAttribLocation(shaderProgram, "xy");
						auto rgbPos = glGetAttribLocation(shaderProgram, "rgb");

						glVertexAttribPointer(xyPos, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
						glVertexAttribPointer(rgbPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));

						glEnableVertexAttribArray(xyPos);
						glEnableVertexAttribArray(rgbPos);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);

						{
							if (xyPos == -1)
								throw runtime_error("Could not find \"xy\"");
							if (rgbPos == -1)
								throw runtime_error("Could not find \"rgb\"");
						}

					}

					glClearColor(1, 1, 1, 1);

					string first_statement;
					string second_statement;
					string result;

					// Operations between statements
					enum op {
						STANDARD,
						PLUS,
						MINUS,
						MUL,
						DIV
					};

					// Current operator (+,-,*,/, STANDARD (off))
					op oper = STANDARD;

					bool isFirstStatement = true;

					// Optionally (for saving data in some moments)
					bool isEndResult = false;

					glClearColor(0.4, 0.4, 0.4, 1);

					// It would be cool. I guess...
					while (!glfwWindowShouldClose(window)) {

						glClear(GL_COLOR_BUFFER_BIT);

						// Draw buttons
						glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
						glBindVertexArray(vao[0]);

						glDrawArrays(GL_TRIANGLES, 0, 1000);

						// Draw nums
						glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
						glBindVertexArray(vao[1]);

						glDrawArrays(GL_TRIANGLES, 0, 2220);

						/* ===========================================
											CALCULATOR
						=========================================== */
						/*
						INSTRUCTION
							1. the First number is written as a string in first_statement;
							2. Selected operator;
							3. the Second number is written as a string in second_statement;
							4. Both expressions are converted to numbers, added together, and converted 
							   back to a string for display on the panel.
						
						
						*/
						/*
						1 0.3 0.8 - 0.45 0.95
						2 0.55 0.8 - 0.7 0.95
						3 0.8 0.8 - 0.95 0.95
						*/

						// If mouse on button "1"
						if (onMouse == 1 && X > 0.3 && Y > 0.84 && X < 0.45 && Y < 0.95) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('1');
							else
								second_statement.push_back('1');
							result.push_back('1');

						}

						// If mouse on button "2"
						if (onMouse == 1 && X > 0.55 && Y > 0.84 && X < 0.7 && Y < 0.95) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('2');
							else
								second_statement.push_back('2');
							result.push_back('2');

						}

						// If mouse on button "3"
						if (onMouse == 1 && X > 0.8 && Y > 0.84 && X < 0.95 && Y < 0.95) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();
							if (isFirstStatement)
								first_statement.push_back('3');
							else
								second_statement.push_back('3');
							result.push_back('3');

						}

						/*
						4 0.3 0.55 - 0.45 0.7
						5 0.55 0.55 - 0.7 0.7
						6 0.8 0.55 - 0.95 0.7
						*/

						// If mouse on button "4"
						if (onMouse == 1 && X > 0.3 && Y > 0.64 && X < 0.45 && Y < 0.75) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('4');
							else
								second_statement.push_back('4');
							result.push_back('4');

						}

						// If mouse on button "5"
						if (onMouse == 1 && X > 0.55 && Y > 0.64 && X < 0.7 && Y < 0.75) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('5');
							else
								second_statement.push_back('5');
							result.push_back('5');

						}

						// If mouse on button "6"
						if (onMouse == 1 && X > 0.8 && Y > 0.64 && X < 0.95 && Y < 0.75) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('6');
							else
								second_statement.push_back('6');
							result.push_back('6');

						}

						/*
						7 0.3 0.3 - 0.45 0.45
						8 0.55 0.3 - 0.7 0.45
						9 0.8 0.3 - 0.95 0.45
						*/

						// If mouse on button "7"
						if (onMouse == 1 && X > 0.3 && Y > 0.44 && X < 0.45 && Y < 0.56) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('7');
							else
								second_statement.push_back('7');
							result.push_back('7');

						}

						// If mouse on button "8"
						if (onMouse == 1 && X > 0.55 && Y > 0.44 && X < 0.7 && Y < 0.56) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('8');
							else
								second_statement.push_back('8');
							result.push_back('8');

						}

						// If mouse on button "9"
						if (onMouse == 1 && X > 0.8 && Y > 0.44 && X < 0.95 && Y < 0.56) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (result.size() == 1 && result[0] == '0' || (oper != STANDARD && second_statement.empty()))
								result.clear();

							if (isFirstStatement)
								first_statement.push_back('9');
							else
								second_statement.push_back('9');
							result.push_back('9');

						}

						/*
						0 0.05 0.8 - 0.2 0.95
						+ 0.05 0.55 - 0.2 0.7
						- 0.05 0.3 - 0.2 0.45
						= 0.05 0.15 - 0.2 0.2
						*/

						// If mouse on button "0"
						if (onMouse == 1 && X > 0.05 && Y > 0.84 && X < 0.2 && Y < 0.95) {

							if (isEndResult) {
								result.clear();
								isEndResult = false;
							}

							if (isFirstStatement)
								first_statement.push_back('0');
							else
								second_statement.push_back('0');

							if (!result.empty() && result[0] != '0')
								result.push_back('0');

						}

						// If mouse on button "+"
						if (onMouse == 1 && X > 0.05 && Y > 0.64 && X < 0.2 && Y < 0.76) {

							isFirstStatement = false;

							oper = PLUS;

						}

						// If mouse on button "-"
						if (onMouse == 1 && X > 0.05 && Y > 0.44 && X < 0.2 && Y < 0.56) {
							isFirstStatement = false;

							oper = MINUS;
						}

						// If mouse on button "*"
						if (onMouse == 1 && X > 0.3 && Y > 0.24 && X < 0.45 && Y < 0.36) {
							isFirstStatement = false;

							oper = MUL;
						}

						// If mouse on button "/"
						if (onMouse == 1 && X > 0.55 && Y > 0.24 && X < 0.7 && Y < 0.36) {
							isFirstStatement = false;

							oper = DIV;
						}

						// If mouse on button "AC"
						if (onMouse == 1 && X > 0.8 && Y > 0.24 && X < 0.95 && Y < 0.36) {
							isFirstStatement = true;
							oper = STANDARD;
							result = "0";
						}

						// If mouse on button "="
						if (onMouse == 1 && X > 0.05 && Y > 0.24 && X < 0.2 && Y < 0.36) {

							auto s1 = strtol(first_statement.data(), nullptr, 10);
							auto s2 = strtol(second_statement.data(), nullptr, 10);

							switch (oper) {
							case PLUS: {
								if (!second_statement.empty())
									result = toString(s1 + s2);
								else
									result = toString(s1 + s1);
								break; }
							case MINUS: {
								if (!second_statement.empty())
									result = toString(s1 - s2);
								else
									result = toString(s1 - s1);
								break; }
							case MUL: {
								if (!second_statement.empty())
									result = toString(s1 * s2);
								else
									result = toString(s1 * s1);
								break; }
							case DIV: {
								if (!second_statement.empty())
									result = toString(s1 / s2);
								else
									result = toString(s1 / s1);
								break; }
							case STANDARD: {
								result = toString(s1);
								break;
							}

							}

							first_statement.clear();
							second_statement.clear();

							isEndResult = true;
							isFirstStatement = true;

							oper = STANDARD;
						}

						// Draw result
						glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
						glBindVertexArray(vao[2]);

						// If you create very long num
						if (strtol(result.data(), nullptr, 10) == INT_MAX) {
							result.clear();
							glDrawArrays(GL_TRIANGLES, 222, 24);

						}

						// Position of the digit number
						int rang = 0;
						for (auto i = result.crbegin(); i != result.crend(); ++i) {

							glUniform1f(stridePos, rang++);

							// 0: 0-12; 1: 12-6; 2: 18-30; 3: 48-24; 4: 72-18; 5: 90-30; 6: 120-30; 7: 150-12; 8: 162-30; 9: 192-30
							switch (*i) {
							case '1': {
								glDrawArrays(GL_TRIANGLES, 12, 6);
								break;
							}
							case '2': {
								glDrawArrays(GL_TRIANGLES, 18, 30);
								break;
							}
							case '3': {
								glDrawArrays(GL_TRIANGLES, 48, 24);
								break;
							}
							case '4': {
								glDrawArrays(GL_TRIANGLES, 72, 18);
								break;
							}
							case '5': {
								glDrawArrays(GL_TRIANGLES, 90, 30);
								break;
							}
							case '6': {
								glDrawArrays(GL_TRIANGLES, 120, 30);
								break;
							}
							case '7': {
								glDrawArrays(GL_TRIANGLES, 150, 12);
								break;
							}
							case '8': {
								glDrawArrays(GL_TRIANGLES, 162, 30);
								break;
							}
							case '9': {
								glDrawArrays(GL_TRIANGLES, 192, 30);
								break;
							}
							case '0': {
								glDrawArrays(GL_TRIANGLES, 0, 12);
								break;
							}
							case '-': {
								glDrawArrays(GL_TRIANGLES, 246, 6);
								break;
							}
							}
						}

						// Reset
						glUniform1f(stridePos, 0);

						glfwSwapBuffers(window);
						glfwWaitEvents();
					}
				}
				break;
			}
			case 0: { exit(0); }
			}
		}
	}
	catch (const exception & e) {
		glfwTerminate();
		std::cout << e.what() << endl;
		return EXIT_FAILURE;
	}
}


/*
char* initComputeShaderRsc[] = {
						"#version 430 core\n",
						"layout(local_size_x = 1) in;\n",
						"uniform float minSpeed;\n",
						"uniform float maxSpeed;\n",
						"uniform float t;\n",
						"struct Point{\n",
						"float t;\n",
						"vec2 p;\n",
						"vec2 v;\n",
						"};\n",
						"layout(std430, binding = 0) buffer ssbo {\n",
						"Point points[];\n",
						"};\n",
						"uint xxhash(uint x, uint y) {\n",
						"x = x*3266489917 + 374761393;\n",
						"x = (x << 17) | (x >> 15);\n",
						"x += y * 3266489917;\n",
						"x *= 668265263;\n",
						"x ^= x >> 15;\n",
						"x *= 2246822519;\n",
						"x ^= x >> 13;\n",
						"x *= 3266489917;\n",
						"x ^= x >> 16;\n",
						"return x;}\n",
						"uint xorshiftState = xxhash(floatBitsToUint(t), gl_GlobalInvocationID.x);\n",
						"float xorshift() {\n",
						"uint x = xorshiftState;\n",
						"x ^= x << 13;\n",
						"x ^= x >> 17;\n",
						"x ^= x << 5;\n",
						"xorshiftState = x;\n",
						"uint m = 0x00ffffff;\n",
						"return (x & m) * (1.0f / 0x1000000);\n",
						"}\n",
						"const float PI = 3.14159;\n",
						"void main() {\n",
						"Point point = points[gl_GlobalInvocationID.x];\n",
						"float speed = minSpeed + xorshift() * (maxSpeed - minSpeed);\n",
						"float angle = 2 * PI * xorshift();\n",
						"point.t = t;\n",
						"point.p.x = -1 + xorshift() * 2;\n",
						"point.p.y = -1 + xorshift() * 2;\n",
						"point.v.x = speed * cos(angle);\n",
						"point.v.y = speed * sin(angle);\n",
						"points[gl_GlobalInvocationID.x] = point;\n",
						"}\n"
					};


*/