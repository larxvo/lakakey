// Deterministic, host-neutral TSF typing transaction simulation.
// This verifies core + key-routing behavior, not a live Windows application.
// Copyright (c) 2026 Larx Vo — https://vovinhlac.com.
#include "lakakey_engine.h"
#include "lakakey_tsf_policy.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
using namespace lakakey;

static void require(bool ok, const char* reason) {
    if (!ok) { std::cerr << "FAIL " << reason << "\n"; std::exit(1); }
}
class SimulatedHost {
    Engine engine_;
    std::string document_;
    std::string composition_;
    bool enabled_=true;
    bool secure_=false;
public:
    explicit SimulatedHost(Settings s={}) : engine_(s) {}
    const std::string& document() const { return document_; }
    std::string visible() const { return document_ + composition_; }
    void secure(bool value) {
        secure_=value;
        engine_.reset();
        composition_.clear();
    }
    void enabled(bool value) {
        enabled_=value;
        auto s=engine_.settings();
        s.enabled=value;
        engine_.set_settings(s);
        engine_.reset();
        composition_.clear();
    }
    void focus_changed() {
        engine_.reset();
        composition_.clear();
    }
    KeyPlan input(char32_t ch, bool shortcut=false) {
        const bool space=ch==U' ';
        const bool backspace=ch==U'\b';
        const bool boundary=space || ch==U'\n' || ch==U'.';
        const bool letter_or_digit=(ch>=U'a'&&ch<=U'z')||
                                 (ch>=U'A'&&ch<=U'Z')||
                                 (ch>=U'0'&&ch<=U'9');
        const bool printable=(ch>=U'!'&&ch<=U'~');
        const char32_t character=(letter_or_digit ||
              (!composition_.empty() && printable)) ? ch : U'\0';
        const KeyPlan plan=plan_key({enabled_,secure_,shortcut,
            !composition_.empty(),true,!composition_.empty(),boundary,backspace,space,character});
        if(plan.action==KeyAction::Insert || plan.action==KeyAction::CommitSpace ||
           plan.action==KeyAction::Erase) {
            const Result result=plan.action==KeyAction::Erase ? engine_.backspace() :
                engine_.feed(plan.action==KeyAction::CommitSpace ? U' ' : character);
            require(result.consumed,"TSF transaction unexpectedly failed");
            document_+=result.commit;
            composition_=result.preedit;
        } else if(plan.action==KeyAction::Pass) {
            // Host owns plaintext input when Lakakey is disabled or in secure fields.
            if(ch>=U' ' && ch<=U'~' && ch!=U'\b')
                document_.push_back(static_cast<char>(ch));
        }
        return plan;
    }
};
int main() {
    SimulatedHost telex;
    for(char32_t ch:std::u32string(U"tieengs")) telex.input(ch);
    require(telex.visible()=="tiếng","live Telex composition");
    auto p=telex.input(U' ');
    require(p.action==KeyAction::CommitSpace && p.eat_on_success,"Space transaction policy");
    require(telex.document()=="tiếng ","single committed word plus space");
    telex.input(U'.');
    require(telex.document()=="tiếng .","host owns punctuation");
    Settings vni{};
    vni.scheme=Scheme::Vni;
    SimulatedHost v(vni);
    for(char32_t ch:std::u32string(U"tie6ng1 ")) v.input(ch);
    require(v.document()=="tiếng ","VNI composition plus space");
    SimulatedHost punctuation;
    for(char32_t ch:std::u32string(U"chaof!")) punctuation.input(ch);
    require(punctuation.document()=="chào!","atomic punctuation commit");
    require(punctuation.visible()=="chào!","no lingering composition after punctuation");
    SimulatedHost bare_punctuation;
    auto host_punctuation=bare_punctuation.input(U'!');
    require(host_punctuation.action==KeyAction::Pass &&
            bare_punctuation.document()=="!","host owns punctuation without preedit");
    SimulatedHost erase;
    for(char32_t ch:std::u32string(U"chaof")) erase.input(ch);
    require(erase.visible()=="chào","Telex tone before backspace");
    erase.input(U'\b');
    require(erase.visible()=="chao","Backspace removes unprinted tone event");
    erase.input(U' ');
    require(erase.document()=="chao ","Backspace and boundary commit");
    SimulatedHost safe;
    safe.secure(true);
    const auto pass=safe.input(U'a');
    require(pass.action==KeyAction::Pass && !pass.eat_on_success,"secure bypass");
    safe.secure(false);
    safe.enabled(false);
    safe.input(U'b');
    require(safe.document()=="ab","disabled passthrough");
    safe.enabled(true);
    safe.input(U'c');
    safe.focus_changed();
    require(safe.visible()=="ab","no preedit leak across focus");
    SimulatedHost shortcuts;
    auto key=shortcuts.input(U'c',true);
    require(key.action==KeyAction::Pass && !key.eat_on_success,"shortcut bypass");
    std::cout<<"PASS end-to-end transaction simulation (not live Windows TSF)\n";
}
