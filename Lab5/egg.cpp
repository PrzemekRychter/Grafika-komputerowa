// Copyright 2020 Przemek Rychter [legal/copyright]
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <cmath>
//#include <libsync.h>

int model = 3;
static GLfloat angleX = 0.0;  // obrót wokoł osi oX ---
static GLint left;
static GLdouble angleY = 0.0;  // obrót wokoł osi oY |
bool spin_enabled = false;
double xPosInt;
double yPosInt;
typedef float point3[3];
class Point {
 public:
  float x, y, z;
};

std::vector<std::vector<Point>> normalVectorR;
static GLfloat viewer[] = {0.0, 0.0, 1.0};

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
// Struktura dla nagłówka pliku  TGA
#pragma pack(1)
typedef struct {
  GLbyte idlength;
  GLbyte colormaptype;
  GLbyte datatypecode;
  unsigned short colormapstart;
  unsigned short colormaplength;
  unsigned char colormapdepth;
  unsigned short x_orgin;
  unsigned short y_orgin;
  unsigned short width;
  unsigned short height;
  GLbyte bitsperpixel;
  GLbyte descriptor;
} TGAHEADER;
#pragma pack(8)

FILE *pFile;
TGAHEADER tgaHeader;
unsigned long lImageSize;
short sDepth;
GLbyte *pbitsperpixel = NULL;

// Wartości domyślne zwracane w przypadku błędu

*ImWidth = 0;
*ImHeight = 0;
*ImFormat = GL_BGR_EXT;
*ImComponents = GL_RGB8;

pFile = fopen(FileName, "rb");
if (pFile == NULL) return NULL;
// Przeczytanie nagłówka pliku
fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
// Odczytanie szerokości, wysokości i głębi obrazu

*ImWidth = tgaHeader.width;
*ImHeight = tgaHeader.height;
sDepth = tgaHeader.bitsperpixel / 8;

// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)

if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 &&
    tgaHeader.bitsperpixel != 32)
  return NULL;

// Obliczenie rozmiaru bufora w pamięci

lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
// Alokacja pamięci dla danych obrazu

pbitsperpixel = (GLbyte *)malloc(lImageSize * sizeof(GLbyte));

if (pbitsperpixel == NULL) return NULL;

if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
  free(pbitsperpixel);
  return NULL;
}

// Ustawienie formatu OpenGL

switch (sDepth) {
  case 3:
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
    break;
  case 4:
    *ImFormat = GL_BGRA_EXT;
    *ImComponents = GL_RGBA8;
    break;
  case 1:
    *ImFormat = GL_LUMINANCE;
    *ImComponents = GL_LUMINANCE8;
    break;
};

fclose(pFile);

return pbitsperpixel;
}

void calculate(std::vector<std::vector<Point>> &points, int n) {  // NOLINT
  point3 normalVector[n][n];
  // 3. Nałożyć na kwadrat jednostkowy dziedziny parametryczne
  // równomierną siatkę NxN punktów.
  for (int i = 0; i < n; i++) {
    std::vector<Point> row;
    std::vector<Point> rowN;
    points.push_back(row);
    normalVectorR.push_back(rowN);
    for (int j = 0; j < n; j++) {
      float u = static_cast<float>(i) / (n - 1);
      float v = static_cast<float>(j) / (n - 1);
      // 4. Dla każdego punktu u, v nałożonej w kroku poprzednim siatki,
      // obliczyć, przy pomocy podanych
      // wyżej rówanań współrzędne x(u, v), y(u, v) i z(u, v) i zapisać je w
      // zadeklarowanej
      // w kroku 2 tablicy.
      Point p = Point();
      points[i].push_back(p);
      points[i][j].x = ((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) +
                         180 * pow(u, 2) - 45 * u) *
                        cos(M_PI * v)) /
                       10.0f;
      points[i][j].y =
          (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2)) / 10.0f - 0.5f;
      points[i][j].z = ((-90 * pow(u, 5) + 225 * powf(u, 4) - 270 * powf(u, 3) +
                         180 * powf(u, 2) - 45 * u) *
                        sin(M_PI * v)) /
                       10.0f;
      float x_u = ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) +
                    360 * u - 45) *
                   cos(M_PI * u));
      float x_v = (M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) -
                           180 * pow(u, 2) + 45 * u) *
                   sin(M_PI * u));
      float y_u = (640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
      float y_v = 0;
      float z_u = ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) +
                    360 * u - 45) *
                   sin(M_PI * u));
      float z_v = (-M_PI * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) -
                            180 * pow(u, 2) + 45 * u) *
                   cos(M_PI * u));
      Point pNom = Point();
      normalVectorR[i].push_back(pNom);
      normalVectorR[i][j].x = y_u * z_v - z_u * y_v;
      normalVectorR[i][j].y = z_u * x_v - x_u * z_v;
      normalVectorR[i][j].z = x_u * y_v - y_u * x_v;
      float length =
          sqrt(pow(normalVectorR[i][j].x, 2) + pow(normalVectorR[i][j].y, 2) +
               pow(normalVectorR[i][j].z, 2));
      if (i < n / 2) {
        if (normalVectorR[i][j].x != 0)
          normalVectorR[i][j].x = (y_u * z_v - z_u * y_v) / length;
        if (normalVectorR[i][j].y != 0)
          normalVectorR[i][j].y = (z_u * x_v - x_u * z_v) / length;
        if (normalVectorR[i][j].z != 0)
          normalVectorR[i][j].z = (x_u * y_v - y_u * x_v) / length;
      } else if (i > n / 2) {
        if (normalVectorR[i][j].x != 0)
          normalVectorR[i][j].x = -1 * (y_u * z_v - z_u * y_v) / length;
        if (normalVectorR[i][j].y != 0)
          normalVectorR[i][j].y = -1 * (z_u * x_v - x_u * z_v) / length;
        if (normalVectorR[i][j].z != 0)
          normalVectorR[i][j].z = -1 * (x_u * y_v - y_u * x_v) / length;
      }
    }
  }
}

void draw(std::vector<std::vector<Point>> &points, int n) {  // NOLINT
  if (model == 1) {
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < n; j++) {
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glEnd();
      }
    }
  } else if (model == 2) {
    // Rysowanie okręgów
    for (int i = 1; i <= n / 2; i++) {
      glColor3f(0.0f, 1.0f, 0.0f);
      // Połowa koła z tyłu
      for (int j = 0; j < n - 1; j++) {
        glBegin(GL_LINES);
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glEnd();
      }
      // Łączenie połów z prawej
      int rem_i = i;
      glBegin(GL_LINES);
      int last = points[i].size() - 1;
      glVertex3f(points[i][last].x, points[i][last].y, points[i][last].z);
      i = points.size() - i;  // dlatego w nadrzednym for nie moze byc i = 0;
      glVertex3f(points[i][0].x, points[i][0].y, points[i][0].z);

      // Połowa koła z przodu
      for (int j = 0; j < n - 1; j++) {
        glBegin(GL_LINES);
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glEnd();
      }
      // Łaczenie połów z lewej
      glBegin(GL_LINES);
      last = points[i].size() - 1;
      glVertex3f(points[i][last].x, points[i][last].y, points[i][last].z);
      i = rem_i;
      glVertex3f(points[i][0].x, points[i][0].y, points[i][0].z);
      glEnd();
    }
    // Rysowanie linii biegnących od "bieguna do bieguna"
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n - 1; j++) {
        glBegin(GL_LINES);
        glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
        glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
        glEnd();
      }
    }
    // Korekta (na obrazie nie było częsci lini siatki)
    for (int j = 0; j < n; j++) {
      glBegin(GL_LINES);
      glVertex3f(points[0][0].x, points[0][0].y, points[0][0].z);
      glVertex3f(points[n - 1][j].x, points[n - 1][j].y, points[n - 1][j].z);
      glEnd();
    }
  } else if (model == 3) {
    for (int i = 0; i < n - 1; i++) {
      for (int j = 0; j < n - 1; j++) {
        glColor3f(0.f, 0.0f, 0.5f);
        glBegin(GL_TRIANGLES);
        point3 p = {normalVectorR[i][j].x, normalVectorR[i][j].y,
                    normalVectorR[i][j].z};
        glNormal3fv(p);
        glTexCoord2f(1.0f, 0.0f);
        point3 p1 = {points[i][j].x, points[i][j].y, points[i][j].z};
        glVertex3fv(p1);
        point3 p44 = {normalVectorR[i][j + 1].x, normalVectorR[i][j + 1].y,
                      normalVectorR[i][j + 1].z};
        glNormal3fv(p44);
        glTexCoord2f(0.0f, 1.0f);
        point3 p2 = {points[i][j + 1].x, points[i][j + 1].y,
                     points[i][j + 1].z};
        glVertex3fv(p2);
        point3 p31 = {normalVectorR[i + 1][j].x, normalVectorR[i + 1][j].y,
                      normalVectorR[i + 1][j].z};
        glNormal3fv(p31);
        glTexCoord2f(0.0f, 0.0f);
        point3 p3 = {points[i + 1][j].x, points[i + 1][j].y,
                     points[i + 1][j].z};
        glVertex3fv(p3);
        glEnd();
        glBegin(GL_TRIANGLES);
        glNormal3fv(p31);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3fv(p3);
        point3 pp = {normalVectorR[i + 1][j + 1].x,
                     normalVectorR[i + 1][j + 1].y,
                     normalVectorR[i + 1][j + 1].z};
        glNormal3fv(pp);
        glTexCoord2f(1.0f, 0.0f);
        point3 p4 = {points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                     points[i + 1][j + 1].z};
        glVertex3fv(p4);
        glNormal3fv(p44);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3fv(p2);
        glEnd();
      }
    }
  }
}

static void cursorPositionCallback(GLFWwindow *window, double xPos,
                                   double yPos) {
  if (left) {
    double xDif = xPos - xPosInt;
    double yDif = yPos - yPosInt;
    std::cout << xDif << " : " << yDif << std::endl;
    angleX = angleX + yDif / 100;
    if (angleX > 360) angleX = 0;
    if (angleX < 0) angleX = 360 + angleX;

    angleY = angleY + xDif / 100;
    if (angleY > 360) angleY = 0;
    if (angleY < 0) angleY = 360 + angleY;
    std::cout << "angle X: " << angleX << std::endl;
    std::cout << "angle Y: " << angleY << std::endl;
  }
  if (!left) {
    xPosInt = xPos;
    yPosInt = yPos;
  }
}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // zmiany yPos powinny powodowac obrót jajka wokoł osi oX ----
    left = 1;
    std::cout << "LEFT MOUSE BUTTON PRESSED" << std::endl;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    // zmiany yPos powinny powodowac obrót jajka wokoł osi oX ----
    left = 0;
    std::cout << "LEFT MOUSE BUTTON RELEASED" << std::endl;
  }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    model = 1;
  }
  if (key == GLFW_KEY_L && action == GLFW_PRESS) {
    model = 2;
  }
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    model = 3;
  }
  if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    spin_enabled = !spin_enabled;
  }
}
int main(void) {
  std::cout << "Podaj ilość punktów: " << std::endl;
  int n;
  std::cin >> n;
  int model = 1;
  GLFWwindow *window;
  // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
  /* Initialize the library */
  if (!glfwInit()) return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(960, 960, "egg", NULL, NULL);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, cursorPositionCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // 2. Zadeklarować tablicę o rozmiarze NxN, która będzie służyła do
  // zapisywania współrzednych
  // punktów w przestrzeni 3-D. Każdy element tablicy zawierał będzie trzy
  // liczby będące
  // współrzędnymi x, y, z jednego punktu.
  std::vector<std::vector<Point>> points;
  calculate(points, n);

  // Zmienne dla obrazu tekstury
  GLbyte *pBytes;
  GLint ImWidth, ImHeight, ImComponents;
  GLenum ImFormat;
  //  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
  pBytes = LoadTGAImage("t_256.tga", &ImWidth, &ImHeight, &ImComponents,
                        &ImFormat);
  // Zdefiniowanie tekstury 2-D
  glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat,
               GL_UNSIGNED_BYTE, pBytes);
  // Zwolnienie pamięci
  free(pBytes);
  // Włączenie mechanizmu teksturowania
  glEnable(GL_TEXTURE_2D);
  // Ustalenie trybu teksturowania
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // Określenie sposobu nakładania tekstur
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  float spin = 0.0f;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess = { 100.0 };
    GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = {1.0, 0.0, 0.3, 1.0 };
    GLfloat light_diffuse[] = { 0.6, 0.5, 0.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 0.2, 0.1, 1.0 };
    GLfloat att_constant = { 1.0 };
    GLfloat att_linear = { (GLfloat) 0.05 };
    GLfloat att_quadratic = { (GLfloat) 0.001 };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
  glShadeModel(GL_SMOOTH);  // właczenie łagodnego cieniowania
  glEnable(GL_LIGHTING);    // właczenie systemu oświetlenia sceny
  glEnable(GL_LIGHT0);      // włączenie źródła o numerze 0
  glEnable(GL_DEPTH_TEST);  // włączenie mechanizmu z-bufora

  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    // Clear information from last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glRotatef(angleY, 0.0, 1.0, 0.0);  // obrót , os orbotu oY
    glRotatef(angleX, 1.0, 0.0, 0.0);  // Obrót o 30 stopni, oś obrotu = oX
    glRotatef(spin, 0, 1.0, 0.0);

    //
    //    glRotatef(spin,0,1.0,0.0);
    draw(points, n);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    //  glfwWaitEventsTimeout(0.0001);
    glfwPollEvents();

    if (spin_enabled) spin += 0.50f;
    if (spin == 360.0f) spin = 0.0f;

    if (angleX == 360.0) angleX = 0.0;
    if (angleY == 360.0) angleY = 0.0;
  }
  glfwTerminate();
  return 0;
}
