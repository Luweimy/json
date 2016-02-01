//
//  main.cpp
//  Json
//
//  Created by luwei on 16/2/1.
//  Copyright (c) 2016年 luwei. All rights reserved.
//

#include <iostream>
#include <unordered_map>
#include <deque>

namespace json {


class Base {
public:
    virtual std::string ToString() const = 0;
    virtual bool IsNull() const = 0;
};

class Null : public Base {
public:
    Null() {}
    virtual std::string ToString() const { return "null"; }
    virtual bool IsNull() const {return true;}
};

class Float : public Base {
public:
    Float(float value):m_value(value) {
    }
    template <typename _Type>
    Float& operator=(_Type v) {
        m_value = v; return *this;
    }
    virtual std::string ToString() const {
        return std::to_string(m_value);
    }
    virtual bool IsNull() const {
        return false;
    }
private: float m_value;
};

class Integer : public Base {
public:
    Integer(int value):m_value(value) {
    }
    template <typename _Type>
    Integer& operator=(_Type v) {
        m_value = v; return *this;
    }
    virtual std::string ToString() const {
        return std::to_string(m_value);
    }
    virtual bool IsNull() const {
        return false;
    }

private: int m_value;
};

class Boolean : public Base {
public:
    Boolean(bool value):m_value(value) {
    }
    Boolean& operator=(bool v) {
        m_value = v; return *this;
    }
    virtual std::string ToString() const {
        return m_value ? "true" : "false";
    }
    virtual bool IsNull() const {
        return false;
    }

private: bool m_value;
};

class String : public Base {
public:
    String(const char *value) {
        m_value = new std::string(value);
    }
    ~String(){
        delete m_value;
    }
    String& operator=(const char *v) {
        m_value->operator=(v); return *this;
    }
    virtual std::string ToString() const {
        return *m_value;
    }
    virtual bool IsNull() const {
        return false;
    }

private: std::string *m_value;
};

class Object : public Base {
public:
    Object() {
        m_value = new std::unordered_map<std::string, Base*>();
    }
    ~Object() {
        delete m_value;
    }
    bool HasKey( const std::string &key ) const {
        return m_value->find( key ) != m_value->end();
    }
    virtual std::string ToString() const {
//        printf("size:%d\n", m_value->size());
        std::string s("{");
        auto iter = m_value->begin();
        if (iter != m_value->end())
            s += iter->first + ":" + iter->second->ToString();
        for (++iter; iter != m_value->end(); iter++) {
            s += ',';
            s += iter->first + ":" + iter->second->ToString();
        }
        s += "}";
        return std::move(s);
    }
    virtual bool IsNull() const {
        return false;
    }
    Base& operator[]( const std::string &key ) {
        printf("ok");
        return *(m_value->operator[](key));
    }

private:
    std::unordered_map<std::string, Base*> *m_value;
};

class Array : public Base {
public:
    Array(){
        m_value = new std::deque<Base*>();
    }
    ~Array(){delete m_value;}
    virtual std::string ToString() const {
        size_t size = m_value->size();
        std::string s("[");
        if (size != 0)
            s += (*m_value)[0]->ToString();
        for (int i = 1; i < size; i++) {
            s += ',';
            s += (*m_value)[i]->ToString();
        }
        s += "]";
        return std::move(s);
    }
    virtual bool IsNull() const {
        return false;
    }
    Base& operator[]( unsigned index ) {
        if (index >= m_value->size()) m_value->resize(index + 1);
        return *(m_value->operator[](index));
    }

private:
    std::deque<Base*> *m_value;
};
    
 
}

static json::Array Make() {
    json::Array ret;
    return ret;
}

json::Array MakeArray() {
    return std::move(Make());
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    json::Object o;
    
    json::Array *a = new json::Array();
    
    o["ok"] = *a;
    
    std::cout<< o.ToString() << std::endl;
    
    return 0;
}
