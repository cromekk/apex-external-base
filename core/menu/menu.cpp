#include "menu.h"
#include <include/includes.h>
#include <core/systems/visuals/visuals.h>
#include <map>
#include <cmath>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <deps/overlay/overlay.h>

static ImGuiKey VKToImGuiKey(int vk)
{
    switch (vk)
    {
    case VK_TAB: return ImGuiKey_Tab;
    case VK_LEFT: return ImGuiKey_LeftArrow;
    case VK_RIGHT: return ImGuiKey_RightArrow;
    case VK_UP: return ImGuiKey_UpArrow;
    case VK_DOWN: return ImGuiKey_DownArrow;
    case VK_PRIOR: return ImGuiKey_PageUp;
    case VK_NEXT: return ImGuiKey_PageDown;
    case VK_HOME: return ImGuiKey_Home;
    case VK_END: return ImGuiKey_End;
    case VK_INSERT: return ImGuiKey_Insert;
    case VK_DELETE: return ImGuiKey_Delete;
    case VK_BACK: return ImGuiKey_Backspace;
    case VK_SPACE: return ImGuiKey_Space;
    case VK_RETURN: return ImGuiKey_Enter;
    case VK_ESCAPE: return ImGuiKey_Escape;
    case VK_OEM_7: return ImGuiKey_Apostrophe;
    case VK_OEM_COMMA: return ImGuiKey_Comma;
    case VK_OEM_MINUS: return ImGuiKey_Minus;
    case VK_OEM_PERIOD: return ImGuiKey_Period;
    case VK_OEM_2: return ImGuiKey_Slash;
    case VK_OEM_1: return ImGuiKey_Semicolon;
    case VK_OEM_PLUS: return ImGuiKey_Equal;
    case VK_OEM_4: return ImGuiKey_LeftBracket;
    case VK_OEM_5: return ImGuiKey_Backslash;
    case VK_OEM_6: return ImGuiKey_RightBracket;
    case VK_OEM_3: return ImGuiKey_GraveAccent;
    case VK_CAPITAL: return ImGuiKey_CapsLock;
    case VK_SCROLL: return ImGuiKey_ScrollLock;
    case VK_NUMLOCK: return ImGuiKey_NumLock;
    case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
    case VK_PAUSE: return ImGuiKey_Pause;
    case VK_NUMPAD0: return ImGuiKey_Keypad0;
    case VK_NUMPAD1: return ImGuiKey_Keypad1;
    case VK_NUMPAD2: return ImGuiKey_Keypad2;
    case VK_NUMPAD3: return ImGuiKey_Keypad3;
    case VK_NUMPAD4: return ImGuiKey_Keypad4;
    case VK_NUMPAD5: return ImGuiKey_Keypad5;
    case VK_NUMPAD6: return ImGuiKey_Keypad6;
    case VK_NUMPAD7: return ImGuiKey_Keypad7;
    case VK_NUMPAD8: return ImGuiKey_Keypad8;
    case VK_NUMPAD9: return ImGuiKey_Keypad9;
    case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
    case VK_DIVIDE: return ImGuiKey_KeypadDivide;
    case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
    case VK_ADD: return ImGuiKey_KeypadAdd;
    case VK_LSHIFT: case VK_RSHIFT: return ImGuiKey_RightShift;
    case VK_LCONTROL: case VK_RCONTROL: return ImGuiKey_RightCtrl;
    case VK_F1: return ImGuiKey_F1;
    case VK_F2: return ImGuiKey_F2;
    case VK_F3: return ImGuiKey_F3;
    case VK_F4: return ImGuiKey_F4;
    case VK_F5: return ImGuiKey_F5;
    case VK_F6: return ImGuiKey_F6;
    case VK_F7: return ImGuiKey_F7;
    case VK_F8: return ImGuiKey_F8;
    case VK_F9: return ImGuiKey_F9;
    case VK_F10: return ImGuiKey_F10;
    case VK_F11: return ImGuiKey_F11;
    case VK_F12: return ImGuiKey_F12;
    case '0': return ImGuiKey_0;
    case '1': return ImGuiKey_1;
    case '2': return ImGuiKey_2;
    case '3': return ImGuiKey_3;
    case '4': return ImGuiKey_4;
    case '5': return ImGuiKey_5;
    case '6': return ImGuiKey_6;
    case '7': return ImGuiKey_7;
    case '8': return ImGuiKey_8;
    case '9': return ImGuiKey_9;
    case 'A': return ImGuiKey_A;
    case 'B': return ImGuiKey_B;
    case 'C': return ImGuiKey_C;
    case 'D': return ImGuiKey_D;
    case 'E': return ImGuiKey_E;
    case 'F': return ImGuiKey_F;
    case 'G': return ImGuiKey_G;
    case 'H': return ImGuiKey_H;
    case 'I': return ImGuiKey_I;
    case 'J': return ImGuiKey_J;
    case 'K': return ImGuiKey_K;
    case 'L': return ImGuiKey_L;
    case 'M': return ImGuiKey_M;
    case 'N': return ImGuiKey_N;
    case 'O': return ImGuiKey_O;
    case 'P': return ImGuiKey_P;
    case 'Q': return ImGuiKey_Q;
    case 'R': return ImGuiKey_R;
    case 'S': return ImGuiKey_S;
    case 'T': return ImGuiKey_T;
    case 'U': return ImGuiKey_U;
    case 'V': return ImGuiKey_V;
    case 'W': return ImGuiKey_W;
    case 'X': return ImGuiKey_X;
    case 'Y': return ImGuiKey_Y;
    case 'Z': return ImGuiKey_Z;
    default: return ImGuiKey_None;
    }
}

static bool g_KeyStates[256] = {};
static bool g_MouseStates[5] = {};
static float g_LastScrollTime = 0.0f;
static float g_AccumulatedScroll = 0.0f;

void input()
{
    ImGuiIO& io = ImGui::GetIO();

    POINT p;
    if (GetCursorPos(&p))
    {
        io.AddMousePosEvent((float)p.x, (float)p.y);
    }

    bool mouse_states[5] = {
        (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0,
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0,
        (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0,
        (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0,
        (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0
    };

    for (int i = 0; i < 5; i++)
    {
        if (g_MouseStates[i] != mouse_states[i])
        {
            g_MouseStates[i] = mouse_states[i];
            io.AddMouseButtonEvent(i, mouse_states[i]);
        }
    }

    static SHORT last_wheel_state = 0;
    SHORT wheel_state = GetAsyncKeyState(VK_MBUTTON);
    SHORT wheel_delta = wheel_state - last_wheel_state;

    if (wheel_delta != 0)
    {
        float wheel_amount = (float)wheel_delta / 120.0f;
        if (abs(wheel_delta) > 1)
        {
            io.AddMouseWheelEvent(0.0f, wheel_amount);
        }
    }
    last_wheel_state = wheel_state;

    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    io.AddKeyEvent(ImGuiMod_Ctrl, ctrl);
    io.AddKeyEvent(ImGuiMod_Shift, shift);
    io.AddKeyEvent(ImGuiMod_Alt, alt);

    for (int vk = 0; vk < 256; vk++)
    {
        bool is_down = (GetAsyncKeyState(vk) & 0x8000) != 0;

        if (g_KeyStates[vk] != is_down)
        {
            g_KeyStates[vk] = is_down;

            ImGuiKey key = VKToImGuiKey(vk);
            if (key != ImGuiKey_None)
            {
                io.AddKeyEvent(key, is_down);
            }

            if (is_down && vk >= 0x20 && vk <= 0xFE)
            {
                BYTE kb[256];
                if (GetKeyboardState(kb))
                {
                    WCHAR wch[4];
                    int result = ToUnicode(vk, MapVirtualKeyW(vk, MAPVK_VK_TO_VSC), kb, wch, 4, 0);

                    if (result > 0)
                    {
                        for (int i = 0; i < result; i++)
                        {
                            if (wch[i] > 0 && wch[i] < 0x10000)
                                io.AddInputCharacter((unsigned int)wch[i]);
                        }
                    }
                }
            }
        }
    }
}

namespace L7Menu
{
    void menu()
    {
        static bool init = false;
        if (!init)
        {
            ImGui::StyleColorsDark();
            init = true;
        }

        ImGui::SetNextWindowSize(ImVec2(280, 150), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("APEX EXTERNAL BASE", &settings::menu_key, ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Text("ESP Visuals");
            ImGui::Separator();
            ImGui::Checkbox("Box ESP", &settings::box);
            ImGui::Checkbox("Team Check", &settings::team_check);
            ImGui::Separator();
            ImGui::TextDisabled("Press INSERT to toggle menu");
        }
        ImGui::End();
    }
}
