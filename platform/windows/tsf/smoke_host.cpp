// Native input QA host: regular, multiline and password fields.
// It never records keystrokes or typed content. © 2026 Larx Vo.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace {
HWND normal_field = nullptr;
HWND notes_field = nullptr;
HWND password_field = nullptr;

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_CREATE: {
        HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(window, GWLP_HINSTANCE));
        CreateWindowExW(0, L"STATIC", L"LAKAKEY  |  Native Input QA",
                        WS_CHILD | WS_VISIBLE, 24, 16, 520, 30,
                        window, nullptr, instance, nullptr);
        CreateWindowExW(0, L"STATIC", L"Regular text · Telex / VNI, accents, deletion",
                        WS_CHILD | WS_VISIBLE, 24, 60, 500, 22,
                        window, nullptr, instance, nullptr);
        normal_field = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                          WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                          24, 88, 540, 30, window,
                          reinterpret_cast<HMENU>(101), instance, nullptr);
        CreateWindowExW(0, L"STATIC", L"Multiline · Enter, arrows, selection, focus changes",
                        WS_CHILD | WS_VISIBLE, 24, 140, 530, 22,
                        window, nullptr, instance, nullptr);
        notes_field = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                        ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_WANTRETURN,
                        24, 168, 540, 135, window,
                        reinterpret_cast<HMENU>(102), instance, nullptr);
        CreateWindowExW(0, L"STATIC", L"Password · no preedit or transformation",
                        WS_CHILD | WS_VISIBLE, 24, 322, 530, 22,
                        window, nullptr, instance, nullptr);
        password_field = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                         WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL,
                         24, 350, 540, 30, window,
                         reinterpret_cast<HMENU>(103), instance, nullptr);
        CreateWindowExW(0, L"STATIC",
                        L"Developer harness only. Activate a test input service separately.",
                        WS_CHILD | WS_VISIBLE, 24, 412, 550, 32,
                        window, nullptr, instance, nullptr);
        SetFocus(normal_field);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default: return DefWindowProcW(window, message, wparam, lparam);
    }
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    const wchar_t name[] = L"LakakeyTsfSmokeHost0_12";
    WNDCLASSW wc{};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.lpszClassName = name;
    wc.hCursor = LoadCursorW(nullptr, IDC_IBEAM);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    if (!RegisterClassW(&wc)) return 1;
    HWND window = CreateWindowExW(0, name, L"Lakakey | Native Input QA",
                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 625, 525,
                  nullptr, nullptr, instance, nullptr);
    if (!window) return 2;
    ShowWindow(window, show);
    UpdateWindow(window);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (IsDialogMessageW(window, &msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
