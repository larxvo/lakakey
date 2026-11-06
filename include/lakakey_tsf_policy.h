#pragma once
// A side-effect-free decision contract shared by TSF test and execution paths.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com
#include <cstdint>

namespace lakakey {
enum class KeyAction : std::uint8_t { Pass, Insert, Erase, Finalize, CommitSpace };
struct KeyInput {
    bool enabled = true;
    bool secure = false;
    bool shortcut_modifier = false;
    bool composing = false;
    bool same_context = true;
    bool has_preedit = false;
    bool boundary = false;
    bool backspace = false;
    bool space = false;
    char32_t character = U'\0';
};
struct KeyPlan {
    KeyAction action = KeyAction::Pass;
    bool eat_on_success = false;
};
// Space must commit the full word and separator in one TSF edit session.
// Other navigation/boundary keys are left to the host unchanged.
inline constexpr KeyPlan plan_key(const KeyInput& in) noexcept {
    if (!in.enabled || in.secure || in.shortcut_modifier || !in.same_context)
        return {};
    if (in.composing && in.space && in.has_preedit)
        return {KeyAction::CommitSpace, true};
    if (in.character != U'\0')
        return {KeyAction::Insert, true};
    if (in.composing && in.backspace && in.has_preedit)
        return {KeyAction::Erase, true};
    if (in.composing && in.boundary)
        return {KeyAction::Pass, false};
    return {};
}
inline constexpr bool is_valid_scalar(char32_t cp) noexcept {
    return cp <= 0x10ffff && (cp < 0xd800 || cp > 0xdfff);
}
}  // namespace lakakey
