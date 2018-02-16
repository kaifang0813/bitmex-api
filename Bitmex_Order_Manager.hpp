//
//  Bitmex_Order_Manager.hpp
//  bfx-api-cpp
//
//  Created by kai fang on 1/9/18.
//  Copyright © 2018 jvr23. All rights reserved.
//

#ifndef Bitmex_Order_Manager_hpp
#define Bitmex_Order_Manager_hpp

#include <stdio.h>


#include "BitmexAPI.hpp"
#include "../Exchange.h"
#include <iostream>
#include <fstream>
#include "structs.h"

using namespace std;
using std::ifstream;

class Bitmex_Order_Manager:Exchange
{
    
    //friend class CexioAPI; // make the cexioapi the friend class of this class so as to call cexioapi member function
    
    
public:
    
    
    
    string accessKey = "";
    string secretKey ="";
   
    BitmexAPI* my_bitmex;               // used to point to the cexioapi object and used inside the class
    
    // used in the sendorder function to gather the order information you send
    
    
   
    vector<Contract> Contract_List;
    map<string, double> CPosition ;
    map<string, double> CPosition_in_order ;
    Account my_account;
    
    
    // constructor
    
    // with inheritance
    explicit Bitmex_Order_Manager(string Bitmex_cong):Exchange(){
        
        logRest = Bitmex_cong;
        const char *keyFilePath = &logRest.c_str()[0];
        ifstream ifs(keyFilePath);
        if (!ifs.is_open())
        {
            cout << "Can't open file: " << logRest<< endl;
            
        }
        else
        {
            getline(ifs, accessKey);
            getline(ifs, secretKey);
        }
        
        //do not have any new thing yet
        my_bitmex= new BitmexAPI(accessKey,secretKey);
        
        
        
    }
    
    
    // destructor
    ~Bitmex_Order_Manager(){
        delete my_bitmex;
    };
    
    // get order book --  3 level
    int getOrderBook(string symbol, int depth = 25);
    // you could get order book according to your defined depth, and the default is 3
    // Send a new order
    int sendOrder(string symbol, double price, double quantity,string tif, bool isMarket);
    // what is the meaning of tif? I think isMarket mean whether it is market order
    // Cancel order
    int cancelOrder(string orderId);
    // Cancel all orders
    int cancelAllOrders();
    int cancelAllOrders_with_type( string type);  // type should be "buy" or "sell"
    // Get order status
    int getOrder(string orderId);
    int getAllOrders();
    int getAllOpenOrders();
    // Retrieve position
    double getPosition(string symbol);
    int getPosition(string symbol, double &Position); // i think this should be a better function compared to the one above
    int getPositions();
    // Retrieve the account info
    int getAccount();
    // get contracts information
    int getContracts();
    
    
    // REST methods -- i keep these here in case that you want to desize the rest request by your self rather than directly use api function
    int publicRequest(const string &method, const string &UrlEndPoint, const string &params, string &result);
    int privateRequest(const string &method, const string &UrlEndPoint, const string &params, string &result);
    
    // new function added in api lead to the following function in the order manager adapter
    
    
    int sendOrderbulk(string symbol, vector<double> price, vector<double> quantity,vector<string> clOrdIDs, vector<string> execInst, vector<string> OrderType);
    
    int sendOrder(string symbol, double price, double quantity,string clOrdID, string execInst, string tif, bool isMarket);
    int cancelOrderbulk_clOrdID(vector<string> clOrdIDs);
    int cancelOrderbulk_orderID(vector<string> orderIDs);
    
    int withdraw(string currency, double amount,string address,string otpToken="");

    
    
    
    
private:
    
    // CexioManager object cannot be copied
    Bitmex_Order_Manager(const Bitmex_Order_Manager&);
    Bitmex_Order_Manager &operator=(const Bitmex_Order_Manager&);
    
    
};






#endif /* Bitmex_Order_Manager_hpp */
