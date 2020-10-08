#include <GLFW/glfw3.h>
#include <cmath> //std::abs()
#include <cstdlib> //std::rand,srand
#include <time.h> 
#include <string>
#include <iostream>
#include <vector>
typedef float point2 [2];

class Box {
    public:
        std::vector<float> xy;      //xy[0],xy[1] określają lewy górny róg dywanu
                                    //xy[2],xy[3] określają prawy dolny róg dywanu
        std::vector<float> colors;  //określenie 4x3 wartości RGB dla każdego rogu
};

//Funkcja oblicza gdzie należy narysować kwadraty oraz określa kolory dla każdego z nich
void calculate(point2 leftUp, point2 rightDown, int level, std::vector<Box> &boxes);
//Funckja rysuje dywan 
void drawCarpet(std::vector<Box> boxes);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    // Wczytanie ilości poziomów dywanu i stworzenie tytułu okna
    std::cout << "Podaj ilość poziomów: ";
    int level; std::cin >> level;
    std::string tytul = std::to_string(level); 
    tytul = tytul + "-poziomowy dywan Sierpińskiego";
    
    //Tablica zawierajaca informacje gdzie narysowac bok oraz jego kolor
    std::vector<Box> boxes;
    //Określenie rozmiaru dywanu
    point2 lu = {-0.9f, 0.9f};   // lewy górny róg dywanu
    point2 rd = { 0.9f,-0.9f};   // prawy dolny róg dywanu
    calculate(lu,rd,level,boxes);
    
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 980, tytul.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        // Rysujemy dywan Sierpinskiego
        drawCarpet(boxes);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void calculate(point2 lu, point2 rd, int level, std::vector<Box> &boxes){
    
    //Dlugosc boku 9 kwadratów stworzonych z pierwotnego
    float new_len =(float) std::abs(lu[0]-rd[0])/ 3.0f;
    
    //Wyznaczanie współrzednych lewo-góra, prawo-dół dla każdego z 9 kwadratów
    //przechodzimy kolejne wiersze
    for(int i = 0; i <3; i++){ //rows
        for (int j = 0; j < 3;j++){ //colums
            // przechodzimy kolejne wiersze
            point2 new_lu = { lu[0] + new_len*(float)j, lu[1] - new_len*(float)i};    
            point2 new_rd = { new_lu[0] + new_len, new_lu[1] - new_len};
            
            if(!(i==1 && j==1)){  // nie zapamietujemy środkowego kwadratu, bo nie chcemy go rysowac
                if( level == 1){  // Kwadraty do narysowania zapamietujemy dopiero na najnizszyszm
                                  // poziomie poniewaz i tak bysmy je "nadrysowali"(podzielili na 9)
                   
                    Box box;
                    // generacja 4x3 wartosci RGB (po 3 dla każdego boku)
                    for(int i = 0; i < 12; i++){
                        box.colors.push_back( (float) (rand() % 101) / 100.0f);
                    }

                    box.xy.push_back(new_lu[0]);
                    box.xy.push_back(new_lu[1]);
                    box.xy.push_back(new_rd[0]);
                    box.xy.push_back(new_rd[1]);
                  
                    boxes.push_back(box);

                }
                // Jeśli nie jesteśmy na środku(nadrzędny if) i nie jestesmy na ostatni poziomie
                // to przechodzimy dalej do podzielania kazdego z 9 kwadratów na 9 mniejszych
                if(level > 1)
                    
                  calculate(new_lu,new_rd,level-1,boxes);
                
            }
         } 
    }

}

void drawCarpet(std::vector<Box> boxes){
    
    for(int i = 0; i!= boxes.size(); i++){
    glBegin(GL_POLYGON); 
   
    //lewo doł
    glColor3f(boxes[i].colors[0],boxes[i].colors[1],boxes[i].colors[2]);
    glVertex2f(boxes[i].xy[0],boxes[i].xy[1]);
    //lewo góra
    point2 ld = {boxes[i].xy[0],boxes[i].xy[3]};
    glColor3f(boxes[i].colors[3],boxes[i].colors[4],boxes[i].colors[5]);
    glVertex2fv(ld);
    //prawo dół
    glColor3f(boxes[i].colors[6],boxes[i].colors[7],boxes[i].colors[8]);
    glVertex2f(boxes[i].xy[2],boxes[i].xy[3]);  
    //prawo góra
    point2 ru = {boxes[i].xy[2], boxes[i].xy[1]};
    glColor3f(boxes[i].colors[9],boxes[i].colors[10],boxes[i].colors[11]);
    glVertex2fv(ru);
    
    glEnd();
    }     
}



