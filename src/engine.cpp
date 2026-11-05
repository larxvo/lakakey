#include "lakakey_engine.h"
#include <algorithm>
#include <stdexcept>

namespace lakakey {
namespace {
constexpr char32_t vow[12][6] = {
    {U'a', U'á', U'à', U'ả', U'ã', U'ạ'},
    {U'ă', U'ắ', U'ằ', U'ẳ', U'ẵ', U'ặ'},
    {U'â', U'ấ', U'ầ', U'ẩ', U'ẫ', U'ậ'},
    {U'e', U'é', U'è', U'ẻ', U'ẽ', U'ẹ'},
    {U'ê', U'ế', U'ề', U'ể', U'ễ', U'ệ'},
    {U'i', U'í', U'ì', U'ỉ', U'ĩ', U'ị'},
    {U'o', U'ó', U'ò', U'ỏ', U'õ', U'ọ'},
    {U'ô', U'ố', U'ồ', U'ổ', U'ỗ', U'ộ'},
    {U'ơ', U'ớ', U'ờ', U'ở', U'ỡ', U'ợ'},
    {U'u', U'ú', U'ù', U'ủ', U'ũ', U'ụ'},
    {U'ư', U'ứ', U'ừ', U'ử', U'ữ', U'ự'},
    {U'y', U'ý', U'ỳ', U'ỷ', U'ỹ', U'ỵ'}
};
struct VInfo { int group = -1; int tone = 0; bool upper = false; };
VInfo vowel_info(char32_t c) {
    for (int i = 0; i < 12; ++i) for (int j = 0; j < 6; ++j) {
        if (c == vow[i][j]) return {i, j, false};
        // Vietnamese uppercase precomposed letters map explicitly.
    }
    constexpr char32_t uppercase[12][6] = {
        {U'A',U'Á',U'À',U'Ả',U'Ã',U'Ạ'}, {U'Ă',U'Ắ',U'Ằ',U'Ẳ',U'Ẵ',U'Ặ'},
        {U'Â',U'Ấ',U'Ầ',U'Ẩ',U'Ẫ',U'Ậ'}, {U'E',U'É',U'È',U'Ẻ',U'Ẽ',U'Ẹ'},
        {U'Ê',U'Ế',U'Ề',U'Ể',U'Ễ',U'Ệ'}, {U'I',U'Í',U'Ì',U'Ỉ',U'Ĩ',U'Ị'},
        {U'O',U'Ó',U'Ò',U'Ỏ',U'Õ',U'Ọ'}, {U'Ô',U'Ố',U'Ồ',U'Ổ',U'Ỗ',U'Ộ'},
        {U'Ơ',U'Ớ',U'Ờ',U'Ở',U'Ỡ',U'Ợ'}, {U'U',U'Ú',U'Ù',U'Ủ',U'Ũ',U'Ụ'},
        {U'Ư',U'Ứ',U'Ừ',U'Ử',U'Ữ',U'Ự'}, {U'Y',U'Ý',U'Ỳ',U'Ỷ',U'Ỹ',U'Ỵ'}
    };
    for (int i = 0; i < 12; ++i) for (int j = 0; j < 6; ++j)
        if (c == uppercase[i][j]) return {i, j, true};
    return {};
}
char32_t vowel_at(int group, int tone, bool upper) {
    if (group < 0 || group >= 12 || tone < 0 || tone > 5) return U'?';
    if (!upper) return vow[group][tone];
    constexpr char32_t uppercase[12][6] = {
        {U'A',U'Á',U'À',U'Ả',U'Ã',U'Ạ'}, {U'Ă',U'Ắ',U'Ằ',U'Ẳ',U'Ẵ',U'Ặ'},
        {U'Â',U'Ấ',U'Ầ',U'Ẩ',U'Ẫ',U'Ậ'}, {U'E',U'É',U'È',U'Ẻ',U'Ẽ',U'Ẹ'},
        {U'Ê',U'Ế',U'Ề',U'Ể',U'Ễ',U'Ệ'}, {U'I',U'Í',U'Ì',U'Ỉ',U'Ĩ',U'Ị'},
        {U'O',U'Ó',U'Ò',U'Ỏ',U'Õ',U'Ọ'}, {U'Ô',U'Ố',U'Ồ',U'Ổ',U'Ỗ',U'Ộ'},
        {U'Ơ',U'Ớ',U'Ờ',U'Ở',U'Ỡ',U'Ợ'}, {U'U',U'Ú',U'Ù',U'Ủ',U'Ũ',U'Ụ'},
        {U'Ư',U'Ứ',U'Ừ',U'Ử',U'Ữ',U'Ự'}, {U'Y',U'Ý',U'Ỳ',U'Ỷ',U'Ỹ',U'Ỵ'}
    };
    return uppercase[group][tone];
}
char32_t lower_ascii(char32_t c) { return c >= U'A' && c <= U'Z' ? c + 32 : c; }
bool is_letter(char32_t c) {
    char32_t lo = lower_ascii(c);
    return (lo >= U'a' && lo <= U'z') || vowel_info(c).group >= 0 || c == U'đ' || c == U'Đ';
}
int telex_tone(char32_t c) {
    switch (lower_ascii(c)) {
        case U's': return 1; case U'f': return 2; case U'r': return 3;
        case U'x': return 4; case U'j': return 5; default: return 0;
    }
}
int vni_tone(char32_t c) {
    return c >= U'1' && c <= U'5' ? static_cast<int>(c-U'0') : 0;
}
}

std::string utf8_encode(const std::u32string& input) {
    std::string out;
    for (char32_t cp: input) {
        if (cp <= 0x7f) out.push_back(static_cast<char>(cp));
        else if (cp <= 0x7ff) {
            out.push_back(static_cast<char>(0xc0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
        } else if (cp <= 0xffff && (cp < 0xd800 || cp > 0xdfff)) {
            out.push_back(static_cast<char>(0xe0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
        } else if (cp <= 0x10ffff) {
            out.push_back(static_cast<char>(0xf0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3f)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
        } else throw std::invalid_argument("Invalid Unicode codepoint");
    }
    return out;
}
std::u32string utf8_decode(const std::string& in) {
    std::u32string out;
    for (std::size_t i=0; i < in.size();) {
        auto b = static_cast<unsigned char>(in[i]);
        std::uint32_t cp=0;
        unsigned need=0;
        std::uint32_t min_cp=0;
        if (b < 0x80) { cp=b; need=0; min_cp=0; }
        else if ((b&0xe0)==0xc0) { cp=b&0x1f; need=1; min_cp=0x80; }
        else if ((b&0xf0)==0xe0) { cp=b&0x0f; need=2; min_cp=0x800; }
        else if ((b&0xf8)==0xf0) { cp=b&0x07; need=3; min_cp=0x10000; }
        else throw std::invalid_argument("Malformed UTF-8 lead byte");
        if (i+need>=in.size()) throw std::invalid_argument("Truncated UTF-8");
        for (unsigned j=1; j<=need; ++j) {
            auto cc=static_cast<unsigned char>(in[i+j]);
            if ((cc&0xc0)!=0x80) throw std::invalid_argument("Malformed UTF-8 continuation");
            cp=(cp<<6)|(cc&0x3f);
        }
        if (cp<min_cp || cp>0x10ffff || (cp>=0xd800 && cp<=0xdfff))
            throw std::invalid_argument("Invalid UTF-8 scalar");
        out.push_back(static_cast<char32_t>(cp)); i+=need+1;
    }
    return out;
}

Engine::Engine(Settings settings) : settings_(settings) {}
void Engine::reset() { letters_.clear(); raw_events_.clear(); tone_=0; }
void Engine::set_settings(Settings settings) { reset(); settings_=settings; }
bool Engine::is_word_char(char32_t c) { return is_letter(c) || (c >= U'0' && c <= U'9'); }
bool Engine::has_vowel() const {
    for (auto c: letters_) if (vowel_info(c).group >= 0) return true;
    return false;
}
void Engine::process_letter(char32_t c) {
    if (!settings_.enabled || settings_.scheme==Scheme::Plain) { letters_.push_back(c); return; }
    bool telex = settings_.scheme==Scheme::Telex;
    int newtone = telex ? telex_tone(c) : vni_tone(c);
    if (newtone && has_vowel()) {
        if (tone_==newtone) { tone_=0; letters_.push_back(c); }
        else tone_=newtone;
        return;
    }
    if (telex && lower_ascii(c)==U'z') {
        if (tone_) { tone_=0; return; }
        bool cleared=false;
        for (auto& l: letters_) {
            auto v=vowel_info(l);
            if (v.group < 0) continue;
            int plain = v.group==1||v.group==2?0:v.group==4?3:
                        v.group==7||v.group==8?6:v.group==10?9:v.group;
            if (plain!=v.group || v.tone!=0) { l=vowel_at(plain,0,v.upper); cleared=true; }
        }
        if (cleared) return;
    }
    if (letters_.empty()) { letters_.push_back(c); return; }
    auto& last = letters_.back();
    auto v = vowel_info(last);
    if (telex && lower_ascii(c)==U'd' && (last==U'd'||last==U'D')) {
        last=(last==U'D')?U'Đ':U'đ'; return;
    }
    if (telex && lower_ascii(c)==U'd' && (last==U'đ'||last==U'Đ')) {
        last=(last==U'Đ')?U'D':U'd'; letters_.push_back(c); return;
    }
    int mark_group=-1;
    if (telex) {
        if (v.group==0 && lower_ascii(c)==U'a') mark_group=2;
        else if (v.group==3 && lower_ascii(c)==U'e') mark_group=4;
        else if (v.group==6 && lower_ascii(c)==U'o') mark_group=7;
        else if (v.group==0 && lower_ascii(c)==U'w') mark_group=1;
        else if (v.group==6 && lower_ascii(c)==U'w') mark_group=8;
        else if (v.group==9 && lower_ascii(c)==U'w') mark_group=10;
    } else {
        if (c==U'6' && v.group==0) mark_group=2;
        else if (c==U'6' && v.group==3) mark_group=4;
        else if (c==U'6' && v.group==6) mark_group=7;
        else if (c==U'7' && v.group==6) mark_group=8;
        else if (c==U'7' && v.group==9) mark_group=10;
        else if (c==U'8' && v.group==0) mark_group=1;
        else if (c==U'9' && (last==U'd'||last==U'D')) {
            last=(last==U'D')?U'Đ':U'đ'; return;
        }
    }
    // 'uow' / 'uo7' yields 'ươ', e.g. 'nguowif' -> 'người'.
    if (v.group==6 && (telex ? lower_ascii(c)==U'w' : c==U'7') && letters_.size()>=2) {
        auto prev=vowel_info(letters_[letters_.size()-2]);
        if (prev.group==9) {
            letters_[letters_.size()-2]=vowel_at(10,0,prev.upper);
            mark_group=8;
        }
    }
    if (mark_group>=0) { last=vowel_at(mark_group,0,v.upper); return; }
    // Support typing the horn key after trailing consonants: 'duongwf' -> 'dường'.
    if (mark_group<0 && ((telex && lower_ascii(c)==U'w') || (!telex && c==U'7'))) {
        for (std::size_t i=letters_.size(); i>0; --i) {
            auto vi=vowel_info(letters_[i-1]);
            int target = vi.group==6?8:vi.group==9?10:vi.group==0&&telex?1:-1;
            if (target<0) continue;
            if (vi.group==6 && i>=2) {
                auto pu=vowel_info(letters_[i-2]);
                if (pu.group==9) letters_[i-2]=vowel_at(10,0,pu.upper);
            }
            letters_[i-1]=vowel_at(target,0,vi.upper);
            return;
        }
    }

    // Repeated mark key escapes a transformation: 'aaa' -> 'aa', 'aww' -> 'aw'.
    if (telex && (lower_ascii(c)==U'a'||lower_ascii(c)==U'e'||lower_ascii(c)==U'o'||lower_ascii(c)==U'w')) {
        int plain=-1;
        if (lower_ascii(c)==U'a' && v.group==2) plain=0;
        else if (lower_ascii(c)==U'e' && v.group==4) plain=3;
        else if (lower_ascii(c)==U'o' && v.group==7) plain=6;
        else if (lower_ascii(c)==U'w' && (v.group==1||v.group==8||v.group==10))
            plain=v.group==1?0:v.group==8?6:9;
        if (plain!=-1) {
            last=vowel_at(plain,0,v.upper);
            letters_.push_back(c);
            return;
        }
    }
    letters_.push_back(c);
}
std::u32string Engine::render_u32() const {
    std::u32string result=letters_;
    if (!tone_) return result;
    std::vector<std::size_t> vowels;
    for (std::size_t i=0;i<result.size();++i) if (vowel_info(result[i]).group>=0) vowels.push_back(i);
    if (vowels.empty()) return result;
    // Initial 'qu' and 'gi': u/i are glides if another nucleus vowel follows.
    if (vowels.size()>1 && result.size()>2) {
        if (lower_ascii(result[0])==U'q' && lower_ascii(result[1])==U'u')
            vowels.erase(std::remove(vowels.begin(),vowels.end(),std::size_t(1)),vowels.end());
        else if (lower_ascii(result[0])==U'g' && lower_ascii(result[1])==U'i')
            vowels.erase(std::remove(vowels.begin(),vowels.end(),std::size_t(1)),vowels.end());
    }
    if (vowels.empty()) return result;
    std::size_t target=vowels.front();
    // Vowels with a base diacritic (â, ă, ê, ô, ơ, ư) take precedence.
    bool found_marked=false;
    for (auto idx: vowels) {
        int g=vowel_info(result[idx]).group;
        if (g==1||g==2||g==4||g==7||g==8||g==10) {
            target=idx; found_marked=true;
        }
    }
    if (!found_marked) {
        bool coda=(vowels.back()+1 < result.size());
        if (vowels.size()==2) {
            target=coda ? vowels.back() : vowels.front();
            if (!coda && settings_.tone_style==ToneStyle::Traditional) {
                int a=vowel_info(result[vowels[0]]).group;
                int b=vowel_info(result[vowels[1]]).group;
                if ((a==6 && (b==0||b==3)) || (a==9 && b==11)) target=vowels.back();
            }
        } else if (vowels.size()>=3) target = coda ? vowels.back() : vowels[1];
    }
    auto v=vowel_info(result[target]);
    result[target]=vowel_at(v.group,tone_,v.upper);
    return result;
}
std::string Engine::preedit() const { return utf8_encode(render_u32()); }
Result Engine::feed(char32_t codepoint) {
    if (!is_word_char(codepoint)) {
        Result r{preedit()+utf8_encode(std::u32string(1,codepoint)),"",true};
        reset(); return r;
    }
    if (letters_.size()>=128) {
        Result r{preedit(),"",true}; reset();
        raw_events_.push_back(codepoint);
        process_letter(codepoint); r.preedit=preedit(); return r;
    }
    raw_events_.push_back(codepoint);
    process_letter(codepoint);
    return {"",preedit(),true};
}
Result Engine::backspace() {
    if (raw_events_.empty()) return {"","",false};
    raw_events_.pop_back();
    letters_.clear(); tone_=0;
    for (auto cp: raw_events_) process_letter(cp);
    return {"",preedit(),true};
}
Result Engine::commit() { Result r{preedit(),"",true}; reset(); return r; }
}
