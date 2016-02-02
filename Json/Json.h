//
//  Json.h
//  Json
//
//  Created by luwei on 16/2/1.
//  Copyright (c) 2016年 luwei. All rights reserved.
//

#ifndef __Json__Json__
#define __Json__Json__

#include <stdio.h>
#include <unordered_map>
#include <deque>
#include <initializer_list>
#include <type_traits>
#include <string>

namespace json {
    
template <typename _Container>
class JsonIterator {
public:
    JsonIterator(_Container *val) : object(val) {}
    JsonIterator(std::nullptr_t)  : object(nullptr) {}
    
    typename _Container::iterator begin() { return object ? object->begin() : typename _Container::iterator(); }
    typename _Container::iterator end() { return object ? object->end() : typename _Container::iterator(); }
    typename _Container::const_iterator begin() const { return object ? object->begin() : typename _Container::iterator(); }
    typename _Container::const_iterator end() const { return object ? object->end() : typename _Container::iterator(); }
    
private:
    _Container *object;
};

class JSON {
public:
    typedef std::string String;
    typedef std::deque<JSON> Array;
    typedef std::unordered_map<std::string, JSON> Object;
    static const JSON NullObject;
    
    enum { _TAB_INDENT = 4 };
    enum class Type {
        Null, Float, Integer, Boolean, String, Array, Object
    };
    
    JSON();
    JSON(std::nullptr_t);
    template <typename _Type>
    JSON(_Type v, typename std::enable_if<std::is_same<_Type, bool>::value>::type* = 0);
    template <typename _Type>
    JSON(_Type v, typename std::enable_if<std::is_floating_point<_Type>::value>::type* = 0);
    template <typename _Type>
    JSON(_Type v, typename std::enable_if<std::is_integral<_Type>::value && !std::is_same<_Type, bool>::value>::type* = 0);
    JSON(const char *v);
    JSON(const std::string &v);
    JSON(std::initializer_list<JSON> list);
    
    JSON(JSON&& other);
    JSON(const JSON &other);
    ~JSON();

    template <typename _Type>
    typename std::enable_if<std::is_same<_Type, bool>::value, JSON&>::type operator=(_Type v);
    template <typename _Type>
    typename std::enable_if<std::is_floating_point<_Type>::value, JSON&>::type operator=(_Type v);
    template <typename _Type>
    typename std::enable_if<std::is_integral<_Type>::value && !std::is_same<_Type, bool>::value, JSON&>::type operator=(long v);
    JSON& operator=(const char *v);
    JSON& operator=(const std::string &v);
    JSON& operator=(const JSON &other);
    JSON& operator=(JSON&& other);
    
    JSON& operator[](const std::string &key);
    JSON& operator[](unsigned index);
    const JSON& Get(const std::string &key) const;
    const JSON& Get(unsigned index) const;

    bool HasKey(const std::string &key) const;
    
    template <typename T>
    void Append(T arg);
    template <typename T, typename... U>
    void Append(T arg, U... args);
    
    std::string ToPrettyJson(int depth=0) const;
    std::string ToJson() const;

    std::string ToString() const;
    double ToFloat();
    long ToInteger();
    bool ToBoolean();
    
    int  GetSize() const;
    Type GetType() const;
    bool IsNull()  const;

    JSON& CopyFrom(const JSON &other);
    JSON& Clear();
    void SetType(Type type);
    
    JsonIterator<Object> ObjectInerator();
    JsonIterator<Array> ArrayInerator();
    
    static JSON Make(Type type);
    static JSON Load(const std::string &s);
    

public:
    union JsonData{
        bool    boolean;
        long    integer;
        double  floating;
        String* string;
        Array*  array;
        Object* object;
    };
    
private:
    union JsonData m_data;
    JSON::Type     m_type;
};

JSON Object();
JSON Array();

template <typename... T>
JSON Array(T... args) {
    JSON array = JSON::Make(JSON::Type::Array);
    array.Append(args...);
    return std::move(array);
}

template <typename _Type>
JSON::JSON(_Type v, typename std::enable_if<std::is_same<_Type, bool>::value>::type*):m_type(Type::Null) {
    this->SetType(Type::Boolean);
    m_data.boolean = v;
}

template <typename _Type>
JSON::JSON(_Type v, typename std::enable_if<std::is_floating_point<_Type>::value>::type*):m_type(Type::Null) {
    this->SetType(Type::Float);
    m_data.floating = v;
}

template <typename _Type>
JSON::JSON(_Type v, typename std::enable_if<std::is_integral<_Type>::value && !std::is_same<_Type, bool>::value>::type*):m_type(Type::Null) {
    this->SetType(Type::Integer);
    m_data.integer = v;
}

template <typename _Type>
typename std::enable_if<std::is_same<_Type, bool>::value, JSON&>::type JSON::operator=(_Type v) {
    this->SetType(Type::Boolean);
    m_data.boolean = v;
    return *this;
}

template <typename _Type>
typename std::enable_if<std::is_floating_point<_Type>::value, JSON&>::type JSON::operator=(_Type v) {
    this->SetType(Type::Float);
    m_data.floating = v;
    return *this;
}

template <typename _Type>
typename std::enable_if<std::is_integral<_Type>::value && !std::is_same<_Type, bool>::value, JSON&>::type JSON::operator=(long v) {
    this->SetType(Type::Integer);
    m_data.integer = v;
    return *this;
}

template <typename T>
void JSON::Append(T arg) {
    SetType(Type::Array);
    m_data.array->emplace_back(arg);
}

template <typename T, typename... U>
void JSON::Append(T arg, U... args) {
    this->Append(arg);
    this->Append(args...);
}


}

#endif /* defined(__Json__Json__) */
