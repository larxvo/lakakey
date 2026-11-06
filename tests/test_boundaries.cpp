// Regression checks for committed delimiters and live-preedit transitions.
// No OS hooks or user typing data. © 2026 Larx Vo
#include "lakakey_engine.h"
#include <cstdlib>
#include <iostream>
#include <string>

namespace {
using namespace lakakey;
void must(bool condition,const char* message) {
    if (!condition) { std::cerr<<"boundary regression: "<<message<<"\n";std::exit(1); }
}
void check(const std::string& sequence,const std::string& expected,
           Settings settings={}) {
    Engine engine(settings);
    std::string actual;
    for (char32_t cp : utf8_decode(sequence)) {
        auto out=engine.feed(cp);
        must(out.consumed,"valid sequence unexpectedly bypassed");
        actual += out.commit;
        if (!out.commit.empty()) {
            must(out.preedit.empty(),"commit left stale preedit");
            must(engine.preedit().empty(),"commit left engine state");
        }
    }
    actual += engine.commit().commit;
    if (actual != expected) {
        std::cerr << "boundary regression: unexpected output for "<<sequence<<"\n";
        std::exit(1);
    }
}
} // namespace

int main() {
    check("tieengs ", "tiếng ");
    check("tieengs.", "tiếng.");
    check("tieengs, ", "tiếng, ");
    check("chaof!", "chào!");
    check("chao? xinf", "chao? xìn");
    check("Vieetj Nam.", "Việt Nam.");
    check("ddoof;", "đồ;");
    check("a1.", "á.", {Scheme::Vni,ToneStyle::Modern,true});
    check("tie6ng1, ", "tiếng, ", {Scheme::Vni,ToneStyle::Modern,true});
    check("HTML CSS!", "HTML CSS!");
    // At the start of a field, delimiter keys should pass through the host
    // (this engine-only test verifies the equivalent textual result).
    check(" .", " .");
    check("   ", "   ");
    for(int i=0;i<2500;++i)
        check("tieengs, chaof! ", "tiếng, chào! ");
    std::cout<<"PASS boundary regressions and 2500 synthetic repeat sequences\n";
}
