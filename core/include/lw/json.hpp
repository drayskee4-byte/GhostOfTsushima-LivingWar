// Living War - minimal JSON value, parser and writer (no external deps).
#pragma once
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace lw {

class Json {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };
    using Array = std::vector<Json>;
    using Object = std::map<std::string, Json>;

    Json() = default;
    Json(std::nullptr_t) {}
    Json(bool b) : type_(Type::Bool), b_(b) {}
    Json(int v) : type_(Type::Number), n_(v) {}
    Json(int64_t v) : type_(Type::Number), n_(static_cast<double>(v)) {}
    Json(uint32_t v) : type_(Type::Number), n_(v) {}
    Json(double v) : type_(Type::Number), n_(v) {}
    Json(const char* s) : type_(Type::String), s_(s) {}
    Json(std::string s) : type_(Type::String), s_(std::move(s)) {}
    Json(Array a) : type_(Type::Array), a_(std::make_shared<Array>(std::move(a))) {}
    Json(Object o) : type_(Type::Object), o_(std::make_shared<Object>(std::move(o))) {}

    static Json array() { return Json(Array{}); }
    static Json object() { return Json(Object{}); }

    Type type() const { return type_; }
    bool isNull() const { return type_ == Type::Null; }
    bool isObject() const { return type_ == Type::Object; }
    bool isArray() const { return type_ == Type::Array; }
    bool isNumber() const { return type_ == Type::Number; }
    bool isString() const { return type_ == Type::String; }
    bool isBool() const { return type_ == Type::Bool; }

    bool asBool(bool def = false) const { return type_ == Type::Bool ? b_ : def; }
    double asNumber(double def = 0) const { return type_ == Type::Number ? n_ : def; }
    int asInt(int def = 0) const { return type_ == Type::Number ? static_cast<int>(n_) : def; }
    int64_t asInt64(int64_t def = 0) const { return type_ == Type::Number ? static_cast<int64_t>(n_) : def; }
    const std::string& asString() const;
    std::string asString(const std::string& def) const { return type_ == Type::String ? s_ : def; }

    // Object access
    Json& operator[](const std::string& key);           // creates object/key
    const Json& operator[](const std::string& key) const; // returns null if missing
    bool has(const std::string& key) const;
    const Object& items() const;
    // Array access
    void push(Json v);
    size_t size() const;
    const Json& at(size_t i) const;
    const Array& elements() const;

    // convenience getters with defaults
    double num(const std::string& k, double def = 0) const { return (*this)[k].asNumber(def); }
    int i(const std::string& k, int def = 0) const { return (*this)[k].asInt(def); }
    bool flag(const std::string& k, bool def = false) const { return (*this)[k].asBool(def); }
    std::string str(const std::string& k, const std::string& def = "") const { return (*this)[k].asString(def); }

    std::string dump(int indent = -1) const;
    static Json parse(const std::string& text); // throws std::runtime_error

private:
    void dumpTo(std::string& out, int indent, int depth) const;
    void detach(); // copy-on-write for shared containers

    Type type_ = Type::Null;
    bool b_ = false;
    double n_ = 0;
    std::string s_;
    std::shared_ptr<Array> a_;
    std::shared_ptr<Object> o_;
};

} // namespace lw
