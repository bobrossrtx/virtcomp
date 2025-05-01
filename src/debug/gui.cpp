// Minimal ImGui+GLFW+OpenGL3 integration for VirtComp VM debug GUI
#include "gui.hpp"
#include "../vhardware/cpu.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <string>

// ImGui backends
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// Helper: error callback for GLFW
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Gui::Gui(const std::string& title) : window_title(title) {}
void Gui::set_title(const std::string& title) { window_title = title; }

void Gui::run_vm(const std::vector<uint8_t>& program) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return;
    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(900, 600, window_title.c_str(), NULL, NULL);
    if (!window) { glfwTerminate(); return; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // VM state
    CPU cpu;
    cpu.reset();
    cpu.execute(program); // TODO: replace with step/run loop for real debugging

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin(window_title.c_str());
        // Registers
        ImGui::Text("Registers:");
        const auto& regs = cpu.get_registers();
        for (size_t i = 0; i < regs.size(); ++i) {
            ImGui::Text("R%zu: %u", i, regs[i]);
            ImGui::SameLine();
        }
        ImGui::NewLine();
        // Memory (first 32 bytes)
        ImGui::Text("Memory (first 32 bytes):");
        const auto& mem = cpu.get_memory();
        for (size_t i = 0; i < 32 && i < mem.size(); ++i) {
            ImGui::Text("[%02zu]=0x%02X", i, mem[i]);
            if ((i+1)%8==0) ImGui::NewLine();
            ImGui::SameLine();
        }
        ImGui::NewLine();
        // Flags, PC, SP, FP
        ImGui::Text("PC: %u  SP: %u  FP: %u  FLAGS: 0x%X", cpu.get_pc(), cpu.get_sp(), cpu.get_fp(), cpu.get_flags());
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
