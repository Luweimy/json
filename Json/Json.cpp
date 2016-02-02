//
//  Json.cpp
//  Json
//
//  Created by luwei on 16/2/1.
//  Copyright (c) 2016年 luwei. All rights reserved.
//

#include "Json.h"

using namespace json;

//
// [JsonParser]
//

class JsonParser {
public:
    static void JumpWhiteSpace(const std::string &str, size_t &offset);
    static JSON ParseNext(const std::string &str, size_t &offset);
    static JSON ParseObject(const std::string &str, size_t &offset);
    static JSON ParseArray(const std::string &str, size_t &offset);
    static JSON ParseString(const std::string &str, size_t &offset);
    static JSON ParseNumber(const std::string &str, size_t &offset);
    static JSON ParseBoolean(const std::string &str, size_t &offset);
    static JSON ParseNull(const std::string &str, size_t &offset);
};

void JsonParser::JsonParser::JumpWhiteSpace(const std::string &s, size_t &offset)
{
    while(isspace(s[offset]))
        ++offset;
}

JSON JsonParser::JsonParser::ParseNext(const std::string &s, size_t &offset) {
    char value;
    JsonParser::JumpWhiteSpace(s, offset);
    value = s[offset];
    switch (value) {
        case '[' : return std::move(JsonParser::ParseArray(s, offset));
        case '{' : return std::move(JsonParser::ParseObject(s, offset));
        case '\"': return std::move(JsonParser::ParseString(s, offset));
        case 't' :
        case 'f' : return std::move(JsonParser::ParseBoolean(s, offset));
        case 'n' : return std::move(JsonParser::ParseNull(s, offset));
        default  : {
            if ((value <= '9' && value >= '0' ) || value == '-')
                return std::move(ParseNumber(s, offset));
        }
    }
    printf("ERROR: Parse: Unknown starting character '%c'\n", value);
    return JSON();
}

JSON JsonParser::JsonParser::ParseObject(const std::string &s, size_t &offset)
{
    JSON object = JSON::Make(JSON::Type::Object);
    
    ++offset;
    JsonParser::JumpWhiteSpace(s, offset);
    if (s[offset] == '}') {
        ++offset; return std::move(object);
    }
    
    while (true) {
        JSON key = JsonParser::ParseNext(s, offset);
        JsonParser::JumpWhiteSpace(s, offset);
        if (s[offset] != ':') {
            printf("Error: Object: Expected colon, found '%c'\n", s[offset]); break;
        }
        JsonParser::JumpWhiteSpace(s, ++offset);
        JSON value = JsonParser::ParseNext(s, offset);
        object[key.ToString()] = value;
        
        JsonParser::JumpWhiteSpace(s, offset);
        if (s[offset] == ',') {
            ++offset; continue;
        }
        else if(s[offset] == '}') {
            ++offset; break;
        }
        else {
            printf("ERROR: Object: Expected comma, found '%c'\n", s[offset]); break;
        }
    }
    
    return std::move(object);
}

JSON JsonParser::JsonParser::ParseArray(const std::string &s, size_t &offset) {
    JSON array = JSON::Make(JSON::Type::Array);
    unsigned index = 0;
    
    ++offset;
    JsonParser::JumpWhiteSpace(s, offset);
    if (s[offset] == ']') {
        ++offset; return std::move(array);
    }
    
    while (true) {
        array[index++] = JsonParser::ParseNext(s, offset);
        JsonParser::JumpWhiteSpace(s, offset);
        
        if (s[offset] == ',') {
            ++offset; continue;
        }
        else if (s[offset] == ']') {
            ++offset; break;
        }
        else {
            printf("ERROR: Array: Expected ',' or ']', found '%c'\n", s[offset]);
            return std::move(JSON::Make(JSON::Type::Array));
        }
    }
    
    return std::move(array);
}

JSON JsonParser::JsonParser::ParseString(const std::string &s, size_t &offset) {
    std::string val;
    for (char c = s[++offset]; c != '\"'; c = s[++offset]) {
        if (c == '\\') {
            switch (s[++offset]) {
                case '\"': val += '\"'; break;
                case '\\': val += '\\'; break;
                case '/' : val += '/' ; break;
                case 'b' : val += '\b'; break;
                case 'f' : val += '\f'; break;
                case 'n' : val += '\n'; break;
                case 'r' : val += '\r'; break;
                case 't' : val += '\t'; break;
                case 'u' : {
                    val += "\\u" ;
                    for (unsigned i = 1; i <= 4; ++i) {
                        c = s[offset+i];
                        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                            val += c;
                        }
                        else {
                            printf("ERROR: String: Expected hex character in unicode escape, found '%c'\n", s[offset]);
                            return std::move(JSON::Make(JSON::Type::String));
                        }
                    }
                    offset += 4;
                    break;
                }
                default  : val += '\\'; break;
            }
        }
        else {
            val += c;
        }
    }
    ++offset;
    return std::move(JSON(val));
}

JSON JsonParser::ParseNumber(const std::string &s, size_t &offset) {
    JSON number;
    std::string val, expstr;
    bool isdouble = false;
    long exp = 0;
    char c;
    
    while (true) {
        c = s[offset++];
        if ((c == '-') || (c >= '0' && c <= '9')) {
            val += c;
        }
        else if (c == '.') {
            val += c;
            isdouble = true;
        }
        else {
            break;
        }
    }
    if (c == 'E' || c == 'e') {
        c = s[offset++];
        if (c == '-') {
            ++offset; expstr += '-';
        }
        while (true) {
            c = s[ offset++ ];
            if (c >= '0' && c <= '9') {
                expstr += c;
            }
            else if( !isspace( c ) && c != ',' && c != ']' && c != '}' ) {
                printf("ERROR: Number: Expected a number for exponent, found '%c'\n", c);
                return std::move(JSON::Make(JSON::Type::Null));
            }
            else {
                break;
            }
        }
        exp = std::stol(expstr);
    }
    else if (!isspace( c ) && c != ',' && c != ']' && c != '}') {
        printf("ERROR: Number: unexpected character '%c'\n", c);
        return std::move(JSON::Make(JSON::Type::Null));
    }
    --offset;
    
    if (isdouble) {
        number = std::stod(val) * std::pow(10, exp);
    }
    else {
        if(!expstr.empty()) {
            number = std::stol(val) * std::pow(10, exp);
        }
        else {
            number = std::stol(val);
        }
    }
    return std::move(number);
}

JSON JsonParser::JsonParser::ParseBoolean(const std::string &s, size_t &offset) {
    JSON boolean;
    if (s.substr(offset, 4) == "true") {
        boolean = true;
    }
    else if (s.substr(offset, 5) == "false") {
        boolean = false;
    }
    else {
        printf("ERROR: Bool: Expected 'true' or 'false', found '%s'\n", s.substr(offset, 5).c_str());
        return std::move(JSON::Make(JSON::Type::Null));
    }
    offset += (boolean.ToBoolean() ? 4 : 5);
    return std::move(boolean);
}

JSON JsonParser::JsonParser::ParseNull( const std::string &str, size_t &offset ) {
    JSON null;
    if( str.substr( offset, 4 ) != "null" ) {
        printf("ERROR: Null: Expected 'null', found '%s'\n", str.substr(offset, 4).c_str());
        return std::move(JSON::Make(JSON::Type::Null));
    }
    offset += 4;
    return std::move(null);
}


//
// [JSON]
//

std::string json_escape( const std::string &str )
{
    std::string s;
    size_t l = str.length();
    for(unsigned i = 0; i < l; i++) {
        switch(str[i]) {
        case '\"': s += "\\\""; break;
        case '\\': s += "\\\\"; break;
        case '\b': s += "\\b";  break;
        case '\f': s += "\\f";  break;
        case '\n': s += "\\n";  break;
        case '\r': s += "\\r";  break;
        case '\t': s += "\\t";  break;
        default  : s += str[i]; break;
        }
    }
    return std::move(s);
}

std::string object_tojson(const JSON::JsonData &data)
{
    std::string s("{");
    for (auto i = data.object->begin(), e = data.object->end(); i != e; i++) {
        s += "\"" + i->first + "\":" + i->second.ToJson();
        if ( std::next(i) != e) {
            s += ',';
        }
    }
    s += "}";
    return std::move(s);
}

std::string object_toprettyjson(const JSON::JsonData &data, int depth)
{
    std::string indent1(JSON::_TAB_INDENT*depth++, ' ');
    std::string indent2(JSON::_TAB_INDENT*depth, ' ');
    
    if (data.object->size() == 0) {
        return "{}";
    }
    std::string s = "{\n";
    for (auto i = data.object->begin(), e = data.object->end(); i != e; i++) {
        s += indent2 + "\"" + i->first + "\":" + i->second.ToPrettyJson(depth);
        if ( std::next(i) != e) {
            s += ",\n";
        }
    }
    s += "\n" + indent1 + "}";
    return std::move(s);
}

std::string array_tojson(const JSON::JsonData &data)
{
    std::string s = "[";
    for (auto i = data.array->begin(), e = data.array->end(); i != e; i++) {
        s += i->ToJson();
        if (std::next(i) != e) {
            s += ',';
        }
    }
    s += "]";
    return std::move(s);
}

std::string array_toprettyjson(const JSON::JsonData &data, int depth)
{
    std::string indent1(JSON::_TAB_INDENT*depth++, ' ');
    std::string indent2(JSON::_TAB_INDENT*depth, ' ');
    
    if (data.array->size() == 0) {
        return "[]";
    }
    std::string indent(' ', depth * JSON::_TAB_INDENT);
    std::string s = "[\n";
    for (auto i = data.array->begin(), e = data.array->end(); i != e; i++) {
        s += indent2 + i->ToPrettyJson(depth);
        if (std::next(i) != e) {
            s += ",\n";
        }
    }
    s += "\n" + indent1 + "]";
    return std::move(s);
}

const JSON JSON::NullObject = nullptr;

JSON::JSON():m_type(Type::Null)
{
    m_data.object = nullptr;
}

JSON::JSON(std::nullptr_t):m_type(Type::Null)
{
    m_data.object = nullptr;
}


JSON::JSON(const char *v):m_type(Type::Null)
{
    this->SetType(Type::String);
    *m_data.string = v;
}

JSON::JSON(const std::string &v):m_type(Type::Null)
{
    this->SetType(Type::String);
    *m_data.string = v;
}

JSON::JSON(std::initializer_list<JSON> list):m_type(Type::Null)
{
    this->SetType(Type::Object);
    for (auto i = list.begin(), e = list.end(); i != e; i++, i++) {
        this->operator[](i->ToString()) = *std::next(i);
    }
}

JSON::JSON(JSON&& other)
:m_data(other.m_data), m_type(other.m_type)
{
    other.m_type = Type::Null;
    other.m_data.object = nullptr;
}

JSON::JSON(const JSON &other)
{
    this->CopyFrom(other);
}

JSON::~JSON()
{
    this->Clear();
}

JSON& JSON::operator=(const char *v)
{
    this->SetType(Type::String);
    *m_data.string = v;
    return *this;
}

JSON& JSON::operator=(const std::string &v)
{
    this->SetType(Type::String);
    *m_data.string = v;
    return *this;
}

JSON& JSON::operator=(const JSON &other)
{
    return this->CopyFrom(other);
}

JSON& JSON::operator=(JSON&& other)
{
    m_data = other.m_data;
    m_type = other.m_type;
    other.m_data.object = nullptr;
    other.m_type = Type::Null;
    return *this;
}

JSON& JSON::operator[](const std::string &key)
{
    this->SetType(Type::Object);
    return m_data.object->operator[](key);
}

JSON& JSON::operator[](unsigned index)
{
    this->SetType(Type::Array);
    if(index >= m_data.array->size()) {
        m_data.array->resize(index + 1);
    }
    return m_data.array->operator[](index);
}

const JSON& JSON::Get(const std::string &key) const
{
    if (Type::Object == m_type) {
        auto i =m_data.object->find(key);
        if (i != m_data.object->end()) {
            return i->second;
        }
    }
    return JSON::NullObject;
}

const JSON& JSON::Get(unsigned index) const
{
    if (Type::Array == m_type) {
        int l = this->GetSize();
        if (index < l) {
            return m_data.array->at(index);
        }
    }
    return JSON::NullObject;
}

bool JSON::HasKey(const std::string &key) const
{
    if(m_type == Type::Object) {
        return m_data.object->find(key) != m_data.object->end();
    }
    return false;
}

std::string JSON::ToPrettyJson(int depth) const
{
    switch( m_type ) {
        case Type::Null:   return "null";
        case Type::Object: return object_toprettyjson(m_data, depth);
        case Type::Array:  return array_toprettyjson(m_data, depth);
        case Type::String: return "\"" + json_escape(*m_data.string) + "\"";
        case Type::Float:  return std::to_string(m_data.floating);
        case Type::Integer:return std::to_string(m_data.integer);
        case Type::Boolean:return m_data.boolean ? "true" : "false";
        default: return "";
    }
}

std::string JSON::ToJson() const
{
    switch( m_type ) {
        case Type::Null:   return "null";
        case Type::Object: return object_tojson(m_data);
        case Type::Array:  return array_tojson(m_data);
        case Type::String: return "\"" + json_escape(*m_data.string) + "\"";
        case Type::Float:  return std::to_string(m_data.floating);
        case Type::Integer:return std::to_string(m_data.integer);
        case Type::Boolean:return m_data.boolean ? "true" : "false";
        default: return "";
    }
}

std::string JSON::ToString() const
{
    return *m_data.string;
}

double JSON::ToFloat()
{
    return m_data.floating;
}

long JSON::ToInteger()
{
    return m_data.integer;
}

bool JSON::ToBoolean()
{
    return m_data.boolean;
}

int JSON::GetSize() const
{
    if( m_type == Type::Object )
        return (int)m_data.object->size();
    else if( m_type == Type::Array )
        return (int)m_data.array->size();
    else
        return -1;
}

JSON::Type JSON::GetType() const
{
    return m_type;
}

bool JSON::IsNull() const
{
    return m_type == Type::Null;
}

JSON& JSON::CopyFrom(const JSON &other)
{
    switch(other.m_type) {
        case Type::Object: m_data.object = new Object(other.m_data.object->begin(), other.m_data.object->end());
            break;
        case Type::Array : m_data.array  = new Array(other.m_data.array->begin(), other.m_data.array->end());
            break;
        case Type::String: m_data.string = new String(*other.m_data.string);
            break;
        default: m_data = other.m_data;
    }
    m_type = other.m_type;
    return *this;
}

JSON& JSON::Clear()
{
    switch (m_type) {
        case Type::String: delete m_data.string; break;
        case Type::Array : delete m_data.array;  break;
        case Type::Object: delete m_data.object; break;
        default: break;
    }
    m_type = Type::Null;
    return *this;
}

void JSON::SetType(Type type)
{
    if (m_type == type) {
        return;
    }
    this->Clear();
    
    switch (type) {
        case Type::String : m_data.string = new String; break;
        case Type::Array  : m_data.array  = new Array;  break;
        case Type::Object : m_data.object = new Object; break;
        case Type::Float  : m_data.floating = 0.f;      break;
        case Type::Integer: m_data.integer  = 0;        break;
        case Type::Boolean: m_data.boolean = false;     break;
        case Type::Null   : m_data.object  = nullptr;   break;
        default: break;
    }
    m_type = type;
}

JsonIterator<JSON::Object> JSON::ObjectInerator() {
    if (m_type == Type::Object)
        return JsonIterator<Object>(m_data.object);
    return JsonIterator<Object>(nullptr);
}

JsonIterator<JSON::Array> JSON::ArrayInerator() {
    if (m_type == Type::Array)
        return JsonIterator<Array>(m_data.array);
    return JsonIterator<Array>(nullptr);
}

JSON JSON::Make(Type type)
{
    JSON ret;
    ret.SetType(type);
    return std::move(ret);
}

JSON JSON::Load(const std::string &s)
{
    size_t offset = 0;
    return std::move(JsonParser::JsonParser::ParseNext(s, offset));
}

JSON json::Object()
{
    return std::move(JSON::Make(JSON::Type::Object));
}

JSON json::Array()
{
    return std::move(JSON::Make(JSON::Type::Array));
}

