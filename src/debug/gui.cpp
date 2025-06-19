// Minimal ImGui+GLFW+OpenGL3 integration for VirtComp VM debug GUI
#include "gui.hpp"
#include "../config.hpp"
#include "../debug/logger.hpp"
#include "../vhardware/cpu.hpp"

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>
#include <set>

using Logging::Logger;

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
    // Create a fullscreen window on the primary monitor
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    // Create a window with fullscreen dimensions, but windowed mode (not borderless/fullscreen)
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, window_title.c_str(), nullptr, nullptr);
    if (window) {
        // Center the window on the primary monitor
        int xpos = mode->width  > 0 ? 0 : 100;
        int ypos = mode->height > 0 ? 0 : 100;
        glfwSetWindowPos(window, xpos+2, ypos+31);
    }
    if (!window) { glfwTerminate(); return; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Enable docking before the first NewFrame call
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Optional: Enable multi-viewports if needed
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Set up fonts, styles, etc.
    ImFont* smallFont = nullptr;
    ImFont* defaultFont = nullptr;
    ImFont* mediumFont = nullptr;
    ImFont* largeFont = nullptr;
    ImFont* smallFontBold = nullptr;
    ImFont* mediumFontBold = nullptr;
    ImFont* defaultFontBold = nullptr;
    ImFont* largeFontBold = nullptr;
    ImFont* smallFontLight = nullptr;
    ImFont* mediumFontLight = nullptr;
    ImFont* defaultFontLight = nullptr;
    ImFont* largeFontLight = nullptr;
    ImFont* smallFontSemiBold = nullptr;
    ImFont* mediumFontSemiBold = nullptr;
    ImFont* defaultFontSemiBold = nullptr;
    ImFont* largeFontSemiBold = nullptr;

    // FiraCode font paths
    const char* font_regular   = "fonts/FiraCode-Regular.ttf";
    const char* font_bold      = "fonts/FiraCode-Bold.ttf";
    const char* font_light     = "fonts/FiraCode-Light.ttf";
    const char* font_semibold  = "fonts/FiraCode-SemiBold.ttf";

    // Only load if all weights exist
    bool has_regular  = std::filesystem::exists(font_regular);
    bool has_bold     = std::filesystem::exists(font_bold);
    bool has_light    = std::filesystem::exists(font_light);
    bool has_semibold = std::filesystem::exists(font_semibold);

    // Add extended ASCII range to the font atlas
    ImFontConfig config;
    static const ImWchar extended_ascii_range[] = { 0x20, 0xFF, 0 };
    config.GlyphRanges = extended_ascii_range;

    if (has_regular) {
        smallFont   = io.Fonts->AddFontFromFileTTF(font_regular, 10.0f, &config);
        mediumFont  = io.Fonts->AddFontFromFileTTF(font_regular, 12.0f, &config);
        defaultFont = io.Fonts->AddFontFromFileTTF(font_regular, 16.0f, &config);
        largeFont   = io.Fonts->AddFontFromFileTTF(font_regular, 20.0f, &config);
    }
    if (has_bold) {
        smallFontBold   = io.Fonts->AddFontFromFileTTF(font_bold, 10.0f);
        mediumFontBold  = io.Fonts->AddFontFromFileTTF(font_bold, 12.0f);
        defaultFontBold = io.Fonts->AddFontFromFileTTF(font_bold, 16.0f);
        largeFontBold   = io.Fonts->AddFontFromFileTTF(font_bold, 20.0f);
    }
    if (has_light) {
        smallFontLight   = io.Fonts->AddFontFromFileTTF(font_light, 10.0f);
        mediumFontLight  = io.Fonts->AddFontFromFileTTF(font_light, 12.0f);
        defaultFontLight = io.Fonts->AddFontFromFileTTF(font_light, 16.0f);
        largeFontLight   = io.Fonts->AddFontFromFileTTF(font_light, 20.0f);
    }
    if (has_semibold) {
        smallFontSemiBold   = io.Fonts->AddFontFromFileTTF(font_semibold, 10.0f);
        mediumFontSemiBold  = io.Fonts->AddFontFromFileTTF(font_semibold, 12.0f);
        defaultFontSemiBold = io.Fonts->AddFontFromFileTTF(font_semibold, 16.0f);
        largeFontSemiBold   = io.Fonts->AddFontFromFileTTF(font_semibold, 20.0f);
    }

    // Fallbacks
    if (!smallFont)   smallFont = io.FontDefault;
    if (!mediumFont)  mediumFont = io.FontDefault;
    if (!defaultFont) defaultFont = io.FontDefault;
    if (!largeFont)   largeFont = io.FontDefault;
    if (!smallFontBold)   smallFontBold = smallFont;
    if (!mediumFontBold)  mediumFontBold = mediumFont;
    if (!defaultFontBold) defaultFontBold = defaultFont;
    if (!largeFontBold)   largeFontBold = largeFont;
    if (!smallFontLight)   smallFontLight = smallFont;
    if (!mediumFontLight)  mediumFontLight = mediumFont;
    if (!defaultFontLight) defaultFontLight = defaultFont;
    if (!largeFontLight)   largeFontLight = largeFont;
    if (!smallFontSemiBold)   smallFontSemiBold = smallFont;
    if (!mediumFontSemiBold)  mediumFontSemiBold = mediumFont;
    if (!defaultFontSemiBold) defaultFontSemiBold = defaultFont;
    if (!largeFontSemiBold)   largeFontSemiBold = largeFont;

    // Use the default font for the GUI
    io.FontDefault = defaultFont;
    io.Fonts->Build();

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(8, 4);
    style.CellPadding = ImVec2(4, 2); // Reduce cell padding for tables
    style.FrameBorderSize = 0.5f;    // Thinner frame borders
    style.WindowBorderSize = 0.5f;   // Thinner window borders
    style.TabBorderSize = 0.5f;      // Thinner tab borders
    style.ScrollbarSize = 10.0f;     // Slightly thinner scrollbars
    style.ScrollbarRounding = 4.0f;

    // Use a lighter dark color scheme for better visibility
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg]        = ImVec4(0.13f, 0.15f, 0.18f, 1.00f); // Editor background
    colors[ImGuiCol_TitleBg]         = ImVec4(0.10f, 0.12f, 0.15f, 1.00f); // Title bar background
    colors[ImGuiCol_TitleBgActive]   = ImVec4(0.13f, 0.15f, 0.18f, 1.00f); // Active title bar
    colors[ImGuiCol_TitleBgCollapsed]= ImVec4(0.10f, 0.12f, 0.15f, 1.00f); // Collapsed title bar
    colors[ImGuiCol_MenuBarBg]       = ImVec4(0.16f, 0.18f, 0.22f, 1.00f); // Menu bar background
    colors[ImGuiCol_FrameBg]         = ImVec4(0.18f, 0.20f, 0.23f, 1.00f); // Frame background
    colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.22f, 0.24f, 0.29f, 1.00f); // Hovered frame background
    colors[ImGuiCol_FrameBgActive]   = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // Active frame background
    colors[ImGuiCol_Button]          = ImVec4(0.18f, 0.20f, 0.23f, 1.00f); // Button background
    colors[ImGuiCol_ButtonHovered]   = ImVec4(0.22f, 0.24f, 0.29f, 1.00f); // Hovered button background
    colors[ImGuiCol_ButtonActive]    = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // Active button background
    colors[ImGuiCol_Header]          = ImVec4(0.20f, 0.22f, 0.27f, 0.65f); // Header background
    colors[ImGuiCol_HeaderHovered]   = ImVec4(0.30f, 0.34f, 0.42f, 0.80f); // Hovered header background
    colors[ImGuiCol_HeaderActive]    = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // Active header background
    colors[ImGuiCol_Separator]       = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); // Separator
    colors[ImGuiCol_SeparatorHovered]= ImVec4(0.30f, 0.60f, 1.00f, 0.78f); // Hovered separator
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.30f, 0.60f, 1.00f, 1.00f); // Active separator
    colors[ImGuiCol_Text]            = ImVec4(0.90f, 0.92f, 0.95f, 1.00f); // Text color
    colors[ImGuiCol_TextDisabled]    = ImVec4(0.50f, 0.54f, 0.60f, 1.00f); // Disabled text color
    colors[ImGuiCol_TextSelectedBg]  = ImVec4(0.30f, 0.60f, 1.00f, 0.35f); // Selected text background

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // VM state
    CPU cpu;
    bool cpu_initialized = false; // <-- Add this flag
    cpu.reset();
    std::copy(program.begin(), program.end(), cpu.get_memory().begin());
    cpu.set_pc(0);
    cpu.set_sp(cpu.get_memory().size() - 4);
    cpu.set_fp(cpu.get_sp());
    cpu_initialized = true;
    static int mem_offset = 0; // <-- Move this here, before any use!
    mem_offset = 0; // Show start of memory
    // State for GUI control
    bool paused = false;

    // Breakpoints set by the user
    static std::set<int> breakpoints;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // // Enable docking and add a dockspace to the main viewport
        // ImGuiIO& io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // ImGuiViewport* viewport = ImGui::GetMainViewport();
        // ImGui::SetNextWindowPos(viewport->WorkPos);
        // ImGui::SetNextWindowSize(viewport->WorkSize);
        // ImGui::SetNextWindowViewport(viewport->ID);
        // ImGuiWindowFlags host_window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        //                                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        // ImGui::Begin("DockSpaceHost", nullptr, host_window_flags);
        // ImGui::PopStyleVar(2);
        // ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        // ImGui::End();

        // Show the Metrics window (now dockable, moveable, resizeable, collapseable, solid black background)
        // Dock Metrics/Debugger window into the main dockspace
        ImGui::SetNextWindowFocus();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f)); // Solid dark background
        ImGui::ShowMetricsWindow();
        ImGui::PopStyleColor();

        // // Debugging: Show ImGui Font Atlas
        // if (ImGui::Begin("Font Debugger")) {
        //     ImGui::Text("Inspecting Font Atlas:");
        //     ImGui::ShowFontAtlas(io.Fonts);
        //     ImGui::End();
        // }

        // Registers {
            // Set fixed position and size: 600x400 at top-left corner
            ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(735, 300), ImGuiCond_Always);
            ImGui::Begin(window_title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);


            if (largeFont) {
                ImGui::PushFont(largeFont);
                ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "VirtComp Debugger");
                ImGui::PopFont(); // Pop largeFont
            }

            ImGui::Separator();

            if (defaultFont) {
                ImGui::PushFont(defaultFont);
                ImGui::Text("Registers:");
                ImGui::PopFont(); // Pop mediumFont
            }

            ImGui::BeginTable("registers", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
            const auto& regs = cpu.get_registers();
            for (size_t i = 0; i < regs.size(); ++i) {
                ImGui::TableNextColumn();
                ImGui::Text("R%zu: %u", i, regs[i]);
            }
            ImGui::EndTable();

            ImGui::Spacing();

            // Memory (first 32 bytes)
            if (defaultFont) {
                ImGui::PushFont(defaultFont);
                ImGui::Text("Memory:");
                ImGui::PopFont(); // Pop mediumFont
            }
            ImVec2 table_size(700, 100); // Fixed height for scrollable area

            // Navigation controls above the table
            static int mem_offset = 0;
            auto& mem = cpu.get_memory();
            uint32_t last_accessed = cpu.get_last_accessed_addr();
            uint32_t last_modified = cpu.get_last_modified_addr();
            const int bytes_per_page = 4 * 7; // 4 rows, 7 columns

            if (ImGui::Button("Prev") && mem_offset - bytes_per_page >= 0) {
                mem_offset -= bytes_per_page;
            }
            ImGui::SameLine();
            if (ImGui::Button("Next") && mem_offset + bytes_per_page < static_cast<int>(mem.size())) {
                mem_offset += bytes_per_page;
            }
            ImGui::SameLine();
            ImGui::Text("Offset: %d", mem_offset);

            // Now the table, with fixed column width
            // Static variables for memory edit popup
            static int edit_idx = -1;
            static int edit_value = 0;
            static bool edit_popup_opened = false;

            if (ImGui::BeginTable("memory", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, table_size)) {
                for (int row = 0; row < 4; ++row) {
                    ImGui::TableNextRow();
                    for (int col = 0; col < 7; ++col) {
                        ImGui::TableSetColumnIndex(col);
                        int idx = mem_offset + row * 7 + col;
                        if (idx < static_cast<int>(mem.size())) {
                            ImGui::PushID(idx);
                            bool is_breakpoint = breakpoints.count(idx) > 0;

                            // Draw the background circle for the breakpoint indicator
                            ImVec2 p = ImGui::GetCursorScreenPos();
                            float radius = 4.0f;
                            ImU32 color = is_breakpoint ? IM_COL32(255, 50, 50, 255) : IM_COL32(90, 0, 0, 255);
                            ImDrawList* draw_list = ImGui::GetWindowDrawList();
                            draw_list->AddCircleFilled(ImVec2(p.x + radius, p.y + radius + 2), radius, color);

                            // Make the whole cell an invisible button
                            ImVec2 cell_size = ImGui::GetContentRegionAvail();
                            cell_size.x = 76.0f; // match your column width
                            cell_size.y = ImGui::GetTextLineHeightWithSpacing();
                            if (ImGui::InvisibleButton("cell", cell_size)) {
                                if (is_breakpoint)
                                    breakpoints.erase(idx);
                                else
                                    breakpoints.insert(idx);
                            }

                            // Draw the memory value, offset a bit to the right of the dot
                            ImGui::SetCursorScreenPos(ImVec2(p.x + radius * 2 + 6, p.y));
                            if (static_cast<uint32_t>(idx) == cpu.get_pc()) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(180, 140, 0, 220));
                                ImGui::TextColored(ImVec4(0, 0, 0, 1), "[0x%02X]=%02X", idx, mem[idx]);
                            } else if (static_cast<uint32_t>(idx) == last_accessed) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 120, 0, 220));
                                ImGui::TextColored(ImVec4(0, 0, 0, 1), "[0x%02X]=%02X", idx, mem[idx]);
                            } else if (static_cast<uint32_t>(idx) == last_modified) {
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 120, 120, 220));
                                ImGui::TextColored(ImVec4(0, 0, 0, 1), "[0x%02X]=%02X", idx, mem[idx]);
                            } else {
                                ImGui::Text("[0x%02X]=%02X", idx, mem[idx]);
                            }
                            ImGui::PopID();

                            // Right-click to open memory edit popup (set static vars, open only for this cell)
                            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                                edit_idx = idx;
                                edit_value = mem[idx];
                                edit_popup_opened = true;
                            }
                        } else {
                            ImGui::Text("-");
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Show memory edit popup (outside table loop, only if requested)
            if (edit_popup_opened) {
                ImGui::OpenPopup("EditMem");
                edit_popup_opened = false;
            }
            if (ImGui::BeginPopup("EditMem")) {
                ImGui::Text("Edit memory [0x%02X]", edit_idx);
                ImGui::SetNextItemWidth(100);
                if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
                if (ImGui::InputInt("Value (0-255)", &edit_value, 1, 10)) {
                    if (edit_value < 0) edit_value = 0;
                    if (edit_value > 255) edit_value = 255;
                }
                if (ImGui::Button("OK")) {
                    if (edit_idx >= 0 && edit_idx < (int)mem.size()) {
                        mem[edit_idx] = static_cast<uint8_t>(edit_value);
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::Spacing();

            // Flags, PC, SP, FP
            ImGui::Text("PC: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "%u", cpu.get_pc());
            ImGui::SameLine();
            ImGui::Text("SP: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "%u", cpu.get_sp());
            ImGui::SameLine();
            ImGui::Text("FP: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "%u", cpu.get_fp());
            ImGui::SameLine();
            ImGui::Text("FLAGS: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,0,1), "0x%X", cpu.get_flags());
            ImGui::Separator();

            ImGui::End();
        // }

        // Log window {
            // Place the log window below the main window, matching its width
            ImGui::Begin("Debug Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
            ImGui::SetWindowPos(ImVec2(20, 280), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(700, 300), ImGuiCond_Always);
            ImGui::Text("Log output:");
            // Hide the scrollbar but keep scrolling enabled by not setting ImGuiWindowFlags_HorizontalScrollbar and customizing style
            ImGuiStyle& style = ImGui::GetStyle();
            float oldScrollbarSize = style.ScrollbarSize;
            style.ScrollbarSize = 0.0f;
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0)); // Make log output background invisible
            ImGui::BeginChild("LogOutput", ImVec2(0, 0), true, 0);
            std::vector<std::string> log_lines = Logger::instance().get_gui_log_buffer();
            // Add padding lines at the end for visual spacing
            constexpr int log_padding_lines = 2;
            for (int i = 0; i < log_padding_lines; ++i) {
                log_lines.emplace_back("");
            }
            // Join all log lines into a single string
            std::string log_text;
            size_t total_len = 0;
            for (const auto& line : log_lines) total_len += line.size() + 1;
            log_text.reserve(total_len);
            for (size_t i = 0; i < log_lines.size(); ++i) {
                log_text += log_lines[i];
                if (i + 1 < log_lines.size()) log_text += '\n';
            }
            if (mediumFont) ImGui::PushFont(mediumFont);
            // Use InputTextMultiline for selectable/copyable, borderless, read-only log
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_NoUndoRedo;
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            static int last_log_size = 0;
            static bool smooth_scroll = false;
            bool force_scroll = false;
            if ((int)log_lines.size() > last_log_size || smooth_scroll) {
                force_scroll = true;
                last_log_size = (int)log_lines.size();
                smooth_scroll = false;
            }
            ImGui::InputTextMultiline("##logblock", (char*)log_text.c_str(), log_text.size() + 1, ImVec2(-1, -1), flags);
            if (force_scroll) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::PopStyleVar(2);
            if (mediumFont) ImGui::PopFont();
            ImGui::EndChild();
            ImGui::PopStyleColor();
            style.ScrollbarSize = oldScrollbarSize;
            if (ImGui::Button("Clear Log")) {
                Logger::instance().clear_gui_log_buffer();
            }
            ImGui::End();
        // }

        // Controller {
            ImGui::SetNextWindowPos(ImVec2(740, 10), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(620, 200), ImGuiCond_Always);
            ImGui::Begin("Controller", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
            ImGui::Text("Control the VM:");
            // Add [File, 2 more columns]
            ImGui::Separator();
            // Define button size
            ImVec2 button_size(80, 30);

            // Begin 2-column layout
            ImGui::Columns(5, nullptr, false);
            // Set column widths
            ImGui::SetColumnOffset(1, 120); // Adjust this value as needed (smaller = closer columns)

            // First column: main controls
            bool do_step = false, do_run = false, do_reset = false;
            if (paused) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Step", button_size)) {
                do_step = true;
            }
            if (ImGui::Button("Run", button_size)) {
                do_run = true;
            }
            if (ImGui::Button("Reset", button_size)) {
                do_reset = true;
            }
            if (ImGui::Button("Exit", button_size)) {
                glfwSetWindowShouldClose(window, true);
            }
            if (paused) {
                ImGui::EndDisabled();
            }

            ImGui::NextColumn();

            // Second column: additional buttons (example placeholders)
            if (ImGui::Button(paused ? "Unpause" : "Pause", button_size)) {
                paused = !paused;
            }
            if (paused) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Break", button_size)) {
                // TODO: implement break functionality
            }
            static bool continue_until_call = false;
            if (ImGui::Button("Continue", button_size) && !paused) {
                continue_until_call = true;
                smooth_scroll = true; // Enable smooth scroll for log output
            }
            if (ImGui::Button("Reload", button_size)) {
                // TODO: implement reload functionality
                // resets the program, clears registers & etc, but preserves memory so you can set breakpoints
            }
            if (paused) {
                ImGui::EndDisabled();
            }

            ImGui::NextColumn();

            // File column
            if (ImGui::Button("Open", button_size)) {
                // TODO: implement Open functionality
            }

            if (ImGui::Button("Open>Run", button_size)) {
                // TODO: implement Open functionality
            }

            // Handle actions only if not paused
            if (!paused) {
                if (do_reset) {
                    cpu.reset();
                    std::copy(program.begin(), program.end(), cpu.get_memory().begin());
                    cpu.set_pc(0);
                    cpu.set_sp(cpu.get_memory().size() - 4);
                    cpu.set_fp(cpu.get_sp());
                    cpu_initialized = true;
                    Logger::instance().clear_gui_log_buffer();
                    mem_offset = 0; // Reset memory offset
                }
                if (do_run) {
                    if (!cpu_initialized) {
                        cpu.reset();
                        std::copy(program.begin(), program.end(), cpu.get_memory().begin());
                        cpu.set_pc(0);
                        cpu.set_sp(cpu.get_memory().size() - 4);
                        cpu.set_fp(cpu.get_sp());
                        cpu_initialized = true;
                    }
                    cpu.run(program);
                }
                if (do_step) {
                    if (!cpu_initialized) {
                        cpu.reset();
                        std::copy(program.begin(), program.end(), cpu.get_memory().begin());
                        cpu.set_pc(0);
                        cpu.set_sp(cpu.get_memory().size() - 4);
                        cpu.set_fp(cpu.get_sp());
                        cpu_initialized = true;
                    }
                    cpu.step(program);

                    // Auto-scroll memory panel to show current PC
                    const int bytes_per_page = 4 * 7;
                    const int mem_size = static_cast<int>(cpu.get_memory().size());
                    int pc = static_cast<int>(cpu.get_pc());
                    if (pc < mem_offset || pc >= mem_offset + bytes_per_page) {
                        mem_offset = (pc / bytes_per_page) * bytes_per_page;
                        if (mem_offset < 0) mem_offset = 0;
                        if (mem_offset > mem_size - bytes_per_page) mem_offset = std::max(0, mem_size - bytes_per_page);
                    }
                }
                // Continue until next CALL
                if (continue_until_call) {
                    while (true) {
                        uint32_t old_pc = cpu.get_pc();
                        bool can_continue = cpu.step(program);
                        // Check if the last executed instruction was CALL
                        if (!can_continue) {
                            continue_until_call = false;
                            break;
                        }
                        if (old_pc < program.size() && static_cast<Opcode>(program[old_pc]) == Opcode::CALL) {
                            continue_until_call = false;
                            break;
                        }
                    }
                }
            }

            ImGui::Columns(1);
            ImGui::End();
        // }

        // Hex Editor for Program File
        // Set fixed position and size: 600x400 at top-left corner
        ImGui::SetNextWindowPos(ImVec2(740, 190), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(620, 455), ImGuiCond_Always);
        ImGui::Begin("Program Hex Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
        ImGui::Text("Hex Editor:");
        ImGui::Separator();

        static int hex_cols = 16;
        static int hex_rows = 16;
        static std::vector<uint8_t> edited_program;
        // Always show 16x10 grid, pad with 0 if needed, but keep edits persistent
        if (edited_program.empty()) {
            edited_program = program;
            edited_program.resize(hex_cols * hex_rows, 0);
        } else if (edited_program.size() < static_cast<size_t>(hex_cols * hex_rows)) {
            edited_program.resize(hex_cols * hex_rows, 0);
        } else if (edited_program.size() > static_cast<size_t>(hex_cols * hex_rows)) {
            edited_program.resize(hex_cols * hex_rows);
        }
        int num_rows = hex_rows;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.30f, 0.36f, 0.45f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.45f, 0.60f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.45f, 0.50f, 0.60f, 1.0f));
        ImGui::PushItemWidth(24.0f);

        // Use small font for hex editor if available
        if (mediumFont) ImGui::PushFont(mediumFont);

        // Header: Offset | 00 01 02 ... 0F | ASCII
        float offset_col_width = 33.0f; // Width for the offset column
        float ascii_col_spacing = 20.0f;

        // Set X position for header to align with hex columns
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_col_width);
        ImGui::TextColored(ImVec4(0.7f,0.8f,1.0f,1.0f), "00");

        // Print hex column headers
        for (int col = 1; col < hex_cols; ++col) {
            ImGui::SameLine(0, col == 0 ? 0.0f : 16.0f);
            ImGui::TextColored(ImVec4(0.7f,0.8f,1.0f,1.0f), "%02X", col);
        }

        // ASCII header
        ImGui::SameLine(0, ascii_col_spacing + 5.0f);
        ImGui::TextColored(ImVec4(0.7f,0.8f,1.0f,1.0f), "ASCII");
        ImGui::Separator();

        for (int row = 0; row < num_rows; ++row) {
            // Offset column
            ImGui::TextColored(ImVec4(0.5f, 0.7f, 1.0f, 1.0f), "%04X:", row * hex_cols);
            ImGui::SameLine();
            // Hex columns
            for (int col = 0; col < hex_cols; ++col) {
            int idx = row * hex_cols + col;
            ImGui::BeginGroup();
            if (idx < (int)edited_program.size()) {
                ImGui::PushID(idx);

                char buf[4];
                snprintf(buf, sizeof(buf), "%02X", edited_program[idx]);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.98f, 1.0f));
                // Center horizontally
                float cursor_x = ImGui::GetCursorPosX();
                ImGui::SetCursorPosX(cursor_x + 2.0f); // Adjusted for better alignment
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f); // Center vertically
                if (ImGui::InputText("", buf, sizeof(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_AutoSelectAll)) {
                unsigned int val = 0;
                if (sscanf(buf, "%x", &val) == 1 && val <= 0xFF) {
                    edited_program[idx] = static_cast<uint8_t>(val);
                }
                }
                ImGui::PopStyleColor();
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.0f); // Restore Y
                ImGui::PopID();
            } else {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2.0f);
                ImGui::TextDisabled("  ");
            }
            ImGui::EndGroup();
            if (col < hex_cols - 1) ImGui::SameLine();
            }
            // ASCII representation
            ImGui::SameLine(0, 16.0f);
            for (int col = 0; col < hex_cols; ++col) {
            int idx = row * hex_cols + col;
            if (idx < (int)edited_program.size()) {
                char c;
                uint8_t val = edited_program[idx];
                if (val >= 32 && val <= 126) { // Printable ASCII range
                    c = static_cast<char>(val);
                } else if (val >= 128) { // Extended ASCII range
                    c = static_cast<char>(val);
                } else {
                    c = '.';
                }
                ImGui::TextColored(ImVec4(0.85f, 0.90f, 1.0f, 1.0f), u8"%c", c);
            } else {
                ImGui::TextDisabled(" ");
            }
            if (col < hex_cols - 1) ImGui::SameLine();
            }
        }

        if (mediumFont) ImGui::PopFont();
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
        ImGui::Separator();

        if (ImGui::Button("Save Changes")) {
            // Save edited_program to file (implement file writing as needed)
            // Example: write_program_to_file(edited_program, "your_program_file.hex");
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.10f, 0.10f, 1.00f); // Set global window background to a plain dark gray
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
