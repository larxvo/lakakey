#pragma once
#include "lakakey_engine.h"
#include <string>
namespace lakakey {
// Portable bridge contract. Host must render preedit through its native IME API,
// never by simulating backspaces into arbitrary applications.
struct SessionUpdate {
    std::string preedit_utf8;
    std::string commit_utf8;
    bool handled=false;
};
class InputSession {
    Engine engine_;
    bool secure_=false;
public:
    explicit InputSession(Settings settings={}) : engine_(settings) {}
    void focus_changed() {engine_.reset();}
    void secure_field(bool secure) {secure_=secure;engine_.reset();}
    void configure(Settings settings) {engine_.set_settings(settings);engine_.reset();}
    SessionUpdate character(char32_t cp) {
        if(secure_)return {};
        auto r=engine_.feed(cp);
        return {r.preedit,r.commit,r.consumed};
    }
    SessionUpdate erase() {
        if(secure_)return {};
        auto r=engine_.backspace();
        return {r.preedit,r.commit,r.consumed};
    }
    SessionUpdate finalize() {
        if(secure_)return {};
        auto r=engine_.commit();
        return {r.preedit,r.commit,r.consumed};
    }
};
}
