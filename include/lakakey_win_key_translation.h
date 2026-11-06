#pragma once
// Windows keyboard-layout translator for TSF. No injected input or text logging.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
#if !defined(_WIN32)
#error This header is only for native Windows builds
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lakakey {
// This overload accepts a caller-supplied keyboard snapshot so CI can test
// layout/shift behavior without manipulating system keyboard state.
inline char32_t translate_keyboard_key(WPARAM key, LPARAM lparam,
                                       HKL layout, const BYTE state[256]) {
    if (!layout || !state) return U'\0';
    wchar_t utf16[8]{};
    const UINT scan_code = (static_cast<UINT>(lparam) >> 16) & 0xffU;
    // Windows 10 1607+; prevents ToUnicodeEx from mutating dead-key state.
    constexpr UINT preserve_dead_keys = 0x04U;
    const int count = ToUnicodeEx(static_cast<UINT>(key), scan_code, state,
                                  utf16, 8, preserve_dead_keys, layout);
    if (count != 1) return U'\0';
    const wchar_t c = utf16[0];
    if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
        (c >= L'0' && c <= L'9'))
        return static_cast<char32_t>(c);
    // Host owns punctuation, dead keys, and non-ASCII input.
    return U'\0';
}
inline char32_t translate_keyboard_key(WPARAM key, LPARAM lparam) {
    BYTE state[256]{};
    if (!GetKeyboardState(state)) return U'\0';
    return translate_keyboard_key(key, lparam, GetKeyboardLayout(0), state);
}
} // namespace lakakey
