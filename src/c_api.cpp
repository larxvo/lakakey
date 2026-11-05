#include "lakakey_c.h"
#include "lakakey_engine.h"
#include <new>
#include <string>
struct lk_handle {
    lakakey::Engine engine;
    std::string commit, preedit;
    explicit lk_handle(lakakey::Settings s):engine(s){}
};
namespace {
lk_result store(lk_handle* h, lakakey::Result r) {
    if (!h) return {"", "", 0};
    h->commit=std::move(r.commit); h->preedit=std::move(r.preedit);
    return {h->commit.c_str(),h->preedit.c_str(),r.consumed?1:0};
}
lakakey::Settings settings(int s,int t,int en) {
    return {s==1?lakakey::Scheme::Vni:s==2?lakakey::Scheme::Plain:lakakey::Scheme::Telex,
            t==1?lakakey::ToneStyle::Traditional:lakakey::ToneStyle::Modern,en!=0};
}
}
extern "C" {
lk_handle* lk_create(int s,int t) { try { return new lk_handle(settings(s,t,1)); } catch (...) { return nullptr; } }
void lk_destroy(lk_handle* h) { delete h; }
lk_result lk_feed(lk_handle* h,uint32_t cp) {
    if (!h || cp>0x10ffff || (cp>=0xd800&&cp<=0xdfff)) return {"","",0};
    try { return store(h,h->engine.feed(static_cast<char32_t>(cp))); } catch (...) { return {"","",0}; }
}
lk_result lk_backspace(lk_handle* h) { if (!h) return {"","",0}; try { return store(h,h->engine.backspace()); } catch (...) { return {"","",0}; } }
lk_result lk_commit(lk_handle* h) { if (!h) return {"","",0}; try { return store(h,h->engine.commit()); } catch (...) { return {"","",0}; } }
void lk_reset(lk_handle* h) { if(h) h->engine.reset(); }
void lk_configure(lk_handle* h,int s,int t,int en) { if(h) h->engine.set_settings(settings(s,t,en)); }
}
