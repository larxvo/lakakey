#include "lakakey_engine.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace lakakey;
int total=0;
void expect(const std::string& input, const std::string& want, Settings s={}) {
    ++total;
    Engine e(s);
    std::string have;
    for (auto cp:utf8_decode(input)) { auto r=e.feed(cp); have+=r.commit; }
    have+=e.commit().commit;
    if(have!=want) { std::cerr<<"FAIL '"<<input<<"': expected '"<<want<<"' got '"<<have<<"'\n"; std::exit(1); }
}
int main() {
    expect("tieengs","tiếng");
    expect("Tieengs","Tiếng");
    expect("TIEENGS","TIẾNG");
    expect("chaof","chào");
    expect("xin chaof","xin chào");
    expect("Vieetj Nam","Việt Nam");
    expect("ddaays","đấy");
    expect("dd","đ");
    expect("DD","Đ");
    expect("aa","â");
    expect("aw","ă");
    expect("ee","ê");
    expect("oo","ô");
    expect("ow","ơ");
    expect("uw","ư");
    expect("uow","ươ");
    expect("nguowif","người");
    expect("duongwf", "dường");
    expect("hoaf","hòa");
    expect("khoer","khỏe");
    expect("thuyr","thủy");
    expect("hoaf","hoà",{Scheme::Telex,ToneStyle::Traditional,true});
    expect("khoer","khoẻ",{Scheme::Telex,ToneStyle::Traditional,true});
    expect("thuyr","thuỷ",{Scheme::Telex,ToneStyle::Traditional,true});
    expect("quas","quá");
    expect("gias","giá");
    expect("gif","gì");
    expect("ngoaif","ngoài");
    expect("cuar","của");
    expect("dduowcj","được");
    expect("chuyeens","chuyến");
    expect("hoangf","hoàng");
    expect("phaanf meemf","phần mềm");
    expect("Carm own","Cảm ơn");
    expect("ngoaij","ngoại");
    expect("thuowngr","thưởng");
    expect("sarn phaamr","sản phẩm");
    expect("kieemr tra","kiểm tra");
    expect("phuwowng","phương");
    expect("suwcs khoer","sức khỏe");
    expect("quyeenf","quyền");
    expect("tieesng","tiếng");
    expect("thoong tin","thông tin");
    expect("laayf","lầy");
    expect("aaaa","aâ"); // sequential escape then remap at end
    expect("ass","as"); // repeated tone key cancels tone and becomes literal
    expect("a1","á",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a2","à",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a3","ả",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a4","ã",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a5","ạ",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a6","â",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a8","ă",{Scheme::Vni,ToneStyle::Modern,true});
    expect("e6","ê",{Scheme::Vni,ToneStyle::Modern,true});
    expect("o6","ô",{Scheme::Vni,ToneStyle::Modern,true});
    expect("o7","ơ",{Scheme::Vni,ToneStyle::Modern,true});
    expect("u7","ư",{Scheme::Vni,ToneStyle::Modern,true});
    expect("uo7","ươ",{Scheme::Vni,ToneStyle::Modern,true});
    expect("d9","đ",{Scheme::Vni,ToneStyle::Modern,true});
    expect("a1a2", "àa", {Scheme::Vni,ToneStyle::Modern,true});
    expect("hello", "hello", {Scheme::Plain,ToneStyle::Modern,true});
    Engine e;
    if (e.backspace().consumed) { std::cerr<<"FAIL empty backspace must pass through\n"; return 1; }
    for (auto c:utf8_decode("tieengs")) e.feed(c);
    if (e.preedit()!="tiếng") { std::cerr<<"FAIL live preedit\n"; return 1; }
    e.backspace();
    if (e.preedit()!="tiêng") { std::cerr<<"FAIL removing nonprinting tone key\n"; return 1; }
    e.feed(U's');
    if (e.feed(U' ').commit!="tiếng ") { std::cerr<<"FAIL commit with boundary\n"; return 1; }
    if (!e.preedit().empty()) { std::cerr<<"FAIL reset after commit\n"; return 1; }
    try { (void)utf8_decode("\xC0\xAF"); std::cerr<<"FAIL invalid UTF8 accepted\n"; return 1; }
    catch (const std::invalid_argument&) {}
    std::cout<<"PASS "<<total<<" mapping tests + state/UTF-8 checks\n";
}
