//
//  Bitmex_Order_Manager.cpp
//  bfx-api-cpp
//
//  Created by kai fang on 1/9/18.
//  Copyright © 2018 jvr23. All rights reserved.
//

#include "Bitmex_Order_Manager.hpp"

#include <math.h>
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
#include "struct.h"
using namespace rapidjson;
using namespace std;
#include "BitmexAPI.hpp"

int Bitmex_Order_Manager::getOrderBook(string symbol, int depth){
    string orderbook="";
    
    int error =  my_bitmex->getorderbook(symbol,orderbook,depth);
    
    if(error != 0)
        return error;
    
    //cout << orderbook;
    
    Document Response;
    Response.Parse(orderbook.c_str());
    
    if(orderbook.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& orderlist = Response;
    bidsPrice.clear();
    bidsSize.clear();
    asksPrice.clear();
    asksSize.clear();
    for(int i=0; i<2*depth; i++){
        if(orderlist[i]["side"].GetString() == "Buy"){
        bidsPrice.push_back(orderlist[i]["price"].GetDouble());
        bidsSize.push_back(orderlist[i]["size"].GetDouble());
        }else{
        asksPrice.push_back(orderlist[i]["price"].GetDouble());
        asksPrice.push_back(orderlist[i]["size"].GetDouble());
        }
    }
    nBids = depth;
    nAsks = depth;
    
    return 0;
    
}

int Bitmex_Order_Manager::sendOrder(string symbol, double price, double quantity, string tif, bool isMarket ){
    
    string sendOrderResponse = "";
    std::string type;
    
    int error = my_bitmex->placeorder(symbol,price,quantity,sendOrderResponse);
    //cout <<endl<< sendOrderResponse <<endl;
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(sendOrderResponse.c_str());
    
    if(Response.HasMember("error")){
        cout << Response["error"]["message"].GetString() <<endl;
        return -100;
    }
    
    ////////////////////////////////////// Here is the respond container of sendorder /////////////////////////////////////
    
    string orderID = Response["orderID"].GetString();
    string orderside = Response["side"].GetString();
    string ordertime = Response["transactTime"].GetString();
    string ordertimestamp = Response["timestamp"].GetString();
    string status = Response["ordStatus"].GetString();
    double filled  = quantity - Response["leavesQty"].GetDouble();
    
    // could not get all information we want in order data structure

    return 0;
    
    
}

// Cancel order
int Bitmex_Order_Manager::cancelOrder(string orderId){
    
    string cancelOrderResponse ="";
    int error = my_bitmex->cancelorder(orderId,cancelOrderResponse);
    
    if(error != 0)
        return error;
 
        Document Response;
        Response.Parse(cancelOrderResponse.c_str());
        
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }else if(Response["ordStatus"].GetString() == "Canceled"){
        
            return 0;
        }else{
            return -55;
        }
        
        return 0;
    }
    



int Bitmex_Order_Manager::cancelAllOrders(){
    
    string cancelAllOrderResponse ="";
    int error = my_bitmex->cancelallorder(cancelAllOrderResponse);
    
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(cancelAllOrderResponse.c_str());
    
    if(cancelAllOrderResponse.substr(0,1)=="["){
        const Value& orderlist = Response;
        for(int i=0; i<Response.Size(); i++){
          if(Response[i]["ordStatus"].GetString() !="Canceled")
              return -55;
        }
        return 0;
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    return 0;
}


int Bitmex_Order_Manager::cancelAllOrders_with_type( string type){ // type should be Buy or Sell
    
    string cancelAllOrderResponse ="";
    string filter;
    if(type == "Buy"||type =="BUY" || type=="buy" ){
        filter = "{\"side\":\"Buy\"}";
    }else if (type == "Sell"||type =="SELL" || type=="sell" ){
        filter = "{\"side\":\"Sell\"}";
    }else{
        return -55;
    }
    int error = my_bitmex->cancelallorder(cancelAllOrderResponse, filter);
    
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(cancelAllOrderResponse.c_str());
    
    if(cancelAllOrderResponse.substr(0,1)=="["){
        const Value& orderlist = Response;
        for(int i=0; i<Response.Size(); i++){
            if(Response[i]["ordStatus"].GetString() !="Canceled")
                return -55;
        }
        return 0;
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    return 0;
}






int Bitmex_Order_Manager::getAllOrders(){
    string allopenorder="";
    int error = my_bitmex->getRawExecutions(allopenorder);
    if(error != 0)
        return error;
    // cout << allopenorder <<endl;
    Document Response;
    Response.Parse(allopenorder.c_str());
    
    
    if(allopenorder.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& orderlist = Response;
    orders.clear();  // difficult to search in vector, O(n*m) for all order, so i directly delete all order and store all orders again
    for (SizeType i = 0; i < orderlist.Size(); i++) // Uses SizeType instead of size_t
    {
        string id = (orderlist[i]["orderID"].GetString());
        string time = (orderlist[i]["transactTime"].GetString());
        string side = (orderlist[i]["side"].GetString());
        double price = orderlist[i]["price"].GetDouble();
        double amount = orderlist[i]["orderQty"].GetDouble();
        string ordersymbol = (orderlist[i]["symbol"].GetString());
        double filled = amount - orderlist[i]["leavesQty"].GetDouble();
        string clientid ="";
        char orderSide;
        if(side == "Buy"){
            orderSide = 'B';}
        else{
                orderSide='S';};
        Order myOrder(id,clientid,ordersymbol,orderSide,time,double(0.0),price,amount,filled);
        // we do not have a right way to converage timestamp, the api use string timestam
        orders.push_back(myOrder);
        nOrders++;
    }
    
    return 0;
    
}

int Bitmex_Order_Manager::getAllOpenOrders(){
    string allopenorder="";
    int error = my_bitmex->getRawExecutions(allopenorder);
    if(error != 0)
        return error;
    // cout << allopenorder <<endl;
    Document Response;
    Response.Parse(allopenorder.c_str());
    
    
    if(allopenorder.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& orderlist = Response;
    orders.clear();  // difficult to search in vector, O(n*m) for all order, so i directly delete all order and store all orders again
    for (SizeType i = 0; i < orderlist.Size(); i++) // Uses SizeType instead of size_t
    {
        if(orderlist[i]["ordStatus"].GetString() == "Filled" ||orderlist[i]["ordStatus"].GetString() == "Canceled"){
        
        }else{
        string id = (orderlist[i]["orderID"].GetString());
        string time = (orderlist[i]["transactTime"].GetString());
        string side = (orderlist[i]["side"].GetString());
        double price = orderlist[i]["price"].GetDouble();
        double amount = orderlist[i]["orderQty"].GetDouble();
        string ordersymbol = (orderlist[i]["symbol"].GetString());
        double filled = amount - orderlist[i]["leavesQty"].GetDouble();
        string clientid ="";
        char orderSide;
        if(side == "Buy"){
            orderSide = 'B';}
        else{
            orderSide='S';};
        Order myOrder(id,clientid,ordersymbol,orderSide,time,double(0.0),price,amount,filled);
        // we do not have a right way to converage timestamp, the api use string timestam
        orders.push_back(myOrder);
        nOrders++;
    }
    }
    return 0;
    
}


int Bitmex_Order_Manager::getOrder(string OrderID){
    string allopenorder="";
    string filter = "{\"orderID\":\""+OrderID+"\"}";
    int error = my_bitmex->getRawExecutions(allopenorder,filter);
    if(error != 0)
        return error;
    // cout << allopenorder <<endl;
    Document Response;
    Response.Parse(allopenorder.c_str());
    
    
    if(allopenorder.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& orderlist = Response;
    for (SizeType i = 0; i < orderlist.Size(); i++) // Uses SizeType instead of size_t
    {
        if(orderlist[i]["ordStatus"].GetString() == "Filled" ||orderlist[i]["ordStatus"].GetString() == "Canceled"){
            
        }else{
            string id = (orderlist[i]["orderID"].GetString());
            string time = (orderlist[i]["transactTime"].GetString());
            string side = (orderlist[i]["side"].GetString());
            double price = orderlist[i]["price"].GetDouble();
            double amount = orderlist[i]["orderQty"].GetDouble();
            string ordersymbol = (orderlist[i]["symbol"].GetString());
            double filled = amount - orderlist[i]["leavesQty"].GetDouble();
            string clientid ="";
            char orderSide;
            if(side == "Buy"){
                orderSide = 'B';}
            else{
                orderSide='S';};
            Order myOrder(id,clientid,ordersymbol,orderSide,time,double(0.0),price,amount,filled);
            // we do not have a right way to converage timestamp, the api use string timestam
           
        }
    }
    return 0;
    
}

int Bitmex_Order_Manager::getContracts(){
    string getinstrument;
    int error = my_bitmex->getinstrumentactive(getinstrument);
    if(error != 0)
        return error;
    
    
    
    Document Response;
    Response.Parse(getinstrument.c_str());
    
    
    if(getinstrument.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& contractlist = Response;
    Contract my_contract;
    for (SizeType i = 0; i < contractlist.Size(); i++) // Uses SizeType instead of size_t
    {
        my_contract.exchange = "Bitmex";
        my_contract.contract = contractlist[i]["symbol"].GetString();  //
        my_contract.base = contractlist[i]["rootSymbol"].GetString();
        my_contract.quote= contractlist[i]["symbol"].GetString();
        my_contract.symbolExchange ="Bitmex"; // do not know what is this, does it mean the symbol you use to represent the exchange, i use cexio for Cex.io
        my_contract.takerFees = contractlist[i]["takerFee"].GetDouble();
        my_contract.makerFees = contractlist[i]["makerFee"].GetDouble();
        my_contract.marginFees =0;  // could not find information on this   and we also need to pay settlement fee
        my_contract.precisionPrice = contractlist[i]["tickSize"].GetDouble();//
        my_contract.precisionSize = contractlist[i]["lotSize"].GetDouble();
        my_contract.initMargin = contractlist[i]["initMargin"].GetDouble();// initMargin when the leverage is 2 --- open a position
        my_contract.maintMargin = contractlist[i]["maintMargin"].GetDouble(); // initMargin when the leverage is 2 --- holding a position
        my_contract.minOrderSize = contractlist[i]["lotSize"].GetDouble();
        my_contract.maxOrderSize = contractlist[i]["maxOrderQty"].GetDouble();;
        my_contract.isShort = true; // do not know the meaning
        my_contract.rateLimitPublic = 300; // do not specify but the remaining could be return as header in http
        my_contract.rateLimitPrivate = 300; // do not specify but the remaining could be return as header in http
        Contract_List.push_back(my_contract);
    }
    return 0;
}



int Bitmex_Order_Manager::getPosition(string symbol, double &Position) // i think this should be a better function compared to the one above
{
    string filter = "{\"symbol\" : \" "+ symbol +"\"}";
    
    string getposition;
    int error = my_bitmex->getposition(getposition,filter);
    if(error != 0)
        return error;
    
    
    
    Document Response;
    Response.Parse(getposition.c_str());
    
    
    if(getposition.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& Positionlist = Response;
    
    for (SizeType i = 0; i < Positionlist.Size(); i++) // Uses SizeType instead of size_t
    {
       
        Position = Positionlist[i]["currentQt"].GetDouble();  //
      
    }


    return 0;
}



double Bitmex_Order_Manager::getPosition(string symbol) // i think this should be a better function compared to the one above
{
    string filter = "{\"symbol\" : \" "+ symbol +"\"}";
    
    string getposition;
    int error = my_bitmex->getposition(getposition,filter);
    if(error != 0)
        return 0;
    
    
    
    Document Response;
    Response.Parse(getposition.c_str());
    
    
    if(getposition.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return 0;
        }
    }
    
    const Value& Positionlist = Response;
    
    for (SizeType i = 0; i < Positionlist.Size(); i++) // Uses SizeType instead of size_t
    {
        
        double Position = Positionlist[i]["currentQt"].GetDouble();  //
        return Position;
    }
    
    
    return 0;
}




int Bitmex_Order_Manager::getPositions()
// Retrieve the account info
{
    
    string getposition;
    int error = my_bitmex->getposition(getposition);
    if(error != 0)
        return error;
    
    
    
    Document Response;
    Response.Parse(getposition.c_str());
    
    
    if(getposition.substr(0,1)=="["){
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    const Value& Positionlist = Response;
    
    for (SizeType i = 0; i < Positionlist.Size(); i++) // Uses SizeType instead of size_t
    {
        
        double Position = Positionlist[i]["currentQty"].GetDouble();  //
        string symbol = Positionlist[i]["symbol"].GetString();
        double Position_order = Positionlist[i]["openingQty"].GetDouble() - Position;
        CPosition.insert(make_pair(symbol,Position));
        CPosition_in_order.insert(make_pair(symbol,Position));
        
    }
    
    return 0;


}



int Bitmex_Order_Manager::getAccount()
{

    
    string getaccount ="";
    int error = my_bitmex->getmargin("XBt", getaccount);
    
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(getaccount.c_str());
    
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    

    //Account my_account;  // should be stored in class rather than here
    my_account.balance = Response["walletBalance"].GetDouble();
    my_account.margin = Response["marginBalance"].GetDouble();
    my_account.availableMargin = Response["availableMargin"].GetDouble();
    my_account.nav = 0;  // do not know which one is the nav
    
    /*
     
     {"account":22046,"currency":"XBt","riskLimit":1000000000000,"prevState":"","state":"","action":"","amount":9968868,"pendingCredit":0,"pendingDebit":0,"confirmedDebit":0,"prevRealisedPnl":-337814,"prevUnrealisedPnl":0,"grossComm":7592,"grossOpenCost":0,"grossOpenPremium":0,"grossExecCost":0,"grossMarkValue":1183666,"riskValue":1183666,"taxableMargin":0,"initMargin":0,"maintMargin":196217,"sessionMargin":0,"targetExcessMargin":0,"varMargin":0,"realisedPnl":-334592,"unrealisedPnl":-34626,"indicativeTax":0,"unrealisedProfit":0,"syntheticMargin":0,"walletBalance":9634276,"marginBalance":9599650,"marginBalancePcnt":1,"marginLeverage":0.1233030370898939,"marginUsedPcnt":0.0204,"excessMargin":9403433,"excessMarginPcnt":1,"availableMargin":9403433,"withdrawableMargin":9403433,"timestamp":"2018-01-09T10:53:21.225Z","grossLastValue":1183666,"commission":null}
     
     
     */
    
    
    


    return 0;
}


//============== URL SELF-DESIGNED FUNCTION ===============//

int Bitmex_Order_Manager::publicRequest(const string &method, const string &UrlEndPoint, const string &params, string &result){
    int publicrequest;
   
    publicrequest = my_bitmex->DoGETrequest(UrlEndPoint, params, result);
    
    return publicrequest;
}

int Bitmex_Order_Manager::privateRequest(const string &method, const string &UrlEndPoint, const string &params, string &result)
{
    // the only difference between private and public in cexio is that we need to pass the authentication as parameters for the get and post
    
    string my_nonce = my_bitmex -> getTonce();
    string my_signature;
    string my_verb = method;
    string endpoint = UrlEndPoint;
    string myparams = params;
    my_bitmex -> APIauthentication(my_nonce, my_signature, my_verb, endpoint, myparams);
    
    int privaterequest;
    if(method == "GET"){
        privaterequest = my_bitmex->DoAuthenticGETrequest(UrlEndPoint, myparams, my_nonce, my_signature, result);
        
    }else if(method == "POST"){
        privaterequest = my_bitmex->DoPOSTrequest(UrlEndPoint, myparams, my_nonce, my_signature, result);
    }else if(method == "PUT"){
        privaterequest = my_bitmex->DoPUTrequest(UrlEndPoint, myparams, my_nonce, my_signature, result);
    }else if(method == "DELETE"){
        privaterequest = my_bitmex->DoDELrequest(UrlEndPoint, myparams, my_nonce, my_signature, result);
    }else{
    
        return -200;
    }
    return privaterequest;
    
}



int Bitmex_Order_Manager::sendOrderbulk(string symbol, vector<double> price, vector<double> quantity,vector<string> clOrdIDs, vector<string> execInst, vector<string> OrderType){
    

    string sendOrderResponse = "";
    std::string type;
    
    int error = my_bitmex->placeorderbulk( symbol,  price, quantity,  clOrdIDs, execInst, OrderType, sendOrderResponse );
    //cout <<endl<< sendOrderResponse <<endl;
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(sendOrderResponse.c_str());
    
    if(sendOrderResponse.substr(0,1)=="["){
        const Value& orderlist = Response;
        for(int i=0; i<Response.Size(); i++){
            if(Response[i]["ordStatus"].GetString() !="New")
                return -55;
        }
        return 0;
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    return 0;
    
    return 0;
}

int Bitmex_Order_Manager::sendOrder(string symbol, double price, double quantity,std::string clOrdID, std::string execInst, string tif, bool isMarket){
    
    string sendOrderResponse = "";
    std::string type;
    string ordertype = "Limit";
    if(isMarket)
        ordertype = "Market";
    
    int error = my_bitmex->placeorder(symbol,price,quantity,clOrdID, execInst,ordertype, sendOrderResponse );
    
    //cout <<endl<< sendOrderResponse <<endl;
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(sendOrderResponse.c_str());
    
    if(Response.HasMember("error")){
        cout << Response["error"]["message"].GetString() <<endl;
        return -100;
    }
    return 0;
}
        
        
int Bitmex_Order_Manager::cancelOrderbulk_clOrdID(vector<string> clOrdIDs){
    
    string cancelOrderResponse ="";
    int error = my_bitmex->cancelordersbulk_ClordID(clOrdIDs,cancelOrderResponse);
    
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(cancelOrderResponse.c_str());
    
    if(cancelOrderResponse.substr(0,1)=="["){
        const Value& orderlist = Response;
        for(int i=0; i<Response.Size(); i++){
            if(Response[i]["ordStatus"].GetString() !="Canceled")
                return -55;
        }
        return 0;
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    return 0;
    
}


int Bitmex_Order_Manager::cancelOrderbulk_orderID(vector<string> orderIDs){
    
    string cancelOrderResponse ="";
    int error = my_bitmex->cancelordersbulk_orderID(orderIDs,cancelOrderResponse);
    
    if(error != 0)
        return error;
    
    Document Response;
    Response.Parse(cancelOrderResponse.c_str());
    
    if(cancelOrderResponse.substr(0,1)=="["){
        const Value& orderlist = Response;
        for(int i=0; i<Response.Size(); i++){
            if(Response[i]["ordStatus"].GetString() !="Canceled")
                return -55;
        }
        return 0;
        
    }else{
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
    }
    
    return 0;
    
}


int Bitmex_Order_Manager::withdraw(string currency, double amount,string address,string otpToken){
   
        
        string withdrw_message = "";
        std::string type;
        
    int error = my_bitmex->requestWithdraw(currency, amount, address, withdrw_message);
        //cout <<endl<< sendOrderResponse <<endl;
        if(error != 0)
            return error;
        
        Document Response;
        Response.Parse(withdrw_message.c_str());
        
        if(Response.HasMember("error")){
            cout << Response["error"]["message"].GetString() <<endl;
            return -100;
        }
        return 0;
    
    
}





