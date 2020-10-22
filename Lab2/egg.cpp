#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#define POM 4
int model = 1;
class Point{
    public:
        float x,y,z;
};

void calculate(std::vector<std::vector<Point>> &points, int n){

    // 3. Nałożyć na kwadrat jednostkowy dziedziny parametrycznej równomierną siatkę NxN punktów.
    for(int i = 0; i < n; i++){
        std::vector<Point> row;
        points.push_back(row);
        for(int j = 0; j < n; j++){
            float u = (1.0f/n)* (float) i; 
            float v = (1.0f/n)* (float) j;
            // 4. Dla każdego punktu u, v nałożonej w kroku poprzednim siatki, obliczyć, przy pomocy podanych
            // wyżej rówanań współrzędne x(u, v), y(u, v) i z(u, v) i zapisać je w zadeklarowanej 
            // w kroku 2 tablicy.
            Point p = Point();            
            points[i].push_back(p);

            points[i][j].x = ( -90*pow(u,5) + 225*pow(u,4) - 270*pow(u,3) + 180*pow(u,2) - 45*u) * cos(M_PI*v);
            points[i][j].y = 160*pow(u,4) - 320*pow(u,3) + 160*pow(u,2) ;          
            points[i][j].z = ( -90*pow(u, 5) + 225* powf(u,4) - 270*powf(u, 3) + 180*powf(u,2) - 45*u) * sin(M_PI * v);

        }
    }
}

void draw(std::vector<std::vector<Point>> &points, int n){
    if(model == 1){
        for(int i = 1; i < n; i++){
            for(int j =0; j < n; j++){
                glBegin(GL_POINTS);
                glColor3f(1.0f, 1.0f, 0.0f);
                glVertex3f(points[i][j].x / 10.0f , points[i][j].y / 10.0f -0.5f  , points[i][j].z / 10.0f ) ;
                glEnd();       
            }
        }
    } else if (model == 2){
        // Rysowanie okręgów
        for(int i = 1; i <= n/2 ; i++){
            glColor3f(0.0f,1.0f,0.0f);
            //Połowa koła z tyłu
            for(int j=0;j<n-1;j++){   
                glBegin(GL_LINES);
                glVertex3f(points[i][j].x / 10.0f , points[i][j].y / 10.0f -0.5f  , points[i][j].z / 10.0f ) ;
                glVertex3f(points[i][j+1].x / 10.0f , points[i][j+1].y / 10.0f -0.5f  , points[i][j+1].z / 10.0f ) ;
                glEnd();
            }
            //Łączenie połów z prawej
            int rem_i = i;
            glBegin(GL_LINES);
            int last = points[i].size()-1;
            glVertex3f(points[i][last].x / 10.0f , points[i][last].y / 10.0f -0.5f  , points[i][last].z / 10.0f ) ;
            i = points.size()-i;    // dlatego w nadrzednym for nie moze byc i = 0;
            glVertex3f(points[i][0].x / 10.0f , points[i][0].y / 10.0f -0.5f  , points[i][0].z / 10.0f ) ;

            //Połowa koła z przodu
            for(int j=0;j<n-1;j++){
                glBegin(GL_LINES);
                glVertex3f(points[i][j].x / 10.0f , points[i][j].y / 10.0f -0.5f  , points[i][j].z / 10.0f ) ;
                glVertex3f(points[i][j+1].x / 10.0f , points[i][j+1].y / 10.0f -0.5f  , points[i][j+1].z / 10.0f ) ;
                glEnd();
            }
            //Łaczenie połów z lewej
            glBegin(GL_LINES);
            last = points[i].size()-1;
            glVertex3f(points[i][last].x / 10.0f , points[i][last].y / 10.0f -0.5f  , points[i][last].z / 10.0f ) ;
            i = rem_i;
            glVertex3f(points[i][0].x / 10.0f , points[i][0].y / 10.0f -0.5f  , points[i][0].z / 10.0f ) ;
            glEnd();
        }
        // Rysowanie linii biegnących od "bieguna do bieguna"
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n-1; j++){
                glBegin(GL_LINES);
                glVertex3f(points[j][i].x / 10.0f , points[j][i].y / 10.0f -0.5f  , points[j][i].z / 10.0f ) ;
                glVertex3f(points[j+1][i].x / 10.0f , points[j+1][i].y / 10.0f -0.5f  , points[j+1][i].z / 10.0f ) ;
                glEnd();
            }
        }
        for(int j = 0; j <n;j++){
            glBegin(GL_LINES);
            glVertex3f(points[0][0].x / 10.0f , points[0][0].y / 10.0f -0.5f  , points[0][0].z / 10.0f ) ;
            glVertex3f(points[n-1][j].x / 10.0f , points[n-1][j].y / 10.0f -0.5f  , points[n-1][j].z / 10.0f ) ;
            glEnd();
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS){
        model = 1;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        model = 2;
}
int main(void){

    std::cout << "Podaj ilość punktów: " << std::endl;
    int n;
    std::cin >> n;
    int model = 1;
    GLFWwindow* window;
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 960,"egg", NULL, NULL);
    glfwSetKeyCallback(window, keyCallback);    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // 2. Zadeklarować tablicę o rozmiarze NxN, która będzie służyła do zapisywania współrzednych 
    // punktów w przestrzeni 3-D. Każdy element tablicy zawierał będzie trzy liczby będące 
    // współrzędnymi x, y, z jednego punktu.
    std::vector<std::vector<Point>> points;
    calculate(points, n);

    float spin = 0.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        //Clear information from last draw
        glClear( GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);

        glRotated(30.0, 1.0, 0, 0);  // Obrót o 30 stopni, oś obrotu = oX

        glRotatef(spin,0,1.0,0.0);
        draw(points, n);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        //  glfwWaitEventsTimeout(0.0001);
        glfwPollEvents();
        spin += 0.05f;
        if(spin == 360.0f)
            spin = 0.0f;
    }

    glfwTerminate();
    return 0;
}


