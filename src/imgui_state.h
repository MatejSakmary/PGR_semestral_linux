//
// Created by matejs on 11/05/2021.
//

#pragma once
#include "game_state.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>
#include <queue>

class ImguiState {
public:
    ImguiState():showCamWindow{false}, showLightsWindow{false}, showObjectsWindow{false}{};
    void ImguiDraw(GameState& gameState);

private:
    bool showCamWindow;
    bool showLightsWindow;
    bool showObjectsWindow;
};
