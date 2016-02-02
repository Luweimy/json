//
//  main.cpp
//  Json
//
//  Created by luwei on 16/2/1.
//  Copyright (c) 2016年 luwei. All rights reserved.
//

#include <iostream>
#include "Json.h"

using namespace json;

//JSON Array() {
//    return std::move(JSON::Make(JSON::Type::Array));
//}
//
//template <typename... T>
//JSON Array(T... args) {
//    JSON array = JSON::Make(JSON::Type::Array);
//    array.Append(args...);
//    return std::move(array);
//}
//
//JSON Object() {
//    return std::move(JSON::Make(JSON::Type::Object));
//}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    JSON o;
    
    o["ok"] = Array("nihao", 1, false, std::string("make"));
//    o["ok1"]["make"] = Object();
    
    std::cout<< o.ToString() << std::endl;
    
    return 0;
}
