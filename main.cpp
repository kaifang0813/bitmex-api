//
//  main.cpp
//  bfx-api-cpp
//
//  Created by kai fang on 12/28/17.
//  Copyright © 2017 jvr23. All rights reserved.
//


#include <iostream>
//#include "CexioAPI.hpp"
#include <fstream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/memorystream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/memorystream.h>
//#include "CEX_Order_Manager.hpp"
//#include <curl/curl.h>
#include "BitmexAPI.hpp"
#include "Unit_Test_Bitmex_API.hpp"
using std::cout;
using std::endl;
using std::ifstream;
using std::string;

using namespace rapidjson;
using namespace std;

int main(){

    //int result = Unit_Test_Bitmex_API();
   int result2 = Unit_Test_Bitmex_Order_Manager();

    return 0;
}
