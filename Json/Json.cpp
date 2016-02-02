//
//  Json.cpp
//  Json
//
//  Created by luwei on 16/2/1.
//  Copyright (c) 2016年 luwei. All rights reserved.
//

#include "Json.h"

using namespace json;

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

std::string object_tostring(const JSON::JsonData &data)
{
    std::string s("{");
    for (auto i = data.object->begin(), e = data.object->end(); i != e; i++) {
        s += "\"" + i->first + "\":" + i->second.ToString();
        if ( std::next(i) != e) {
            s += ',';
        }
    }
    s += "}";
    return std::move(s);
}

std::string array_tostring(const JSON::JsonData &data)
{
    std::string s = "[";
    for (auto i = data.array->begin(), e = data.array->end(); i != e; i++) {
        s += i->ToString();
        if (std::next(i) != e) {
            s += ',';
        }
    }
    s += "]";
    return std::move(s);
}

const JSON JSON::Null = nullptr;

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
    return JSON::Null;
}

const JSON& JSON::Get(unsigned index) const
{
    if (Type::Array == m_type) {
        int l = this->GetSize();
        if (index < l) {
            return m_data.array->at(index);
        }
    }
    return JSON::Null;
}

bool JSON::HasKey(const std::string &key) const
{
    if(m_type == Type::Object) {
        return m_data.object->find(key) != m_data.object->end();
    }
    return false;
}

std::string JSON::ToString() const {
    switch( m_type ) {
        case Type::Null:   return "null";
        case Type::Object: return object_tostring(m_data);
        case Type::Array:  return array_tostring(m_data);
        case Type::String: return "\"" + json_escape(*m_data.string) + "\"";
        case Type::Float:  return std::to_string(m_data.floating);
        case Type::Integer:return std::to_string(m_data.integer);
        case Type::Boolean:return m_data.boolean ? "true" : "false";
        default: return "";
    }
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

JSON& JSON::CopyFrom(const JSON &other) {
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

JSON& JSON::Clear() {
    switch (m_type) {
        case Type::String: delete m_data.string; break;
        case Type::Array : delete m_data.array;  break;
        case Type::Object: delete m_data.object; break;
        default: break;
    }
    m_type = Type::Null;
    return *this;
}

void JSON::SetType(Type type) {
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

JSON JSON::Make(Type type)
{
    JSON ret;
    ret.SetType(type);
    return std::move(ret);
}
JSON JSON::Load(const std::string &s) {
    return std::move(JSON());
}


