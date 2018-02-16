//
//  Unit_Test_Bitmex_API.cpp
//  bfx-api-cpp
//
//  Created by kai fang on 1/9/18.
//  Copyright © 2018 jvr23. All rights reserved.
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
#include "Bitmex_Order_Manager.hpp"
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
using namespace rapidjson;
using namespace std;



// ATTENTIONS : EACH OF THE FOLLOWING FUNCTION WORK, HOWEVER, HF REQUEST WILL MAKE SIGNATURE INVALID DUE TO RATE LIMIT
int Unit_Test_Bitmex_API()
{
    
    // every request need to have a time difference of one million second due to the setup of nonce
    
    //BitmexAPI mybitmex("buSRlx7Rgfgtl7Q4verQ_40O","JTUqBTMZVqB6x55GV98q6qGRqbgrO0jGDITZ75M5lM4uu3pg");
    BitmexAPI mybitmex("eUv4KBPPsJZwnqMccFIc0gXW","YOopNs90S5Z9VNdWvycOPRootVnLHiT7O8gKQzY0_UBeWWXB");  //testnet key
    // when use the testnet , we need to set the path to http://testnet.bitmex.com rather than http://www.bitmex.com
    string result;
    
    
    mybitmex.getfunding(result);
    cout << "getfunding : " << result<<endl<<endl<<endl;
    //  == get instrument == //
    mybitmex.getinstrument(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    mybitmex.getinstrumentactive(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    mybitmex.getinstrumentactiveintervals(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    mybitmex.getinstrumentactiveandindices(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    mybitmex.getinstrumentindices(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    mybitmex.getinstrumentcompositeindex(result);
    cout << "getinstrument" << result <<endl <<endl<<endl;
    
    // == get execution order ==//
    mybitmex.getRawExecutions(result);  //== all order
    cout << result <<endl <<endl<<endl;
    
    string filter = "{\"execType\": [ \"Trade\"]}";
    mybitmex.getRawExecutions(result,filter);  //== order trade order could change filter to settlement, close...
    cout << result <<endl <<endl<<endl;
    
    mybitmex.getExecutionsHistory(result);
    cout << result <<endl <<endl<<endl;
    
    // ===  order == //
    mybitmex.getorder(result,""); // "" mean all, you could set that to be a json object the specify the order type
    cout <<result<<endl <<endl<<endl;
    
    
    mybitmex.getorder( result); // filter default to be open order --- "{\"open\":true}"
    cout <<result<<endl <<endl<<endl;
    
    mybitmex.placeorder("XBTUSD", 10000, 10, result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.changeorder("a082dd7e-4ccf-97db-a0b3-7f48ef1af6a7", 100, 10, result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.changeorderPrice("a082dd7e-4ccf-97db-a0b3-7f48ef1af6a7", 101,  result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.changeorderQuantity("a082dd7e-4ccf-97db-a0b3-7f48ef1af6a7",  11, result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.cancelorder("c04ae1a8-5a07-4129-a8fc-edda84e67a32", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.cancelallorder(result);
    cout <<result <<endl<<endl<<endl;
    
    //=== position == //
    
    mybitmex.getposition(result);
    cout <<result <<endl<<endl<<endl;
    
    string symbolefilter = "{\"symbole\": \"XBTUSD\"}";
    mybitmex.getposition(result, symbolefilter);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.isolateleverage("true", "XBTUSD", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.chooseleverage(result, "XBTUSD", 10.0);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.transfermargin(result, "XBTUSD", 1);
    cout <<result <<endl<<endl<<endl;
    
    //== order book ==//
    mybitmex.getorderbook("XBTUSD" , result, 25);
    cout <<result <<endl<<endl<<endl;
    
    //===  user ===//
    mybitmex.getwallet("XBt", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.getmargin("XBt", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.getcommission(result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.getposition(result);
    cout <<result <<endl<<endl<<endl;
    
    
    mybitmex.getwallet("XBt", result);
    cout <<result <<endl<<endl<<endl;
    mybitmex.getwallet("XBt", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.getmargin("XBt", result);
    cout <<result <<endl<<endl<<endl;
    
    
  
    
    vector<double> price = {8000,8001,8002,8003};
    vector<double> price2={7000,7000,7000,7000};
    vector<double> quant = {1,1,1,1};
    vector<string> clordid = {"sdf","sdfmwe","sfl","sdflo"};
    vector<string> exeinst={"ParticipateDoNotInitiate","ParticipateDoNotInitiate","",""};
   
    
    vector<string> exeinst2={"","","",""};
    
    mybitmex.placeorderbulk("XBTUSD", price, quant,clordid,  result,exeinst2);
    cout <<result <<endl<<endl<<endl;
   
    mybitmex.cancelordersbulk_ClordID(clordid, result);
    cout <<result <<endl<<endl<<endl;
    
    
    vector<string> Ordertype={"Limit","Limit","Market","Market"};
    mybitmex.placeorderbulk("XBTUSD", price2, quant,clordid,exeinst, Ordertype, result);
    
    cout <<result <<endl<<endl<<endl;
   
    
    mybitmex.placeorder("XBTUSD", 7000, 10,"765jfj6frwvkj2", "","Market", result);
    cout <<result <<endl<<endl<<endl;
    // the following function need you to change the key at the beginning to the withdrawal key
   /*
    mybitmex.requestWithdraw("XBt", 1, "213412342345", result);
    cout <<result <<endl<<endl<<endl;
    // you need to know the meaning of token to do the test  --  it should be from TFA gateway you set, i set it in my phone, so not easy to get
    mybitmex.confirmWithdraw("234245", result);
    cout <<result <<endl<<endl<<endl;
    
    mybitmex.cancelWithdraw("234245", result);
    cout <<result <<endl<<endl<<endl;
    
   */
    return 0;
    
    
}



int Unit_Test_Bitmex_Order_Manager(){

    // something strange is that when i continue to run the following code, signature will become invalid, i think we may need to change the way we get tonce to make it run properly
    Bitmex_Order_Manager my_bitmex_OM("API-key-secret.txt");
    /*
    my_bitmex_OM.getAccount();
    my_bitmex_OM.getContracts();
    my_bitmex_OM.getPositions();
    my_bitmex_OM.sendOrder("XBTUSD", 13000, 100, "", false);
    my_bitmex_OM.getOrderBook("XBTUSD");
    my_bitmex_OM.getAllOrders();
    my_bitmex_OM.cancelAllOrders_with_type("Buy");
    my_bitmex_OM.sendOrder("XBTUSD", 13000, 100, "", false);
    my_bitmex_OM.cancelAllOrders();
     */
    vector<double> price = {7000,7001,7002,7003};
    vector<double> quant = {1,1,1,1};
    vector<string> clordid = {"kaim714rd2","kai8ew1ad22","kai9dd1ew32","kai10we2d1qa2"};
    vector<string> exeinst={"ParticipateDoNotInitiate","ParticipateDoNotInitiate","ParticipateDoNotInitiate","ParticipateDoNotInitiate"};
     vector<string> exeinst2={"","","",""};
    vector<string> ordertype ={"Limit", "Limit","Market","Market"};
    my_bitmex_OM.sendOrderbulk("XBTUSD", price, quant,clordid,exeinst2,ordertype);
   
    my_bitmex_OM.cancelOrderbulk_clOrdID(clordid);
    
    my_bitmex_OM.sendOrder("XBTUSD", 6500, 100,"7sgedf6nhgfewewvkj","", "", false);
    
    return 0;


}

