#ifdef NDEBUG
#undef NDEBUG // tests must assert in Release as well
#endif
#include "lakakey_session.h"
#include <cassert>
#include <iostream>
int main(){
 lakakey::InputSession s;
 for(char32_t c:std::u32string(U"tieengs"))s.character(c);
 auto r=s.finalize();
 std::cerr << "got=" << r.commit_utf8 << "\n"; assert(r.commit_utf8=="tiếng");
 s.focus_changed();
 s.secure_field(true);
 assert(!s.character(U'a').handled);
 s.secure_field(false);
 for(char32_t c:std::u32string(U"chao"))s.character(c);
 assert(s.finalize().commit_utf8=="chao");
 s.configure({lakakey::Scheme::Vni,lakakey::ToneStyle::Modern,true});
 for(char32_t c:std::u32string(U"tie6ng1"))s.character(c);
 assert(s.finalize().commit_utf8=="tiếng");
 std::cout<<"session tests passed\n";
}
