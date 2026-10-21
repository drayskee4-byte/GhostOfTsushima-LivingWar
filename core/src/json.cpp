#include "lw/json.hpp"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace lw {

namespace {
const Json kNull;
const std::string kEmpty;
const Json::Object kEmptyObj;
const Json::Array kEmptyArr;
}

void Json::detach() {
    if (a_ && a_.use_count() > 1) a_ = std::make_shared<Array>(*a_);
    if (o_ && o_.use_count() > 1) o_ = std::make_shared<Object>(*o_);
}

const std::string& Json::asString() const { return type_ == Type::String ? s_ : kEmpty; }

Json& Json::operator[](const std::string& key) {
    if (type_ != Type::Object) { *this = Json(Object{}); }
    detach();
    return (*o_)[key];
}

const Json& Json::operator[](const std::string& key) const {
    if (type_ != Type::Object) return kNull;
    auto it = o_->find(key);
    return it == o_->end() ? kNull : it->second;
}

bool Json::has(const std::string& key) const { return type_ == Type::Object && o_->count(key) > 0; }
const Json::Object& Json::items() const { return type_ == Type::Object ? *o_ : kEmptyObj; }

void Json::push(Json v) {
    if (type_ != Type::Array) { *this = Json(Array{}); }
    detach();
    a_->push_back(std::move(v));
}
size_t Json::size() const {
    if (type_ == Type::Array) return a_->size();
    if (type_ == Type::Object) return o_->size();
    return 0;
}
const Json& Json::at(size_t i) const { return (type_ == Type::Array && i < a_->size()) ? (*a_)[i] : kNull; }
const Json::Array& Json::elements() const { return type_ == Type::Array ? *a_ : kEmptyArr; }

// ---------------------------------------------------------------- writer
static void escapeTo(std::string& out, const std::string& s) {
    out += '"';
    for (unsigned char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) { char b[8]; std::snprintf(b, sizeof b, "\\u%04x", c); out += b; }
                else out += static_cast<char>(c);
        }
    }
    out += '"';
}

void Json::dumpTo(std::string& out, int indent, int depth) const {
    auto nl = [&](int d) {
        if (indent < 0) return;
        out += '\n';
        out.append(static_cast<size_t>(indent * d), ' ');
    };
    switch (type_) {
        case Type::Null: out += "null"; break;
        case Type::Bool: out += b_ ? "true" : "false"; break;
        case Type::Number: {
            if (!std::isfinite(n_)) { out += "0"; break; }
            char b[32];
            if (n_ == std::floor(n_) && std::fabs(n_) < 1e15) std::snprintf(b, sizeof b, "%lld", static_cast<long long>(n_));
            else std::snprintf(b, sizeof b, "%.6g", n_);
            out += b;
            break;
        }
        case Type::String: escapeTo(out, s_); break;
        case Type::Array: {
            out += '[';
            bool first = true;
            for (auto& v : *a_) {
                if (!first) out += ',';
                first = false;
                nl(depth + 1);
                v.dumpTo(out, indent, depth + 1);
            }
            if (!a_->empty()) nl(depth);
            out += ']';
            break;
        }
        case Type::Object: {
            out += '{';
            bool first = true;
            for (auto& kv : *o_) {
                if (!first) out += ',';
                first = false;
                nl(depth + 1);
                escapeTo(out, kv.first);
                out += indent < 0 ? ":" : ": ";
                kv.second.dumpTo(out, indent, depth + 1);
            }
            if (!o_->empty()) nl(depth);
            out += '}';
            break;
        }
    }
}

std::string Json::dump(int indent) const {
    std::string out;
    dumpTo(out, indent, 0);
    return out;
}

// ---------------------------------------------------------------- parser
namespace {
struct Parser {
    const std::string& s;
    size_t p = 0;
    [[noreturn]] void fail(const char* what) {
        throw std::runtime_error(std::string("JSON parse error at offset ") + std::to_string(p) + ": " + what);
    }
    void ws() { while (p < s.size() && (s[p] == ' ' || s[p] == '\n' || s[p] == '\r' || s[p] == '\t')) ++p; }
    bool lit(const char* w) {
        size_t n = std::strlen(w);
        if (s.compare(p, n, w) == 0) { p += n; return true; }
        return false;
    }
    static void utf8(std::string& o, unsigned cp) {
        if (cp < 0x80) o += static_cast<char>(cp);
        else if (cp < 0x800) { o += static_cast<char>(0xC0 | (cp >> 6)); o += static_cast<char>(0x80 | (cp & 0x3F)); }
        else if (cp < 0x10000) { o += static_cast<char>(0xE0 | (cp >> 12)); o += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); o += static_cast<char>(0x80 | (cp & 0x3F)); }
        else { o += static_cast<char>(0xF0 | (cp >> 18)); o += static_cast<char>(0x80 | ((cp >> 12) & 0x3F)); o += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); o += static_cast<char>(0x80 | (cp & 0x3F)); }
    }
    unsigned hex4() {
        if (p + 4 > s.size()) fail("bad \\u escape");
        unsigned v = 0;
        for (int i = 0; i < 4; ++i) {
            char c = s[p++]; v <<= 4;
            if (c >= '0' && c <= '9') v |= c - '0';
            else if (c >= 'a' && c <= 'f') v |= c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') v |= c - 'A' + 10;
            else fail("bad hex digit");
        }
        return v;
    }
    std::string str() {
        if (s[p] != '"') fail("expected string");
        ++p;
        std::string o;
        while (true) {
            if (p >= s.size()) fail("unterminated string");
            char c = s[p++];
            if (c == '"') break;
            if (c != '\\') { o += c; continue; }
            if (p >= s.size()) fail("bad escape");
            char e = s[p++];
            switch (e) {
                case '"': o += '"'; break; case '\\': o += '\\'; break; case '/': o += '/'; break;
                case 'b': o += '\b'; break; case 'f': o += '\f'; break; case 'n': o += '\n'; break;
                case 'r': o += '\r'; break; case 't': o += '\t'; break;
                case 'u': {
                    unsigned cp = hex4();
                    if (cp >= 0xD800 && cp < 0xDC00 && lit("\\u")) {
                        unsigned lo = hex4();
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                    }
                    utf8(o, cp);
                    break;
                }
                default: fail("bad escape");
            }
        }
        return o;
    }
    Json value(int depth) {
        if (depth > 200) fail("nesting too deep");
        ws();
        if (p >= s.size()) fail("unexpected end");
        char c = s[p];
        if (c == '{') {
            ++p;
            Json::Object o;
            ws();
            if (p < s.size() && s[p] == '}') { ++p; return Json(std::move(o)); }
            while (true) {
                ws();
                std::string k = str();
                ws();
                if (p >= s.size() || s[p] != ':') fail("expected ':'");
                ++p;
                o[k] = value(depth + 1);
                ws();
                if (p < s.size() && s[p] == ',') { ++p; continue; }
                if (p < s.size() && s[p] == '}') { ++p; break; }
                fail("expected ',' or '}'");
            }
            return Json(std::move(o));
        }
        if (c == '[') {
            ++p;
            Json::Array a;
            ws();
            if (p < s.size() && s[p] == ']') { ++p; return Json(std::move(a)); }
            while (true) {
                a.push_back(value(depth + 1));
                ws();
                if (p < s.size() && s[p] == ',') { ++p; continue; }
                if (p < s.size() && s[p] == ']') { ++p; break; }
                fail("expected ',' or ']'");
            }
            return Json(std::move(a));
        }
        if (c == '"') return Json(str());
        if (lit("true")) return Json(true);
        if (lit("false")) return Json(false);
        if (lit("null")) return Json();
        size_t start = p;
        if (s[p] == '-') ++p;
        while (p < s.size() && (isdigit(static_cast<unsigned char>(s[p])) || s[p] == '.' || s[p] == 'e' || s[p] == 'E' || s[p] == '+' || s[p] == '-')) ++p;
        if (start == p) fail("unexpected character");
        try { return Json(std::stod(s.substr(start, p - start))); }
        catch (...) { fail("bad number"); }
    }
};
} // namespace

Json Json::parse(const std::string& text) {
    Parser ps{text};
    Json v = ps.value(0);
    ps.ws();
    if (ps.p != text.size()) ps.fail("trailing characters");
    return v;
}

} // namespace lw
