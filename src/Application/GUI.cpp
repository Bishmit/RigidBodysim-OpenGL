#include "GUI.h"

#include <filesystem>
#include <cstring>
#include <cmath>

// Static member definitions
bool  GUI::show_panel    = true;
float GUI::panel_x       = 0.0f;
float GUI::addBoxWidth   = 100.f;
float GUI::addBoxHeight  = 20.f;
float GUI::localRotation = 0.f;

float GUI::EaseOut(float a, float b, float t) {
    t = 1 - powf(1 - t, 3);
    return a + (b - a) * t;
}

void GUI::Render([[maybe_unused]] GLFWwindow* window, SimContext& ctx) {
    const float panel_width = 360.0f;

    ImGuiIO& io      = ImGui::GetIO();
    float    screen_w = io.DisplaySize.x;
    float    screen_h = io.DisplaySize.y;
    float    target_x = show_panel ? (screen_w - panel_width) : screen_w;

    panel_x = EaseOut(panel_x, target_x, io.DeltaTime * 10.0f);

    // --- Side panel ---
    ImGui::SetNextWindowPos(ImVec2(panel_x, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panel_width, screen_h), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.92f);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(6, 4));
    ImGui::PushStyleColor(ImGuiCol_WindowBg,      ImVec4(0.08f, 0.08f, 0.12f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Separator,     ImVec4(0.3f,  0.3f,  0.5f,  1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,       ImVec4(0.14f, 0.14f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,    ImVec4(0.4f,  0.6f,  1.0f,  1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.6f, 0.8f, 1.0f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,     ImVec4(0.4f,  0.9f,  0.4f,  1.f));

    ImGui::Begin("##panel", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar);

    // Header
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.f));
    ImGui::SetWindowFontScale(1.1f);
    ImGui::Text("  RigidBody Simulator");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 1.f));
    ImGui::Text("  Press H to toggle panel");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // Stats row
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 0.6f, 1.f));
    ImGui::Text("Bodies: %zu", ctx.bodies.size());
    ImGui::SameLine(0, 20.f);
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // --- Simulation controls ---
    ImGui::SeparatorText("Simulation");
    ImGui::Spacing();

    bool isPaused = ctx.pause;
    if (isPaused) {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.6f, 0.4f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.6f, 0.0f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.4f, 0.3f, 0.0f, 1.f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.1f, 0.4f, 0.1f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.6f, 0.2f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.0f, 0.3f, 0.0f, 1.f));
    }
    if (ImGui::Button(isPaused ? "Resume" : "Pause", ImVec2(-1, 32)))
        ctx.pause = !ctx.pause;
    ImGui::PopStyleColor(3);
    ImGui::Spacing();

    ImGui::SliderFloat("Gravity",     &ctx.gravity,     -10.f, 10.f);
    ImGui::SliderFloat("Restitution", &ctx.restitution,  0.0f,  1.f);
    ImGui::SliderFloat("Friction",    &ctx.friction,     0.0f,  1.f);
    ImGui::InputInt("Max Iterations", &ctx.maxIteration, 1);
    if (ImGui::SliderFloat("Correction", &ctx.correctionValue, 0.0f, 1.f))
        CollisionSolver::SetCorrectionValue(ctx.correctionValue);

    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.18f, 0.18f, 0.28f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.28f, 0.45f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.10f, 0.20f, 1.f));
    if (ImGui::Button(ctx.showNormal ? "Hide Normals" : "Show Normals", ImVec2(160, 28)))
        ctx.showNormal = !ctx.showNormal;
    ImGui::SameLine();
    if (ImGui::Button(ctx.showCollisionPoint ? "Hide Contacts" : "Show Contacts", ImVec2(-1, 28)))
        ctx.showCollisionPoint = !ctx.showCollisionPoint;
    ImGui::PopStyleColor(3);

    // --- Great ball ---
    ImGui::Spacing();
    ImGui::SeparatorText("Great Ball");
    ImGui::Spacing();

    if (ImGui::SliderFloat("Circle Radius", &ctx.radius_, 10.f, 200.f))
        if (ctx.greatBall) ctx.greatBall->SetRadius(ctx.radius_);

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.3f,  0.2f, 0.5f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f,  0.3f, 0.8f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.2f,  0.1f, 0.4f, 1.f));
    if (ImGui::Button(ctx.attachPendulum ? "Detach Pendulum" : "Attach Pendulum", ImVec2(-1, 28)))
        ctx.attachPendulum = !ctx.attachPendulum;
    ImGui::PopStyleColor(3);

    // --- Body builder ---
    ImGui::Spacing();
    ImGui::SeparatorText("Body Builder");
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.15f, 0.35f, 0.55f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.50f, 0.80f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.10f, 0.25f, 0.40f, 1.f));
    if (ImGui::Button("+ Add Box", ImVec2(120, 30))) {
        Body* addBox = new Body(BoxShape(addBoxWidth, addBoxHeight), 200.f, 200.f, 1.f, localRotation);
        ctx.recentSelectedBody = addBox;
        ctx.bodies.push_back(addBox);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.12f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.40f, 0.08f, 0.08f, 1.f));
    if (ImGui::Button("Clear All", ImVec2(-1, 30)))
        ctx.onClearAll();
    ImGui::PopStyleColor(3);

    ImGui::Spacing();

    if (ctx.recentSelectedBody) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.11f, 0.11f, 0.18f, 1.f));
        ImGui::BeginChild("##selectedBody", ImVec2(-1, 165), true);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.9f, 0.6f, 1.f));
        ImGui::Text("Selected Body");
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::SliderAngle("Rotation", &localRotation, -90.f, 90.f))
            ctx.recentSelectedBody->rotation = localRotation;

        if (ImGui::InputFloat("Width", &addBoxWidth, 1.f, 10.f, "%.1f")) {
            ctx.recentSelectedBody->SetWidth(addBoxWidth);
            ctx.recentSelectedBody->UpdateShapeData();
        }
        if (ImGui::InputFloat("Height", &addBoxHeight, 1.f, 10.f, "%.1f")) {
            ctx.recentSelectedBody->SetHeight(addBoxHeight);
            ctx.recentSelectedBody->UpdateShapeData();
        }

        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.55f, 0.12f, 0.12f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.20f, 0.20f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.40f, 0.08f, 0.08f, 1.f));
        if (ImGui::Button("Delete Selected", ImVec2(-1, 28)))
            ctx.onDeleteBody(ctx.recentSelectedBody);
        ImGui::PopStyleColor(3);

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    // --- State save / load ---
    ImGui::Spacing();
    ImGui::SeparatorText("State Save / Load");
    ImGui::Spacing();

    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##stateName", ctx.stateName, 128);

    ImGui::Spacing();
    std::string filepath = std::string("states/") + ctx.stateName + ".json";

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.50f, 0.15f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.70f, 0.20f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.35f, 0.10f, 1.f));
    if (ImGui::Button("Save State", ImVec2(160, 34))) {
        if (std::filesystem::exists(filepath)) {
            ctx.showOverwriteModal = true;
            strncpy(ctx.pendingFilepath, filepath.c_str(), 256);
        } else {
            std::filesystem::create_directories("states");
            ctx.onSave(filepath);
            ctx.showSavedToast = true;
            ctx.toastTimer = 2.5f;
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.25f, 0.60f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.40f, 0.90f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.15f, 0.45f, 1.f));
    if (ImGui::Button("Load State", ImVec2(-1, 34))) {
        if (std::filesystem::exists(filepath))
            ctx.onLoad(filepath);
        else {
            ctx.showLoadFailToast = true;
            ctx.toastTimer = 2.5f;
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.45f, 0.45f, 0.55f, 1.f));
    ImGui::Text("Path: states/%s.json", ctx.stateName);
    ImGui::PopStyleColor();

    // --- Overwrite modal ---
    if (ctx.showOverwriteModal)
        ImGui::OpenPopup("File Already Exists##modal");

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(390, 0), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.10f, 0.10f, 0.16f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Border,  ImVec4(0.40f, 0.40f, 0.60f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 14));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);

    if (ImGui::BeginPopupModal("File Already Exists##modal", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.75f, 0.1f, 1.f));
        ImGui::SetWindowFontScale(1.1f);
        ImGui::Text("File Already Exists");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextWrapped("A save file named \"%s\" already exists.", ctx.stateName);
        ImGui::Spacing();
        ImGui::TextWrapped("Choose to overwrite it, or save under a new name below.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.65f, 0.12f, 0.10f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.90f, 0.20f, 0.15f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.08f, 0.06f, 1.f));
        if (ImGui::Button("Overwrite", ImVec2(110, 34))) {
            std::filesystem::create_directories("states");
            ctx.onSave(std::string(ctx.pendingFilepath));
            ctx.showOverwriteModal = false;
            ctx.showSavedToast    = true;
            ctx.toastTimer        = 2.5f;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::SetNextItemWidth(210.f);
        ImGui::InputText("##newname", ctx.newSaveName, 128);
        ImGui::SameLine();

        bool nameEmpty = (ctx.newSaveName[0] == '\0');
        if (nameEmpty) ImGui::BeginDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.35f, 0.65f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.50f, 0.90f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.25f, 0.50f, 1.f));
        if (ImGui::Button("Save As", ImVec2(-1, 34))) {
            std::string newPath = "states/" + std::string(ctx.newSaveName) + ".json";
            if (!std::filesystem::exists(newPath)) {
                std::filesystem::create_directories("states");
                ctx.onSave(newPath);
                strncpy(ctx.stateName, ctx.newSaveName, 128);
                memset(ctx.newSaveName, 0, 128);
                ctx.showOverwriteModal = false;
                ctx.showSavedToast    = true;
                ctx.toastTimer        = 2.5f;
                ImGui::CloseCurrentPopup();
            } else {
                memset(ctx.newSaveName, 0, 128);
            }
        }
        ImGui::PopStyleColor(3);
        if (nameEmpty) ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        float cancelW = 80.f;
        ImGui::SetCursorPosX((390.f - cancelW) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.22f, 0.22f, 0.30f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.50f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.15f, 0.15f, 0.22f, 1.f));
        if (ImGui::Button("Cancel", ImVec2(cancelW, 28))) {
            ctx.showOverwriteModal = false;
            memset(ctx.newSaveName, 0, 128);
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    // --- Toast notifications ---
    if (ctx.showSavedToast || ctx.showLoadFailToast) {
        bool  isSuccess = ctx.showSavedToast;
        float toastW = 280.f, toastH = 42.f;

        ImGui::SetNextWindowPos(
            ImVec2(screen_w * 0.5f - toastW * 0.5f, screen_h - toastH - 24.f),
            ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.88f);

        ImGui::Begin("##toast", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove);

        if (isSuccess) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 1.f, 0.45f, 1.f));
            ImGui::Text("State saved: %s.json", ctx.stateName);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.35f, 0.35f, 1.f));
            ImGui::Text("File not found: %s.json", ctx.stateName);
        }
        ImGui::PopStyleColor();
        ImGui::End();

        ctx.toastTimer -= io.DeltaTime;
        if (ctx.toastTimer <= 0.f) {
            ctx.showSavedToast    = false;
            ctx.showLoadFailToast = false;
        }
    }

    // --- End panel ---
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(6);

    // H key toggles panel
    if (ImGui::IsKeyPressed(ImGuiKey_H))
        show_panel = !show_panel;
}
