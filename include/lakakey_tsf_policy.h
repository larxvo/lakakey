#pragma once
// A side-effect-free decision contract shared by TSF test and execution paths.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
#include <cstdint>

namespace lakakey {
enum class KeyAction : std::uint8_t { Pass, Insert, Erase, Finalize };
struct KeyInput {
    bool enabled = true;
    bool secure = false;
    bool shortcut_modifier = false;
    bool composing = false;
    bool same_context = true;
    bool has_preedit = false;
    bool boundary = false;
    bool backspace = false;
    char32_t character = U'\0';
};
struct KeyPlan {
    KeyAction action = KeyAction::Pass;
    bool eat_on_success = false;
};
// Never swallow a delimiter or navigation key. Finalize composition first,
// then allow the host application to process that key normally.
inline constexpr KeyPlan plan_key(const KeyInput& in) noexcept {
    if (!in.enabled || in.secure || in.shortcut_modifier || !in.same_context)
        return {};
    if (in.character != U'\0')
        return {KeyAction::Insert, true};
    if (in.composing && in.backspace && in.has_preedit)
        return {KeyAction::Erase, true};
    if (in.composing && in.boundary)
        return {KeyAction::Finalize, false};
    return {};
}
inline constexpr bool is_valid_scalar(char32_t cp) noexcept {
    return cp <= 0x10ffff && (cp < 0xd800 || cp > 0xdfff);
}
}  // namespace lakakey
