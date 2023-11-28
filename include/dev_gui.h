#ifndef DEV_GUI_H
#define DEV_GUI_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

//#include <camera.h>
#include <input.h>
#include <nfd/nfd.h>
//#include <scene_graph.h>
#include <my_math.h>

#include "gltf/gltf.h"

bool animationPlaying = false;
double previousTimeFPS;
int frameCount = 0;
int fps = 0;

int nodeId = 0;

glm::vec3 sliderColor = glm::vec3(1.0f, 1.0f, 1.0f);
float dayNightSpeed = 0.02f;

Camera* playerCamera;
//SceneNode* selectedNode;

bool polygonMode = false;
bool showCollisionData = false;

float animationSpeed = 0.0f;
float animationBlend = 0.0f;

float speedModifier = 1.0f;

Material selectedMaterial;
bool showTextureWindow = true;

glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, -7.0f);

static void ShowExampleAppSimpleOverlay(bool* p_open, int fps)
{
    static int location = 1;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0) {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    } else if (location == -2) {
        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", p_open, window_flags)) {
        ImGui::Text("Simple overlay\n"
                    "(right-click to change position)");
        ImGui::Separator();
        // FPS
        ImVec4 textColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        ImGui::TextColored(textColor, "FPS %d", fps);
        // **
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Custom", NULL, location == -1))
                location = -1;
            if (ImGui::MenuItem("Center", NULL, location == -2))
                location = -2;
            if (ImGui::MenuItem("Top-left", NULL, location == 0))
                location = 0;
            if (ImGui::MenuItem("Top-right", NULL, location == 1))
                location = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, location == 2))
                location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3))
                location = 3;
            if (p_open && ImGui::MenuItem("Close"))
                *p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void CollisionData()
{
    ImGui::Begin("Collision");

    ImGui::Text("Player Position: %f  %f  %f", playerPosition.x, playerPosition.y, playerPosition.z);

    ImGui::CollapsingHeader("Polygons");
    ImGui::BeginChild("Scrolling");

    for (size_t i = 0; i < potentialColliders.size(); ++i) {
        ImGui::Text("Face: %d", i + 1);
        ImGui::Text("    Vertices: ");

        for (int j = 0; j < 3; ++j) {
            glm::vec3 vertex = potentialColliders[i].vertices[j];
            ImGui::Text("       {%.2f,%.2f,%.2f} ", vertex.x, vertex.y, vertex.z);
        }

        ImGui::Text("\n    Normal: %.2f %.2f %.2f\n", potentialColliders[i].normal.x, potentialColliders[i].normal.y, potentialColliders[i].normal.z);
    }
    ImGui::EndChild();

    ImGui::End();
}

void TextureWindow()
{
    ImGui::Begin("Textures");

    ImVec2 windowSize = ImGui::GetWindowSize();

    // Assuming you want the image to be square, you can use the smaller dimension as both width and height
    float imageSize = std::min(windowSize.x, windowSize.y);
    imageSize -= 50;

    if (ImGui::BeginTabBar("TextureTypes", 0)) {
        if (ImGui::BeginTabItem("BaseColor Texture")) {
            ImGui::Image((ImTextureID)selectedMaterial.m_BaseColorTextureId, ImVec2(imageSize, imageSize));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Normal Texture")) {
            ImGui::Image((void*)(intptr_t)selectedMaterial.m_NormalTextureId, ImVec2(imageSize, imageSize));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Metallic Texture")) {
            ImGui::Image((void*)(intptr_t)selectedMaterial.m_MetallicTextureId, ImVec2(imageSize, imageSize));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Roughness Texture")) {
            ImGui::Image((void*)(intptr_t)selectedMaterial.m_RoughnessTextureId, ImVec2(imageSize, imageSize));
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Occlusion Texture")) {
            ImGui::Image((void*)(intptr_t)selectedMaterial.m_OcclusionTextureId, ImVec2(imageSize, imageSize));
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}



void MainMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                nfdchar_t* outPath = NULL;
                nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

                if (result == NFD_OKAY) {

                    puts("Success!");
                    puts(outPath);

                    int length = strlen(outPath);

                    for (int i = 0; i < length; i++) {
                        if (outPath[i] == '\\') {
                            outPath[i] = '/'; // Replace backslash with forward slash
                        }
                    }

                    //createNode(root_node, outPath);

                    free(outPath);
                } else if (result == NFD_CANCEL) {
                    puts("User pressed cancel.");
                } else {
                    printf("Error: %s\n", NFD_GetError());
                }
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                nfdchar_t* outPath = NULL;
                nfdresult_t result = NFD_SaveDialog(".json", NULL, &outPath);

                if (result == NFD_OKAY) {
                    // The user selected a file and clicked "Save"
                    // 'outPath' contains the selected file path
                    printf("Selected file: %s\n", outPath);
                    // You can free the 'outPath' memory when you're done with it
                    free(outPath);
                } else if (result == NFD_CANCEL) {
                    // The user canceled the dialog
                    printf("Dialog canceled by the user.\n");
                } else {
                    // Handle other error cases
                    printf("Error: %s\n", NFD_GetError());
                }
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                // Handle the "Exit" action
                // Example: glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Open")) {
            }
            if (ImGui::MenuItem("Save")) {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Model")) {
            if (ImGui::MenuItem("Open")) {
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera")) {

            //default
            static int e = 1;

            ImGui::Text("Camera Type");
            ImGui::RadioButton("Free", &e, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Third", &e, 1);
            ImGui::SameLine();
            ImGui::RadioButton("First", &e, 2);

            playerCamera->Type = Camera_Type(e);

            ImGui::Separator();

            ImGui::Text("FOV");
            ImGui::SliderFloat("##fovslider", &playerCamera->FOV, 50, 150, "%.f");

            ImGui::Text("Zoom");
            ImGui::SliderFloat("##zoomslider", &playerCamera->Zoom, 10, 150, "%.f");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Lighting")) {

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Day");
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
            ImGui::Button("Night");
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            ImGui::Text("Day/Time Speed");
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            ImGui::SliderFloat("##timespeed", &dayNightSpeed, 0.0f, 1.0f, "%.2f");
            ImGui::PopStyleColor();

            ImGui::Separator();

            ImGui::Text("Color");
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::SliderFloat("##redslider", &sliderColor.x, 0.0f, 1.0f, "%.2f");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ImGui::SliderFloat("##blueslide", &sliderColor.y, 0.0f, 1.0f, "%.2f");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
            ImGui::SliderFloat("##greenslider", &sliderColor.z, 0.0f, 1.0f, "%.2f");
            ImGui::PopStyleColor();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Collision")) {

            /*
            ImGui::Checkbox("Show Hitboxes", &drawHitboxes);
            ImGui::Separator();*/

            ImGui::Checkbox("glPolygonMode", &polygonMode);
            ImGui::Separator();

            if (ImGui::MenuItem(" Collision Data Window")) {
                showCollisionData = !showCollisionData;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Movement")) {

            ImGui::Checkbox("NoClip", &noClip);
            ImGui::Separator();

            ImGui::Checkbox("animationPlaying", &animationPlaying);
            ImGui::Separator();

            ImGui::Text("Animation Speed");
            ImGui::SliderFloat("##animationspeedslider", &animationSpeed, 0.0f, 0.1f, "%.03f");

            ImGui::Text("Animation Blend");
            ImGui::SliderFloat("##animationblendslider", &animationBlend, 0.0f, 1.0f, "%.03f");

            ImGui::Text("Speed Modifier");
            ImGui::SliderFloat("##speedModifierslider", &speedModifier, 0.9f, 1.1f, "%.03f");

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Texture")) {

            if (ImGui::MenuItem("Texture Window")) {
                showTextureWindow = !showTextureWindow;
            }

            ImGui::EndMenu();
        }
        // Add more menus and items here
        ImGui::EndMainMenuBar();
    }
}

void LightWindow()
{
    ImGui::Begin("Light");

    ImGui::Text("Light Position: %f  %f  %f", lightPosition.x, lightPosition.y, lightPosition.z);

    ImGui::DragFloat("lightPosition X", &lightPosition.x, 0.005f);
    ImGui::DragFloat("lightPosition Y", &lightPosition.y, 0.005f);
    ImGui::DragFloat("lightPosition Z", &lightPosition.z, 0.005f);

    ImGui::End();
}

void Main_GUI_Loop(double time)
{
    double currentTime = time;
    frameCount++;

    if (currentTime - previousTimeFPS >= 1.0) // If one second has passed
    {
        fps = frameCount;
        frameCount = 0;
        previousTimeFPS = currentTime;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    // Frame Start
    ImGui::NewFrame();

    // ImGui::ShowMetricsWindow();
    ImGui::ShowDemoWindow();

    bool t = true;

    ShowExampleAppSimpleOverlay(&t, fps);
    LightWindow();

    if (showCollisionData) {
        CollisionData();
    }

    if (showTextureWindow) {
        TextureWindow();
    }

    MainMenuBar();

    // Frame End
    ImGui::Render();
}

#endif // !1