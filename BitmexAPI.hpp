//
//  BitmexAPI.hpp
//  bfx-api-cpp
//
//  Created by kai fang on 1/7/18.
//  Copyright © 2018 jvr23. All rights reserved.
//

// documentation for the following code could be found in https://testnet.bitmex.com/api/explorer/

#ifndef BitmexAPI_hpp
#define BitmexAPI_hpp


#include <iostream>
#include <curl/curl.h>
#include <vector>
#include <map>


using std::string;
using std::vector;
using std::map;
using std::pair;

class BitmexAPI
{
public:
    
    // Enumerations
    enum BitmexERR // positive values for curl internal error codes
    {
        curlERR = -50,
        badSymbol = -40,
        badCurrency = -39,
        badDepositMethod = -38,
        badWalletType = -37,
        requiredParamsMissing = -36,
        wireParamsMissing = -35,
        addressParamsMissing = -34,
        badOrderType = -33,
        vectorTooLarge = -60,
        badStatus = -55,
        connectionfail = -100,
        noavailableinfo = -200,
        badordertype=-201
    };
    
    // Structure for multiple new orders endpoint
    
    // Constructor - destructor
    explicit BitmexAPI(const string &accessKey, const string &secretKey);
    ~BitmexAPI();
    
    // ============== Announcement Information ===============//
    
    int getAnouncement(string & result);
    int getUrgentAnouncement(string & result);
    
    //============ funding =====//
    int getfunding(string & result);
    
    //=========== Execution =========//
    
    int getRawExecutions(string &result, string filter="");
    int getExecutionsHistory(string &result);

    //=============instrument ========//
    
    int getinstrument(string & result);
    int getinstrumentactive(string &result);
    int getinstrumentactiveandindices(string & result);
    int getinstrumentactiveintervals(string & result);
    int getinstrumentcompositeindex(string & result);
    int getinstrumentindices(string &result);
    
    //============insurance ==========//
    int getinsurance(string&result);
    
    
    //========== level 2 order book ==========//
    
    int getorderbook(string symbol , string & result,int depth = 25);
    
    //==============ORDER====================//
    int getorder(string & result, string filter = "{\"open\":true}");
    int changeorder(string orderid, double price, double quantity, string & result);
    int changeorderPrice(string orderid, double price, string & result);
    int changeorderQuantity(string orderid,  double quantity, string & result);
    int placeorder(string symbol, double price, double quantity, string &result, string ordertype = "Limit");
    // ====== add--new  Feb 9
    int getorder(string symbol, string & result,string filter = "{\"open\":true}");
    // note: execInst: Optional execution instructions. Valid options: ParticipateDoNotInitiate, AllOrNone, MarkPrice, IndexPrice, LastPrice, Close, ReduceOnly, Fixed. 'AllOrNone' instruction requires displayQty to be 0. 'MarkPrice', 'IndexPrice' or 'LastPrice' instruction valid for 'Stop', 'StopLimit', 'MarketIfTouched', and 'LimitIfTouched' orders.
    int cancelordersbulk_orderID(vector<string> orderIDs, string& result);
    int cancelordersbulk_ClordID(vector<string> ClordIDs, string& result);
   

    int placeorderbulk(string symbol, vector<double> price, vector<double>quantity, vector<string> clOrdID, string &result, vector<string> execInst = NullexecInst, string Ordertype = "Limit");
    
    // market order
    int placeorder(string symbol, double price, double quantity, string clOrdID, string execInst, string ordertype ,string &result);// for both market order and limit order
    int placeorderbulk(string symbol, vector<double> price, vector<double>quantity, vector<string> clOrdID,  vector<string> execInst, vector<string> Ordertype, string &result); // for both market order and limit order
    //===================
    int cancelorder(string orderid, string&result);
    int cancelallorder(string &result);
    int cancelallorder(string &result, string filter);

    //==============margin trading -- by change leverage ============//
    int getposition(string&result, string filter = "");
    int isolateleverage(string isolateornot, string symbol, string &result); // whether allow isolate leverage for each position
    int chooseleverage(string& result, string symbol, double leverage);
    int transfermargin(string & result, string symbol, double amount);
    
    //================user data ============//
    int getwallet(string currency, string &result);
    int getmargin(string currency, string & result);
    int getcommission(string & result);
    
    //===========
    //deposit into wallet
    //==== do not find anything about how to deposit into wallet
    //withdraw from wallet
    int requestWithdraw(string currency, double amount,string address, string& result,string otpToken="");
    int confirmWithdraw(string token, string& result);
    int cancelWithdraw(string token, string& result);
    
    
    
    
    //=============== URL REQUEST RELATIVE MEMBER AND FUNCTION ============================//
    
    // make these public in order to be access in inheritance
    string APIurl;
    string accessKey, secretKey, UserID;
    CURL *curl;
    CURLcode res;
    map<string, double> PricePrecisionmap;
    
    //make these public in order to be access in inheritance
    int DoGETrequest(const string &UrlEndPoint, const string &params, string &result);
    int DoAuthenticGETrequest(const string &UrlEndPoint, const string &params, string& nonce, string& signature, string &result);
    int DoPOSTrequest(const string &UrlEndPoint, const string &params, string& nonce, string& signature, string &result);
    int DoDELrequest(const string &UrlEndPoint, const string &params, string &nonce,string &signature, string &result);
    int DoPUTrequest(const string &UrlEndPoint, const string &params, string &nonce,string &signature, string &result);
    static string getTonce();
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    void APIauthentication(string & nonce, string&signature, string& verb, string& path, string &data);
    
    
    
private:
    
    // CexioAPI object cannot be copied
    BitmexAPI(const BitmexAPI&);
    BitmexAPI &operator=(const BitmexAPI&);

    // Utility private methods
    static vector<string> NullexecInst; // empty vector for execInst vector
    int parseWDconfParams(string &params);
    static string bool2string(const bool &in);
    static bool inArray(const string &value, const vector<string> &symbols);
    string URLParser(string jsonstring);

};
#endif /* BitmexAPI_hpp */
