#include <windows.h>
#include <commdlg.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace cv;

// Convierte una matriz cv::Mat en una textura OpenGL para mostrarla dentro de ImGui
GLuint MatATexturaOpenGL(const Mat& matOriginal) {
    if (matOriginal.empty()) return 0;

    Mat temp;
    // Normalizar si es 16 bits o float 
    if (matOriginal.depth() != CV_8U) {
        normalize(matOriginal, temp, 0, 255, NORM_MINMAX, CV_8U);
    }
    else {
        temp = matOriginal.clone();
    }

    // Convertir a RGBA para OpenGL
    if (temp.channels() == 1) {
        cvtColor(temp, temp, COLOR_GRAY2RGBA);
    }
    else if (temp.channels() == 3) {
        cvtColor(temp, temp, COLOR_BGR2RGBA);
    }
    else if (temp.channels() == 4) {
        cvtColor(temp, temp, COLOR_BGRA2RGBA);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp.cols, temp.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp.data);
    return textureID;
}

// Abre el cuadro de diálogo nativo de Windows para elegir archivo
string AbrirDialogoArchivo() {
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Imagenes (*.tif;*.tiff;*.png;*.jpg)\0*.tif;*.tiff;*.png;*.jpg\0Todos (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return string(ofn.lpstrFile);
    }
    return "";
}

// Analiza la imagen con OpenCV y devuelve el reporte para el panel de salida
string AnalizarImagen(const string& ruta, Mat& imagenOut) {
    stringstream ss;
    ss << "======================================\n";
    ss << "ANALISIS DE IMAGEN \n";
    ss << "======================================\n";
    ss << "OpenCV Version: " << CV_VERSION << "\n";
    ss << "Ruta: " << ruta << "\n\n";

    imagenOut = imread(ruta, IMREAD_UNCHANGED);
    if (imagenOut.empty()) {
        ss << "ERROR: No se pudo abrir la imagen.\n";
        return ss.str();
    }

    ss << "METADATOS:\n";
    ss << " - Resolucion: " << imagenOut.cols << " x " << imagenOut.rows << " px\n";
    ss << " - Canales: " << imagenOut.channels() << "\n";
    ss << " - Profundidad: ";
    switch (imagenOut.depth()) {
    case CV_8U:  ss << "8 bits unsigned (CV_8U)\n"; break;
    case CV_16U: ss << "16 bits unsigned (CV_16U)\n"; break;
    case CV_32F: ss << "32 bits float (CV_32F)\n"; break;
    default:     ss << "Codigo " << imagenOut.depth() << "\n"; break;
    }

    int x = imagenOut.cols / 2;
    int y = imagenOut.rows / 2;
    ss << "\nPIXEL CENTRAL (" << x << ", " << y << "):\n";

    if (imagenOut.channels() == 1) {
        if (imagenOut.depth() == CV_8U)
            ss << " - Valor: " << static_cast<int>(imagenOut.at<uchar>(y, x)) << "\n";
        else if (imagenOut.depth() == CV_16U)
            ss << " - Valor: " << imagenOut.at<unsigned short>(y, x) << "\n";
        else if (imagenOut.depth() == CV_32F)
            ss << " - Valor: " << imagenOut.at<float>(y, x) << "\n";

        double minVal, maxVal;
        minMaxLoc(imagenOut, &minVal, &maxVal);
        ss << "\nRANGO DE VALORES:\n";
        ss << " - Minimo: " << minVal << "\n";
        ss << " - Maximo: " << maxVal << "\n";
    }
    else if (imagenOut.channels() == 3 && imagenOut.depth() == CV_8U) {
        Vec3b px = imagenOut.at<Vec3b>(y, x);
        ss << " - Blue  = " << static_cast<int>(px[0]) << "\n";
        ss << " - Green = " << static_cast<int>(px[1]) << "\n";
        ss << " - Red   = " << static_cast<int>(px[2]) << "\n";
    }

    return ss.str();
}

// Inspector interactivo de pixeles
void InspeccionarPixel(const Mat& imagen)
{
    // Verificar que existe una imagen cargada
    if (imagen.empty())
        return;

    // Verificar que el mouse esta sobre la imagen
    if (!ImGui::IsItemHovered())
        return;

    // Obtener los limites de la imagen en pantalla
    ImVec2 inicio = ImGui::GetItemRectMin();
    ImVec2 fin = ImGui::GetItemRectMax();

    // Obtener la posicion del mouse
    ImVec2 mouse = ImGui::GetMousePos();

    // Dimensiones de la imagen mostrada
    float anchoVisor = fin.x - inicio.x;
    float altoVisor = fin.y - inicio.y;

    if (anchoVisor <= 0 || altoVisor <= 0)
        return;

    // Transformar coordenadas de pantalla
    // a coordenadas de la imagen original
    int x = static_cast<int>(
        (mouse.x - inicio.x) / anchoVisor * imagen.cols
        );

    int y = static_cast<int>(
        (mouse.y - inicio.y) / altoVisor * imagen.rows
        );

    // Evitar accesos fuera de los limites de la matriz
    if (x < 0 || x >= imagen.cols ||
        y < 0 || y >= imagen.rows)
        return;

    // Crear panel flotante de informacion
    if (ImGui::BeginTooltip())
    {
        ImGui::Text("INSPECTOR DE PIXELES");
        ImGui::Separator();

        ImGui::Text("X: %d", x);
        ImGui::Text("Y: %d", y);

        ImGui::Separator();

        // CASO 1: IMAGEN RGB DE 8 BITS

        if (imagen.type() == CV_8UC3)
        {
            Vec3b pixel = imagen.at<Vec3b>(y, x);

            int B = pixel[0];
            int G = pixel[1];
            int R = pixel[2];

            ImGui::Text("RGB");
            ImGui::Text("Red:   %d", R);
            ImGui::Text("Green: %d", G);
            ImGui::Text("Blue:  %d", B);

            // Mostrar visualmente el color del pixel
            ImGui::ColorButton(
                "##ColorPixel",
                ImVec4(
                    R / 255.0f,
                    G / 255.0f,
                    B / 255.0f,
                    1.0f
                ),
                ImGuiColorEditFlags_NoTooltip,
                ImVec2(80, 30)
            );

            // Crear una matriz de un unico pixel
            // en punto flotante, normalizada entre 0 y 1
            Mat pixelBGR(1, 1, CV_32FC3);

            pixelBGR.at<Vec3f>(0, 0) = Vec3f(
                B / 255.0f,
                G / 255.0f,
                R / 255.0f
            );

            // CONVERSION A HSV

            Mat pixelHSV;

            cvtColor(pixelBGR, pixelHSV, COLOR_BGR2HSV);

            Vec3f hsv = pixelHSV.at<Vec3f>(0, 0);

            float H = hsv[0];
            float S = hsv[1];
            float V = hsv[2];

            ImGui::Separator();
            ImGui::Text("HSV");
            ImGui::Text("H: %.2f grados", H);
            ImGui::Text("S: %.2f %%", S * 100.0f);
            ImGui::Text("V: %.2f %%", V * 100.0f);

            // CONVERSION A CIELAB

            Mat pixelLab;

            cvtColor(pixelBGR, pixelLab, COLOR_BGR2Lab);

            Vec3f lab = pixelLab.at<Vec3f>(0, 0);

            ImGui::Separator();
            ImGui::Text("CIELAB");
            ImGui::Text("L*: %.2f", lab[0]);
            ImGui::Text("a*: %.2f", lab[1]);
            ImGui::Text("b*: %.2f", lab[2]);

            // INDICE EXCESS GREEN

            int ExG = 2 * G - R - B;

            float suma = static_cast<float>(R + G + B);

            ImGui::Separator();
            ImGui::Text("INDICES RGB");
            ImGui::Text("ExG: %d", ExG);

            if (suma > 0.0f)
            {
                float ExGNormalizado = ExG / suma;

                ImGui::Text(
                    "ExG normalizado: %.4f",
                    ExGNormalizado
                );
            }
            else
            {
                ImGui::Text("ExG normalizado: indefinido");
            }
        }

        // CASO 2: IMAGEN MONOCANAL

        else if (imagen.channels() == 1)
        {
            ImGui::Text("Banda monocanal");

            if (imagen.depth() == CV_8U)
            {
                ImGui::Text(
                    "Valor: %d",
                    static_cast<int>(imagen.at<uchar>(y, x))
                );
            }
            else if (imagen.depth() == CV_16U)
            {
                ImGui::Text(
                    "Valor: %u",
                    static_cast<unsigned int>(
                        imagen.at<unsigned short>(y, x)
                        )
                );
            }
            else if (imagen.depth() == CV_32F)
            {
                ImGui::Text(
                    "Valor: %.6f",
                    imagen.at<float>(y, x)
                );
            }
            else
            {
                ImGui::Text("Profundidad no implementada");
            }
        }
        else
        {
            ImGui::Text("Formato no implementado");
        }

        ImGui::EndTooltip();
    }
}

int main() {
    // Inicializar ventana GLFW
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1200, 720, "Analisis de Cobertura de Suelo - INTA", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync

    // Inicializar contexto de Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Variables de estado de la aplicación
    string logOutput = "Listo. Presiona 'Cargar Imagen' para comenzar...";
    Mat imagenActual;
    GLuint texturaImagen = 0;
    float umbralEjemplo = 0.25f;

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Configurar ventana principal de ImGui ocupando toda la pantalla
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Panel Principal", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // --- COLUMNA IZQUIERDA -> CONTROLES Y OUTPUT ---
        ImGui::BeginChild("PanelControles", ImVec2(450, 0), true);

        if (ImGui::Button("Cargar Imagen (.tif / .png / .jpg)", ImVec2(-1, 40))) {
            string ruta = AbrirDialogoArchivo();
            if (!ruta.empty()) {
                logOutput = AnalizarImagen(ruta, imagenActual);
                if (texturaImagen != 0) glDeleteTextures(1, &texturaImagen);
                texturaImagen = MatATexturaOpenGL(imagenActual);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Parametros de Prueba (Proximo: NDVI):");
        ImGui::SliderFloat("Umbral Suelo/Veg", &umbralEjemplo, -1.0f, 1.0f, "%.2f");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Salida / Log de Diagnostico:");
        ImGui::BeginChild("CajaLog", ImVec2(0, 0), true);
        ImGui::TextWrapped("%s", logOutput.c_str());
        ImGui::EndChild();

        ImGui::EndChild();

        ImGui::SameLine();

        // --- COLUMNA DERECHA -> VISOR DE IMAGEN ---
        ImGui::BeginChild("PanelVisor", ImVec2(0, 0), true);
        if (texturaImagen != 0 && !imagenActual.empty()) {
            ImVec2 areaDisponible = ImGui::GetContentRegionAvail();
            float escalaX = areaDisponible.x / imagenActual.cols;
            float escalaY = areaDisponible.y / imagenActual.rows;
            float escala = min(escalaX, escalaY);

            ImVec2 tamanoFinal(imagenActual.cols * escala, imagenActual.rows * escala);
            ImGui::Image((ImTextureID)(intptr_t)texturaImagen, tamanoFinal);

            InspeccionarPixel(imagenActual);
        }
        else {
            ImGui::Text("Ninguna imagen cargada en el visor.");
        }
        ImGui::EndChild();

        ImGui::End();

        // Renderizar cuadro en pantalla
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Limpieza al cerrar
    if (texturaImagen != 0) glDeleteTextures(1, &texturaImagen);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}