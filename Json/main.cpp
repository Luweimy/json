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

class JBO {
public: virtual std::string ToString() = 0;
};

class Null : public JBO {
public: std::string ToString() { return "null"; }
};

class Float : public JBO {
public: std::string ToString() {
    return std::to_string(m_value);
}
private:
    float m_value;
};

class Integer : public JBO {
public: std::string ToString() {
    return std::to_string(m_value);
}
private:
    int m_value;
};

class Boolean : public JBO {
public: std::string ToString() {
    return m_value ? "true" : "false";
}
private:
    bool m_value;
};

class String : public JBO {
public: std::string ToString() {
    return *m_value;
}
private:
    std::string *m_value;
};

class Object : public JBO {
public: std::string ToString() {
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
private:
    std::unordered_map<std::string, JBO*> *m_value;
};

class Array : public JBO {
public: std::string ToString() {
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
private:
    std::deque<JBO*> *m_value;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    return 0;
}
