#include <deps/overlay/overlay.h>
ID3D11Device* Overlay::device = nullptr;
ID3D11DeviceContext* Overlay::device_ctx = nullptr;
IDXGISwapChain* Overlay::swap_chain = nullptr;
ID3D11RenderTargetView* Overlay::render_target = nullptr;

auto Overlay::hijack_window() -> bool
{
    discord_hwnd = FindWindowA(skCrypt("Chrome_WidgetWin_1"), skCrypt("Discord Overlay"));
    if (!discord_hwnd)
        return false;

    return true;
}

auto Overlay::create_device() -> bool
{
    DXGI_SWAP_CHAIN_DESC swap_chain_description = {};
    swap_chain_description.BufferCount = 2;
    swap_chain_description.BufferDesc.Width = 0;
    swap_chain_description.BufferDesc.Height = 0;
    swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_description.BufferDesc.RefreshRate.Numerator = 240;
    swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_description.OutputWindow = discord_hwnd;
    swap_chain_description.SampleDesc.Count = 1;
    swap_chain_description.SampleDesc.Quality = 0;
    swap_chain_description.Windowed = TRUE;
    swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const D3D_FEATURE_LEVEL feature_array[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };

    D3D_FEATURE_LEVEL d3d_feature_lvl;
    HRESULT swap_chain_result = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 0, feature_array, 2,
        D3D11_SDK_VERSION, &swap_chain_description,
        &swap_chain, &device,
        &d3d_feature_lvl,
        &device_ctx);

    if (FAILED(swap_chain_result))
        return false;

    ID3D11Texture2D* buffer = nullptr;
    swap_chain_result = swap_chain->GetBuffer(0, IID_PPV_ARGS(&buffer));
    if (FAILED(swap_chain_result) || !buffer)
        return false;

    swap_chain_result = device->CreateRenderTargetView(buffer, nullptr, &render_target);
    buffer->Release();

    if (FAILED(swap_chain_result))
        return false;

    if (!ImGui::GetCurrentContext())
        ImGui::CreateContext();

    if (!ImGui_ImplWin32_Init(discord_hwnd) || !ImGui_ImplDX11_Init(device, device_ctx))
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; 

    ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 16.0f);
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(15, 15);
    style.WindowRounding = 8.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(450, 350);

    style.FramePadding = ImVec2(8, 5);
    style.FrameRounding = 4.0f;
    style.FrameBorderSize = 0.0f;

    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 25.0f;

    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;

    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;

    style.GrabMinSize = 10.0f;
    style.GrabRounding = 4.0f;

    ImVec4* colors = style.Colors;

    const ImVec4 Background = ImVec4(0.08f, 0.08f, 0.12f, 0.98f);
    const ImVec4 BackgroundDark = ImVec4(0.05f, 0.05f, 0.08f, 1.0f);
    const ImVec4 Primary = ImVec4(0.20f, 0.60f, 1.0f, 1.0f);
    const ImVec4 PrimaryHover = ImVec4(0.30f, 0.70f, 1.0f, 1.0f);
    const ImVec4 PrimaryActive = ImVec4(0.15f, 0.50f, 0.90f, 1.0f);
    const ImVec4 Accent = ImVec4(0.40f, 0.80f, 1.0f, 1.0f);
    const ImVec4 TextDark = ImVec4(0.50f, 0.55f, 0.65f, 1.0f);
    const ImVec4 Border = ImVec4(0.15f, 0.15f, 0.20f, 1.0f);

    colors[ImGuiCol_WindowBg] = Background;
    colors[ImGuiCol_ChildBg] = BackgroundDark;
    colors[ImGuiCol_PopupBg] = Background;

    colors[ImGuiCol_Border] = Border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    colors[ImGuiCol_FrameBg] = BackgroundDark;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);

    colors[ImGuiCol_TitleBg] = BackgroundDark;
    colors[ImGuiCol_TitleBgActive] = BackgroundDark;
    colors[ImGuiCol_TitleBgCollapsed] = BackgroundDark;

    colors[ImGuiCol_MenuBarBg] = BackgroundDark;

    colors[ImGuiCol_ScrollbarBg] = BackgroundDark;
    colors[ImGuiCol_ScrollbarGrab] = Border;
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.30f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.35f, 1.0f);

    colors[ImGuiCol_CheckMark] = Accent;
    colors[ImGuiCol_SliderGrab] = Primary;
    colors[ImGuiCol_SliderGrabActive] = PrimaryActive;

    colors[ImGuiCol_Button] = BackgroundDark;
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.28f, 1.0f);

    colors[ImGuiCol_Header] = BackgroundDark;
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.20f, 0.28f, 1.0f);

    colors[ImGuiCol_Separator] = Border;
    colors[ImGuiCol_SeparatorHovered] = Primary;
    colors[ImGuiCol_SeparatorActive] = PrimaryActive;

    colors[ImGuiCol_ResizeGrip] = BackgroundDark;
    colors[ImGuiCol_ResizeGripHovered] = Primary;
    colors[ImGuiCol_ResizeGripActive] = PrimaryActive;

    colors[ImGuiCol_Tab] = BackgroundDark;
    colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.60f, 1.0f, 0.4f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.60f, 1.0f, 0.6f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.17f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.22f, 1.0f);

    ImGui::GetIO().Fonts->Build();

    auto* draw = ImGui::GetBackgroundDrawList();
    draw->Flags |= ImDrawListFlags_AntiAliasedLines;
    draw->Flags |= ImDrawListFlags_AntiAliasedFill;

    return true;
}

auto Overlay::destroy_device() -> bool
{
    if (ImGui::GetCurrentContext())
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (render_target)
    {
        render_target->Release();
        render_target = nullptr;
    }

    if (swap_chain)
    {
        swap_chain->SetFullscreenState(FALSE, nullptr);
        swap_chain->Release();
        swap_chain = nullptr;
    }

    if (device_ctx)
    {
        device_ctx->ClearState();
        device_ctx->Flush();
        device_ctx->Release();
        device_ctx = nullptr;
    }

    if (device)
    {
        device->Release();
        device = nullptr;
    }
    return true;
}