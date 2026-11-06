#include "lakakey_tsf_policy.h"
#include <cstdlib>
#include <iostream>
using namespace lakakey;
static void demand(bool cond, const char* what) {
    if (!cond) { std::cerr << "FAILED: " << what << '\n'; std::exit(1); }
}
static void verify(KeyInput in, KeyAction action, bool eat) {
    auto result=plan_key(in);
    demand(result.action==action,"key action mismatch");
    demand(result.eat_on_success==eat,"key consumption mismatch");
}
int main() {
    KeyInput plain{};
    plain.character=U'a';
    verify(plain,KeyAction::Insert,true);
    plain.enabled=false; verify(plain,KeyAction::Pass,false);
    plain.enabled=true; plain.secure=true; verify(plain,KeyAction::Pass,false);
    plain.secure=false; plain.shortcut_modifier=true; verify(plain,KeyAction::Pass,false);
    plain.shortcut_modifier=false; plain.same_context=false; verify(plain,KeyAction::Pass,false);
    plain.same_context=true; plain.character=U'\0';
    plain.backspace=true; verify(plain,KeyAction::Pass,false);
    plain.composing=true; plain.has_preedit=true; verify(plain,KeyAction::Erase,true);
    plain.has_preedit=false; verify(plain,KeyAction::Pass,false);
    plain.backspace=false; plain.boundary=true;
    verify(plain,KeyAction::Finalize,false);
    plain.composing=false; verify(plain,KeyAction::Pass,false);
    plain.composing=true; plain.character=U'b';
    verify(plain,KeyAction::Insert,true);
    for (int flags=0; flags < 128; ++flags) {
        KeyInput t{};
        t.enabled=(flags&1)!=0; t.secure=(flags&2)!=0;
        t.shortcut_modifier=(flags&4)!=0; t.composing=(flags&8)!=0;
        t.same_context=(flags&16)!=0; t.has_preedit=(flags&32)!=0;
        t.backspace=(flags&64)!=0;
        for (int kind=0;kind<3;++kind) {
            t.character=kind==1?U'c':U'\0'; t.boundary=kind==2;
            KeyPlan p=plan_key(t);
            demand(!p.eat_on_success || p.action==KeyAction::Insert || p.action==KeyAction::Erase,
                   "host key incorrectly consumed");
            if (t.secure || !t.enabled || t.shortcut_modifier || !t.same_context)
                demand(p.action==KeyAction::Pass,"security or focus bypass failed");
        }
    }
    demand(is_valid_scalar(U'á') && is_valid_scalar(U'👋'),"Unicode scalar check");
    demand(!is_valid_scalar(static_cast<char32_t>(0xd800)),"surrogate accepted");
    demand(!is_valid_scalar(static_cast<char32_t>(0x110000)),"out-of-range scalar accepted");
    std::cout << "TSF key policy contract: PASS\n";
}
