#include "lakakey_engine.h"
#include "lakakey_session.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace lakakey;
static void require(bool ok,const char* why){if(!ok){std::cerr<<"FAIL "<<why<<"\n";std::exit(1);}}
static void unicode_contract(){
 const auto value=std::u32string(U"Tiếng Việt — Hà Nội 👋");
 require(utf8_decode(utf8_encode(value))==value,"Unicode roundtrip");
 for(const std::string invalid:{
  std::string("\xC0\xAF",2),std::string("\xED\xA0\x80",3),
  std::string("\xF4\x90\x80\x80",4),std::string("\xE2\x82",2),
  std::string("\x80",1)}) {
  bool rejected=false;
  try{(void)utf8_decode(invalid);}
  catch(const std::invalid_argument&){rejected=true;}
  require(rejected,"Malformed UTF-8 accepted");
 }
 bool rejected=false;
 try{(void)utf8_encode(std::u32string(1,static_cast<char32_t>(0xd800)));}
 catch(const std::invalid_argument&){rejected=true;}
 require(rejected,"Surrogate scalar encoded");
}
static void disabled_input_contract(){
 Settings off{};off.enabled=false;
 Engine engine(off);
 require(!engine.feed(U'a').consumed,"Disabled core consumed key");
 require(engine.preedit().empty(),"Disabled core retained preedit");
 require(!engine.backspace().consumed,"Disabled core consumed erase");
 require(!engine.commit().consumed,"Disabled core consumed commit");
 InputSession session(off);
 require(!session.character(U'a').handled,"Disabled session consumed key");
 require(!session.erase().handled,"Disabled session consumed erase");
 require(!session.finalize().handled,"Disabled session consumed commit");
 session.configure({Scheme::Telex,ToneStyle::Modern,true});
 for(char32_t ch:std::u32string(U"tieengs"))session.character(ch);
 require(session.finalize().commit_utf8=="tiếng","Reenabled session broken");
}
static void secure_and_focus_contract(){
 InputSession session;
 for(char32_t ch:std::u32string(U"tieengs"))session.character(ch);
 session.focus_changed();
 require(session.finalize().commit_utf8.empty(),"Old composition leaked after focus");
 session.secure_field(true);
 require(!session.character(U'x').handled,"Secure field consumed key");
 require(!session.erase().handled,"Secure field consumed erase");
 require(!session.finalize().handled,"Secure field consumed commit");
 session.secure_field(false);
 for(char32_t ch:std::u32string(U"chaof"))session.character(ch);
 require(session.finalize().commit_utf8=="chào","Focus restore broken");
}
static void stress_contract(){
 Engine engine;
 for(int word=0;word<10000;++word){
  for(char32_t ch:std::u32string(U"tieengs"))engine.feed(ch);
  require(engine.commit().commit=="tiếng","Stress conversion mismatch");
 }
 require(engine.preedit().empty(),"Stale preedit");
 require(!engine.backspace().consumed,"Empty backspace swallowed");
}
int main(){
 unicode_contract(); disabled_input_contract(); secure_and_focus_contract();stress_contract();
 std::cout<<"PASS Unicode / mode bypass / secure field / focus / stress\n";
}
