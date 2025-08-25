#include "App.h"
#include "Storage.h"
#include "Flashcard.h"
#include "UI.h"
#include <vector>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

std::vector<Flashcard> flashcards;
std::vector<Topic> topics;

App::App(){
    Storage::LoadFlashcards("data/flashcards.json", flashcards);
    Storage::LoadTopics("data/topics.json", topics);
}
App::~App(){}

void App::Run(){
    if(!glfwInit())
        return;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Flashcards", NULL, NULL);
    if(!window){
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //VSync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    UI ui;

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui.Render(flashcards, topics);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::OnExit(){
    Storage::SaveFlashcards("data/flashcards.json", flashcards);
    Storage::SaveTopics("data/topics.json", topics);
}