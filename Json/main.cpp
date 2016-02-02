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
using namespace std;

void test_array() {
    
    JSON array;
    
    array[2] = "Test2";
    cout << array.ToJson() << endl;
    array[1] = "Test1";
    cout << array.ToJson() << endl;
    array[0] = "Test0";
    cout << array.ToJson() << endl;
    array[3] = "Test4";
    cout << array.ToJson() << endl;
    
    // Arrays can be nested:
    JSON Array2;
    
    Array2[2][0][1] = true;
    
    cout << Array2.ToJson() << endl;
}

void test_load() {
    JSON Int = JSON::Load( " 123 " );
    JSON Float = JSON::Load( " 123.234 " );
    JSON Str = JSON::Load( "\"String\"" );
    JSON EscStr = JSON::Load( "\" \\\"Some\\/thing\\\" \"" );
    JSON Arr = JSON::Load( "[1,2, true, false,\"STRING\", 1.5]" );
    JSON Obj = JSON::Load( "{ \"Key\" : \"StringValue\","
                          "   \"Key2\" : true, "
                          "   \"Key3\" : 1234, "
                          "   \"Key4\" : null }" );
    
    cout << Int.ToJson() << endl;
    cout << Float.ToJson() << endl;
    cout << Str.ToJson() << endl;
    cout << EscStr.ToJson() << endl;
    cout << Arr.ToJson() << endl;
    cout << Obj.ToJson() << endl;
}

void test_prim() {
    JSON obj;
    
    obj = true;
    cout << "Value: " << boolalpha << obj.ToBoolean() << endl;
    
    obj = "Test String";
    cout << "Value: " << obj.ToString() << endl;
    
    obj = 2.2;
    cout << "Value: " << obj.ToFloat() << endl;
    
    obj = 3;
    cout << "Value: " << obj.ToInteger() << endl;
}

void test_json() {
        // Example of creating each type
        // You can also do JSON::Make( JSON::Class )
        JSON null;
        JSON Bool( true );
        JSON Str( "RawString" );
        JSON Str2( string( "C++String" ) );
        JSON Int( 1 );
        JSON Float( 1.2 );
        JSON Arr = json::Array();
        JSON Obj = json::Object();
        
        // Types can be overwritten by assigning
        // to the object again.
        Bool = false;
        Bool = "rtew";
        Bool = 1;
        Bool = 1.1;
        Bool = string( "asd" );
        
        // Append to Arrays, appending to a non-array
        // will turn the object into an array with the
        // first element being the value that's being
        // appended.
        Arr.Append( 1 );
        Arr.Append( "test" );
        Arr.Append( false );
        
        // Access Array elements with operator[]( unsigned ).
        // Note that this does not do bounds checking, and
        // returns a reference to a JSON object.
        JSON& val = Arr[0];
        
        // Arrays can be intialized with any elements and
        // they are turned into JSON objects. Variadic
        // Templates are pretty cool.
        JSON Arr2 = json::Array( 2, "Test", true );
        
        // Objects are accessed using operator[]( string ).
        // Will create new pairs on the fly, just as std::map
        // would.
        Obj["Key1"] = 1.0;
        Obj["Key2"] = "Value";
        
        JSON Obj2 = json::Object();
        Obj2["Key3"] = 1;
        Obj2["Key4"] = Arr;
        Obj2["Key5"] = Arr2;
        
        // Nested Object
        Obj["Key6"] = Obj2;
        
        // Dump Obj to a string.
        cout << Obj.ToJson() << endl;
        
        // We can also use a more JSON-like syntax to create
        // JSON Objects.
        JSON Obj3 = {
            "Key1", "Value",
            "Key2", true,
            "Key3", {
                "Key4", json::Array( "This", "Is", "An", "Array" ),
                "Key5", {
                    "BooleanValue", true
                }
            }
        };
        
        cout << Obj3.ToJson() << endl;
}

void dumpArray( JSON &array ) {
    for( auto &j : array.ArrayInerator() )
        std::cout << "Value: " << j.ToJson() << "\n";
}

void dumpObject( JSON &object ) {
    for( auto &j : object.ObjectInerator() )
        std::cout << "Object[ " << j.first.c_str() << " ] = " << j.second.ToJson() << "\n";
}

void test_iter()
{
    JSON array = JSON::Make( JSON::Type::Array );
    JSON obj   = JSON::Make( JSON::Type::Object );
    
    array[0] = "Test0";
    array[1] = "Test1";
    array[2] = "Test2";
    array[3] = "Test3";
    
    obj[ "Key0" ] = "Value1";
    obj[ "Key1" ] = array;
    obj[ "Key2" ] = 123;
    
    std::cout << "=============== tests ================\n";
    dumpArray( array );
    dumpObject( obj );
}


int main(int argc, const char * argv[]) {
    
    test_array();
    test_load();
    test_prim();
    test_json();
    test_iter();
    
    JSON obj( {
        "Key", 1,
        "Key3", true,
        "Key4", nullptr,
        "Key2", {
            "Key4", "VALUE",
            "Arr", json::Array( 1, "Str", false )
        }
    } );
    
    cout << obj.ToPrettyJson() << endl;

    
    json::JSON o = {
        "Key1", "Value",
        "Key2", true,
        "Key3", {
            "Key5", {
                "BooleanValue", true
            }
        }
    };
    
    o["my"] = json::Array("nihao", std::string("make"), 1, 1.2, false);
    JSON o1 = json::JSON::Load("{\"nihao\":[]}");
    std::cout<< "Json = " << o1.ToJson() << std::endl;

    return 0;
}
