#include <Windows.h>
#include "Requeriments/imgui.h"
#include "Requeriments/imgui_impl_win32.h"
#include "Requeriments/imgui_impl_dx11.h"
#include "Utils/Variables/index.h"
#include "../Core/Engine.h"

bool showmenu = true;
Engine engine = Engine(); // A nossa instância principal do motor

void Render(HWND hwnd)
{
    // 1. Lógica correta de clique de rato e transparência
    // (Apenas executa um pedido à API do Windows quando abrimos ou fechamos o menu, em vez de o fazer a cada frame)
    static bool menu_estado_anterior = false; // Garante que atualiza na primeira iteração
    if (showmenu != menu_estado_anterior) {
        if (showmenu) {
            // Menu Aberto: Permite clicar no menu ImGui (removemos a flag TRANSPARENT)
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED);
        }
        else {
            // Menu Fechado: Os cliques do rato ignoram a nossa janela e passam para o jogo (adicionamos TRANSPARENT)
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
        }
        menu_estado_anterior = showmenu;
    }

    if (showmenu) {
        ImGui::SetNextWindowSize(ImVec2(520.f, 400.f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.97f);

        if (ImGui::Begin("Support Cheats", nullptr,
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize))
        {
            if (ImGui::BeginTabBar("##MainTabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton))
            {
                // =================== VISUALS ===================
                if (ImGui::BeginTabItem("Visuals"))
                {
                    ImGui::Text("ESP Settings");
                    ImGui::Separator();

                    ImGui::Checkbox("Enable ESP", &var::enableesp);

                    ImGui::BeginDisabled(!var::enableesp);
                    {
                        ImGui::Checkbox("Box", &var::box);
                        ImGui::Checkbox("Health", &var::health);
                        ImGui::Checkbox("Names", &var::names);
                        ImGui::Checkbox("Snaplines", &var::snaplines);
                        ImGui::Checkbox("Skeleton", &var::skeleton);
                    }
                    ImGui::EndDisabled();

                    ImGui::EndTabItem();
                }

                // =================== AIMBOT ====================
                if (ImGui::BeginTabItem("Aimbot"))
                {
                    ImGui::Text("Aimbot Settings");
                    ImGui::Separator();

                    ImGui::Checkbox("Enable Aimbot", &var::enable_aimbot);

                    ImGui::BeginDisabled(!var::enable_aimbot);
                    {
                        ImGui::SliderFloat("FOV", &var::aimbot_fov, 1.f, 360.f, "FOV: %.1f");
                        ImGui::Checkbox("Show FOV circle", &var::show_fov);

                        ImGui::SliderFloat("Max distance", &var::aimbot_distance,
                            50.f, 10000.f, "%.0f units");

                        ImGui::SliderFloat("Smoothness", &var::smoothness,
                            1.f, 20.f, "Smooth: %.1f");
                    }
                    ImGui::EndDisabled();

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    // 2. Renderizar ESP independentemente do menu estar aberto ou fechado!
    // Assim o cheat nunca para de processar os grafismos, e previne falhas de memória.
    engine.RenderEsp();

    // 3. Lógica do botão de Toggle (INSERT)
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        showmenu = !showmenu;
    }
}