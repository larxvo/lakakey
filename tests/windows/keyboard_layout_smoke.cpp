// Windows-only keyboard layout regression checks, no synthesized keystrokes.
// Copyright (c) 2026 Larx Vo
#include "lakakey_win_key_translation.h"
#include <iostream>

static int fail(const char* error) {
    std::cerr << "keyboard translation: FAIL " << error << "\n";
    return 1;
}
int main() {
    HKL layout=LoadKeyboardLayoutW(L"00000409", KLF_NOTELLSHELL);
    if (!layout) return fail("US layout unavailable on runner");
    BYTE plain[256]{};
    const LPARAM a_scan = static_cast<LPARAM>(0x1eU << 16);
    if (lakakey::translate_keyboard_key('A',a_scan,layout,plain)!=U'a')
        return fail("US alpha lowercase");
    plain[VK_SHIFT]=0x80;
    if (lakakey::translate_keyboard_key('A',a_scan,layout,plain)!=U'A')
        return fail("US shift+alpha uppercase");
    plain[VK_SHIFT]=0;
    plain[VK_CAPITAL]=0x01;
    if (lakakey::translate_keyboard_key('A',a_scan,layout,plain)!=U'A')
        return fail("US capslock uppercase");
    plain[VK_CAPITAL]=0;
    if (lakakey::translate_keyboard_key('1',static_cast<LPARAM>(0x02U<<16),layout,plain)!=U'1')
        return fail("US numeric key");
    plain[VK_SHIFT]=0x80;
    if (lakakey::translate_keyboard_key('1',static_cast<LPARAM>(0x02U<<16),layout,plain)!=U'\0')
        return fail("punctuation must pass through");
    if (lakakey::translate_keyboard_key(VK_LEFT,static_cast<LPARAM>(0x4bU<<16),layout,plain)!=U'\0')
        return fail("navigation must pass through");
    std::cout << "Windows keyboard-layout translation: PASS\n";
}
