#ifndef LAKAKEY_ENGINE_H
#define LAKAKEY_ENGINE_H
#include <cstdint>
#include <string>
#include <vector>
namespace lakakey {
enum class Scheme { Telex, Vni, Plain };
enum class ToneStyle { Modern, Traditional };
struct Settings {
    Scheme scheme = Scheme::Telex;
    ToneStyle tone_style = ToneStyle::Modern;
    bool enabled = true;
};
struct Result {
    std::string commit;   // Send this to the host as committed text
    std::string preedit;  // Replace the whole currently displayed IME preedit
    bool consumed = true; // False: host should process the key itself
};
// Stateful per-input-context engine. Never store keystrokes on disk.
// Host adapter is responsible for sessions, cursor/focus changes and secure input fields.
class Engine {
public:
    explicit Engine(Settings settings = {});
    Result feed(char32_t codepoint);
    Result backspace();
    Result commit();
    void reset();
    void set_settings(Settings settings);
    const Settings& settings() const { return settings_; }
    std::string preedit() const;
private:
    Settings settings_;
    std::u32string letters_;
    std::u32string raw_events_; // enables precise Undo for nonprinting tone keys
    int tone_ = 0;
    bool has_vowel() const;
    void process_letter(char32_t c);
    std::u32string render_u32() const;
    static bool is_word_char(char32_t c);
};
std::string utf8_encode(const std::u32string& input);
std::u32string utf8_decode(const std::string& input);
}
#endif
