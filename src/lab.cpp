#include "lakakey_engine.h"
#include <iostream>
#include <string>
int main(int argc, char** argv) {
    lakakey::Settings opts;
    for (int i=1;i<argc;++i) {
        std::string arg(argv[i]);
        if (arg=="--vni") opts.scheme=lakakey::Scheme::Vni;
        else if (arg=="--plain") opts.scheme=lakakey::Scheme::Plain;
        else if (arg=="--traditional") opts.tone_style=lakakey::ToneStyle::Traditional;
    }
    std::cout<<"Lakakey Engine Lab 0.1 — Larx Vo / vovinhlac.com\n"
             <<"Type a raw keystroke sequence then press Enter; Ctrl+D exits.\n";
    std::string line;
    while (std::getline(std::cin,line)) {
        lakakey::Engine engine(opts);
        std::string output;
        try {
            for (char32_t c:lakakey::utf8_decode(line)) {
                auto r=engine.feed(c); output+=r.commit;
            }
            output+=engine.commit().commit;
            std::cout<<"→ "<<output<<"\n";
        } catch (const std::exception& e) { std::cerr<<"Error: "<<e.what()<<"\n"; }
    }
}
