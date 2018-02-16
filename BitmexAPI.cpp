//
//  BitmexAPI.cpp
//  bfx-api-cpp
//
//  Created by kai fang on 1/7/18.
//  Copyright © 2018 jvr23. All rights reserved.
//


//base url :https://www.bitmex.com/api/v1
//documentation  https://www.bitmex.com/api/explorer/#!/Order/Order_new


#include "BitmexAPI.hpp"
#include <iostream>
#include <utility>
#include <map>
#include <fstream>
#include <regex>
#include <chrono>
#include <openssl/hmac.h>
#include "openssl/sha.h"
#include "openssl/hmac.h"
#include "base64.h"
#include <openssl/evp.h>
#include "hex_str.hpp"
#include <openssl/x509.h>
#include <openssl/crypto.h>
using std::cout;
using std::vector;
using std::to_string;


//////////////////////////////////////////////////////////////////////////////
// Constructor - destructor
//////////////////////////////////////////////////////////////////////////////


BitmexAPI::
BitmexAPI(const string &accessKey, const string &secretKey):
accessKey(accessKey), secretKey(secretKey) , curl(NULL)
{
    curl = curl_easy_init();
    APIurl = "https://testnet.bitmex.com";
    // if you use testnet api key to do the test, you should use APIurl = "https://testnet.bitmex.com";
    //cout << secretKey <<std::endl;
    
}


BitmexAPI::
~BitmexAPI()
{
    curl_easy_cleanup(curl);
}


//////////////////////////////////////////////////////////////////////////////
////////////////////// Utility private methods
//////////////////////////////////////////////////////////////////////////////


// bool to string cast
string BitmexAPI::
bool2string(const bool &in)
{
    return in ? "true" : "false";
}


// Tonce
string BitmexAPI::
getTonce()
{
    
    using namespace std::chrono;
    
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    
    return to_string(ms.count());
    
}


// Curl write callback function. Appends fetched *content to *userp pointer.
// *userp pointer is set up by curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result) line.
// In this case *userp will point to result.
size_t BitmexAPI::
WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
    
}


// Search in vector of strings
bool BitmexAPI::
inArray(const string &value, const vector<string> &symbols)
{
    return std::find(symbols.begin(), symbols.end(), value) != symbols.end();
}


string BitmexAPI::URLParser(string jsonstring){

    char* output = curl_easy_escape(curl, jsonstring.c_str(), jsonstring.size());
    return string(output);
    
}



int  BitmexAPI::
DoGETrequest(const string &UrlEndPoint, const string &params, string &result)
{
    
    if(curl) {
        
        string url = APIurl + UrlEndPoint + params;
        //std:: cout << url << std::endl;
        struct curl_slist *httpHeaders = NULL;
        
        httpHeaders = curl_slist_append(httpHeaders, "Content-Type: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"X-Requested-With: XMLHttpRequest");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        res = curl_easy_perform(curl);
        
        // libcurl internal error handling
        if (res != CURLE_OK)
        {
            cout << "Libcurl error in DoGETrequest(), code:\n";
            return res;
        }
        return res;
        
    }
    else
    {
        // curl not properly initialized curl = NULL
        return curlERR;
        
    }
}


int  BitmexAPI::
 DoAuthenticGETrequest(const string &UrlEndPoint, const string &params, string& nonce, string& signature, string &result)
{
    
    if(curl) {
        
        string url = APIurl + UrlEndPoint + params;
        //std:: cout << url << std::endl;
        struct curl_slist *httpHeaders = NULL;
        httpHeaders = curl_slist_append(httpHeaders, ("api-nonce:" + nonce).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-signature:" + signature).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-key:" + accessKey).c_str());
        
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        res = curl_easy_perform(curl);
        curl_slist_free_all(httpHeaders);

        // libcurl internal error handling
        if (res != CURLE_OK)
        {
            cout << "Libcurl error in DoGETrequest(), code:\n";
            return res;
        }
        return res;
        
    }
    else
    {
        // curl not properly initialized curl = NULL
        return curlERR;
        
    }
}




int BitmexAPI::
DoPOSTrequest(const string &UrlEndPoint, const string &params, string& nonce, string& signature, string &result)
{
    
    if(curl) {
        
        string url = APIurl + UrlEndPoint;

        
        // Headers
        struct curl_slist *httpHeaders = NULL;
        httpHeaders = curl_slist_append(httpHeaders, ("api-nonce:" + nonce).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-key:" + accessKey).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-signature:" + signature).c_str());
        httpHeaders = curl_slist_append(httpHeaders, "Content-Type: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"X-Requested-With: XMLHttpRequest");

        
        
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // debug option
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
       // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        res = curl_easy_perform(curl);
       // curl_slist_free_all(slist1);
        curl_slist_free_all(httpHeaders);
        
        
        // libcurl internal error handling
        if (res != CURLE_OK)
        {
            cout << "Libcurl error in DoPOSTrequest(), code:\n";
            return res;
        }
        return res;
        
    }
    else
    {
        // curl not properly initialized curl = NULL
        return curlERR;
        
    }
}

int BitmexAPI::
DoDELrequest(const string &UrlEndPoint, const string &params, string &nonce,string &signature, string &result){
    
    
    if(curl) {
        
        string url = APIurl + UrlEndPoint;
        struct curl_slist *httpHeaders = NULL;
        httpHeaders = curl_slist_append(httpHeaders, ("api-nonce:" + nonce).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-key:" + accessKey).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-signature:" + signature).c_str());
        httpHeaders = curl_slist_append(httpHeaders, "Content-Type: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"X-Requested-With: XMLHttpRequest");
        

        
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"DELETE");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // debug option
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        res = curl_easy_perform(curl);
        curl_slist_free_all(httpHeaders);
        
        
        // libcurl internal error handling
        if (res != CURLE_OK)
        {
            cout << "Libcurl error in DoPOSTrequest(), code:\n";
            return res;
        }
        return res;
        
    }
    else
    {
        // curl not properly initialized curl = NULL
        return curlERR;
    }
    
}



int BitmexAPI::
DoPUTrequest(const string &UrlEndPoint, const string &params, string &nonce,string &signature, string &result){
    
    
    if(curl) {
        
        string url = APIurl + UrlEndPoint;
        struct curl_slist *httpHeaders = NULL;
        httpHeaders = curl_slist_append(httpHeaders, ("api-nonce:" + nonce).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-key:" + accessKey).c_str());
        httpHeaders = curl_slist_append(httpHeaders, ("api-signature:" + signature).c_str());
        httpHeaders = curl_slist_append(httpHeaders, "Content-Type: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"Accept: application/json");
        httpHeaders = curl_slist_append(httpHeaders,"X-Requested-With: XMLHttpRequest");
        
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"PUT");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L); // debug option
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        res = curl_easy_perform(curl);
        curl_slist_free_all(httpHeaders);
        
        
        // libcurl internal error handling
        if (res != CURLE_OK)
        {
            cout << "Libcurl error in DoPOSTrequest(), code:\n";
            return res;
        }
        return res;
        
    }
    else
    {
        // curl not properly initialized curl = NULL
        return curlERR;
    }
    
}




//////////////////////////////////////////////////////////////////////////////
// Authentication
//////////////////////////////////////////////////////////////////////////////

void BitmexAPI::APIauthentication(string & nonce, string&signature, string& verb, string& path, string &data){
    
    
    nonce = getTonce();
    string message = verb + path + nonce + data;
    unsigned char* digest = HMAC (EVP_sha256(),
                                  secretKey.c_str(), strlen(secretKey.c_str()),
                                  (unsigned char*)message.c_str(), strlen(message.c_str()),
                                  NULL, NULL);
    signature = hex_str(digest, digest + SHA256_DIGEST_LENGTH);
    
    for(int i=0;i <signature.size(); i++){
        signature[i] = toupper(signature[i]);
        //cout<<signature[i];
    }
    
    
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                 Start Implementation of API
//
//////////////////////////////////////////////////////////////////////////////////////////////////

//// ===========================      Announcement      =============================//
int BitmexAPI::getAnouncement(string & result){

    string endPoint = "/api/v1/announcement";
    string params = "";
    
    return DoGETrequest(endPoint, params, result);
    

}

int BitmexAPI::getUrgentAnouncement(string & result){
    
    string endPoint = "/api/v1/announcement/urgent";
    string params = "";
    
    return DoGETrequest(endPoint, params, result);


}




//// ===========================      funding      =============================//
int BitmexAPI::getfunding(string & result){
    
    string endPoint = "/api/v1/funding";
    string params = "";
    
    return DoGETrequest(endPoint, params, result);
    
    
}
//------------------- for swap trading, this will give us the funding history of the swap ------//




//// ===========================      Executation      =============================//


/*This returns all raw transactions, which includes order opening and cancelation, and order status changes. It can be quite noisy. More focused information is available at /execution/tradeHistory. You may also use the filter param to target your query. Specify an array as a filter value, such as {"execType": ["Settlement", "Trade"]} to filter on multiple values.

*/

int BitmexAPI::getRawExecutions(string &result, string filter){
    string endPoint = "/api/v1/execution";
    if (filter != ""){
        string urlfilter = URLParser(filter);
        endPoint += "?filter=" + urlfilter;
    }
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    string params = "";

    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    cout << my_signature <<std::endl;
       return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);

}

// only return those trade relative order
int BitmexAPI:: getExecutionsHistory(string &result){
    string endPoint = "/api/v1/execution/tradeHistory";
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);
}

//=============================  Instrument  ===============================================//

int BitmexAPI::getinstrument(string & result){
    
    string endPoint = "/api/v1/instrument";
    string params = "";
    return DoGETrequest(endPoint, params, result);
    
}

int BitmexAPI::getinstrumentactive(string &result){

    string endPoint = "/api/v1/instrument/active";
    string params = "";
    return DoGETrequest(endPoint, params, result);
}

int BitmexAPI::getinstrumentactiveandindices(string & result){
    string endPoint = "/api/v1/instrument/activeAndIndices";
    string params = "";
    return DoGETrequest(endPoint, params, result);

}

int BitmexAPI::getinstrumentactiveintervals(string & result){
    string endPoint = "/api/v1/instrument/activeIntervals";
    string params = "";
    return DoGETrequest(endPoint, params, result);
}


int BitmexAPI::getinstrumentcompositeindex(string & result){
    string endPoint = "/api/v1/instrument/compositeIndex";
    string params = "";
    return DoGETrequest(endPoint, params, result);

}

int BitmexAPI::getinstrumentindices(string &result){
    string endPoint = "/api/v1/instrument/indices";
    string params = "";
    return DoGETrequest(endPoint, params, result);
    
}

//===================== insurance ====================//

int BitmexAPI::getinsurance(string &result){
    string endPoint = "/api/v1/instrument/insurance";
    string params = "";
    return DoGETrequest(endPoint, params, result);
}

//=========================  order =======================//
int BitmexAPI::getorder(string & result, string filter){
    string endPoint = "/api/v1/order";
    if (filter != ""){
        string urlfilter = URLParser(filter);
        endPoint += "?filter=" + urlfilter;
    }
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);

}

int BitmexAPI::changeorder(string orderid, double price, double quantity, string & result){
    
    string endPoint = "/api/v1/order";
    string params = "{";
    params += "\"orderID\":\""+orderid+"\",";
    params += "\"price\":"+to_string(price)+",";
    params += "\"orderQty\":"+to_string(quantity)+"";
    params += "}";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "PUT";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPUTrequest(endPoint, params, my_nonce, my_signature, result);
};

int BitmexAPI::changeorderPrice(string orderid, double price, string & result){
    string endPoint = "/api/v1/order";
    string params = "{";
    params += "\"orderID\":\""+orderid+"\",";
    params += "\"price\":"+to_string(price)+"";
    
    params += "}";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "PUT";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
   // cout << my_signature << std::endl;
    return DoPUTrequest(endPoint, params, my_nonce, my_signature, result);
};

int BitmexAPI::changeorderQuantity(string orderid, double quantity, string & result){
    string endPoint = "/api/v1/order";
    string params = "{";
    params += "\"orderID\":\""+orderid+"\",";
   
    params += "\"orderQty\":"+to_string(quantity)+"";
    params += "}";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "PUT";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPUTrequest(endPoint, params, my_nonce, my_signature, result);
};


int BitmexAPI:: placeorder(string symbol, double price, double quantity, string &result, string ordertype){

    string endPoint = "/api/v1/order";
    string type;
    
    //endPoint += "?symbol="+symbol +"&price="+to_string(price)+"&orderQty=" + to_string(quantity);
    //string params="";
    
    string params = "{";
    //params += "\"ordType\"=\""+ordertype+"\",";  // Market, Limit, MarketWithLeftOverAsLimit, Stop, StopLimit, MarketIfTouched, LimitIfTouched

    params += "\"symbol\":\""+symbol+"\",";
    params += "\"price\":"+to_string(price)+",";
    params += "\"orderQty\":"+to_string(quantity)+"";
    params += "}";
    
    
   // cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);

}



int BitmexAPI::cancelorder(string orderid, string&result){
    string endPoint = "/api/v1/order";
    string type;
    string params = "{";
    params += "\"orderID\":\""+orderid+"\"";
    params += "}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "DELETE";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoDELrequest(endPoint, params, my_nonce, my_signature, result);

}

int BitmexAPI::cancelallorder(string &result){
    string endPoint = "/api/v1/order/all";
    string type;
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "DELETE";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
   // cout << my_signature << std::endl;
    return DoDELrequest(endPoint, params, my_nonce, my_signature, result);

}


int BitmexAPI::cancelallorder(string &result, string filter){
    string endPoint = "/api/v1/order/all";
    if (filter != ""){
        string urlfilter = URLParser(filter);
        endPoint += "?filter=" + urlfilter;
    }
    
    string type;
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "DELETE";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    // cout << my_signature << std::endl;
    return DoDELrequest(endPoint, params, my_nonce, my_signature, result);
    
}

//==================== position ========================//

int BitmexAPI::getposition(string&result, string filter){  // filter could be symbol like {"symbole" = "XBTUSD"}
    string endPoint = "/api/v1/position";
    if (filter != ""){
        string urlfilter = URLParser(filter);
        endPoint += "?filter=" + urlfilter;
    }
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);
    
}


int BitmexAPI::isolateleverage(string isolateornot, string symbol, string &result){
    string endPoint = "/api/v1/position/isolate";
    
    string params = "{";
    params += "\"isolateornot\":"+isolateornot+",";
    params += "\"symbol\":\""+symbol+"\"";

    params += "}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);


}

int BitmexAPI::chooseleverage(string& result, string symbol, double leverage){
    string endPoint = "/api/v1/position/isolate";
    
    string params = "{";
    params += "\"leverage\":"+to_string(leverage)+",";
    params += "\"symbol\":\""+symbol+"\"";
    params += "}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);

}



int BitmexAPI::transfermargin(string & result, string symbol, double amount){ // use this function to move margin between different position when a posiiton is isolated
    
    string endPoint = "/api/v1/position/transferMargin";
    
    string params = "{";
    params += "\"amount\":"+to_string(amount)+",";
    params += "\"symbol\":\""+symbol+"\"";
    params += "}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
}


//================ orderbook=====================//
int BitmexAPI::getorderbook(string symbol , string & result,int depth){
    string endPoint = "/api/v1/orderBook/L2?symbol="+symbol+"&depth="+to_string(depth);
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature <<std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);


}

//=============== user ======================//

int BitmexAPI::getwallet(string currency, string &result){

    string endPoint = "/api/v1/user/wallet?currency="+currency;
    string params = "";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);



}

int BitmexAPI::getmargin(string currency, string & result){
    string endPoint = "/api/v1/user/margin?currency="+currency;
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
   // cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);



}

int BitmexAPI::getcommission( string & result){
    string endPoint = "/api/v1/user/commission";
    string params = "";
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);
    
}

//===========

// ====== add--new  Feb 9
int  BitmexAPI::getorder(string symbol, string & result,string filter){
    string endPoint = "/api/v1/order";
    if (filter != ""){
        string urlfilter = URLParser(filter);
        endPoint += "?filter=" + urlfilter;
    }
    string  params = "{";
    params += "\"symbol\":\""+symbol+"\"";
    params += "}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "GET";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return  DoAuthenticGETrequest(endPoint, params, my_nonce, my_signature, result);
    
    
    
}

// note: execInst: Optional execution instructions. Valid options: ParticipateDoNotInitiate, AllOrNone, MarkPrice, IndexPrice, LastPrice, Close, ReduceOnly, Fixed. 'AllOrNone' instruction requires displayQty to be 0. 'MarkPrice', 'IndexPrice' or 'LastPrice' instruction valid for 'Stop', 'StopLimit', 'MarketIfTouched', and 'LimitIfTouched' orders.
int  BitmexAPI::cancelordersbulk_orderID(vector<string> orderIDs, string& result){
    string endPoint = "/api/v1/order/bulk";
    string type;
    string params = "{\"orderID\":[";
    for(int i=0; i<orderIDs.size(); i++){
    params += "\""+orderIDs[i]+"\"";
        if(i<orderIDs.size()-1){
             params += ",";
        }
    }
    params += "]}";
    
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "DELETE";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoDELrequest(endPoint, params, my_nonce, my_signature, result);
    
    
    
    
}

int  BitmexAPI::cancelordersbulk_ClordID(vector<string> ClordIDs, string& result){
    string endPoint = "/api/v1/order";
    string type;
    string params = "{\"clOrdID\":[";
    for(int i=0; i<ClordIDs.size(); i++){
        params += "\""+ClordIDs[i]+"\"";
        if(i<ClordIDs.size()-1){
            params += ",";
        }
    }
    params += "]}";
    //cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "DELETE";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoDELrequest(endPoint, params, my_nonce, my_signature, result);
    
    
    
    
}
//cancel multiple order at bulk
int  BitmexAPI::placeorderbulk(string symbol, vector<double> price, vector<double>quantity, vector<string> clOrdID, string &result, vector<string> execInst, string Ordertype ){
    
    string endPoint = "/api/v1/order/bulk";
    string type;
    
    //endPoint += "?symbol="+symbol +"&price="+to_string(price)+"&orderQty=" + to_string(quantity);
    //string params="";
    
    string params ="{\"orders\":[";
    
    
    for(int i=0; i<price.size(); i++){
        params += "{";
        //params += "\"ordType\"=\""+ordertype+"\",";  // Market, Limit, MarketWithLeftOverAsLimit, Stop, StopLimit, MarketIfTouched, LimitIfTouched
        
        params += "\"symbol\":\""+symbol+"\",";
        params += "\"clOrdID\":\""+clOrdID[i]+"\",";
       
        if(execInst[i] !="")
          params += "\"execInst\":\""+execInst[i]+"\",";
        
        params += "\"price\":"+to_string(price[i])+",";
        params += "\"orderQty\":"+to_string(quantity[i])+"";
        params += "}";
    
        if(i<price.size()-1){
            params += ",";
        }
    }
    params += "]}";
    
    //cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
    
    
}



//deposit into wallet
//==== do not find anything about how to deposit into wallet
//withdraw from wallet
int BitmexAPI::requestWithdraw(string currency, double amount,string address, string& result,string otpToken){
    string endPoint = "/api/v1/user/requestWithdrawal";
    string type;
    
  
    
    string params = "{";
   
        params += "\"currency\":\""+currency+"\",";
        params += "\"address\":\""+address+"\",";
        params += "\"amount\":"+to_string(amount)+",";
    
    if(otpToken != "")
        params += "\"otpToken\":\""+otpToken+"\",";

        params += "}";
    
    // cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
}

int BitmexAPI::confirmWithdraw(string token, string& result){
    
    string endPoint = "/api/v1/user/confirmWithdrawal";
    string type;
    
    
    string params = "{";
    
    params += "\"token\":\""+token+"\"";
    params += "}";
    
    // cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
    
}

int BitmexAPI::cancelWithdraw(string token, string& result){
    string endPoint = "/api/v1/cancelWithdrawal";
    string type;
    
    
    string params = "{";
    
    params += "\"token\":\""+token+"\"";
    params += "}";
    
    // cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
    
    
}



int BitmexAPI::placeorder(string symbol, double price, double quantity, string clOrdID, string execInst, string ordertype ,string &result)// for both market order and limit order
{
    
    string endPoint = "/api/v1/order";
    string type;
    
    //endPoint += "?symbol="+symbol +"&price="+to_string(price)+"&orderQty=" + to_string(quantity);
    //string params="";
    
    string params = "{";
    //params += "\"ordType\"=\""+ordertype+"\",";  // Market, Limit, MarketWithLeftOverAsLimit, Stop, StopLimit, MarketIfTouched, LimitIfTouched
    
    params += "\"symbol\":\""+symbol+"\",";
    params += "\"clOrdID\":\""+clOrdID+"\",";
    if(execInst !="")
      params += "\"execInst\":\""+execInst+"\",";
    
    if(ordertype == "Limit"){
        params += "\"price\":"+to_string(price)+",";
    }else if (ordertype == "Market"){
        params += "\"ordType\":\""+ordertype+"\",";
        
    }else{
        
        return -201;
    }
    params += "\"orderQty\":"+to_string(quantity)+"";
    params += "}";
    
    
    // cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
    
    
}

int BitmexAPI::placeorderbulk(string symbol, vector<double> price, vector<double>quantity, vector<string> clOrdID,  vector<string> execInst, vector<string> Ordertype, string &result) // for both market order and limit order
{
    string endPoint = "/api/v1/order/bulk";
    string type;
    
    //endPoint += "?symbol="+symbol +"&price="+to_string(price)+"&orderQty=" + to_string(quantity);
    //string params="";
    
    string params ="{\"orders\":[";
    
    
    for(int i=0; i<price.size(); i++){
        params += "{";
        //params += "\"ordType\"=\""+ordertype+"\",";  // Market, Limit, MarketWithLeftOverAsLimit, Stop, StopLimit, MarketIfTouched, LimitIfTouched
        
        params += "\"symbol\":\""+symbol+"\",";
        params += "\"clOrdID\":\""+clOrdID[i]+"\",";
        
        if(execInst[i] !="")
            params += "\"execInst\":\""+execInst[i]+"\",";
        
        if(Ordertype[i] == "Limit"){
        params += "\"price\":"+to_string(price[i])+",";
        }else if (Ordertype[i] == "Market"){
            params += "\"ordType\":\""+Ordertype[i]+"\",";

        }else{
            
            return -201;
        }
        params += "\"orderQty\":"+to_string(quantity[i])+"";
        params += "}";
        
        if(i<price.size()-1){
            params += ",";
        }
    }
    params += "]}";
    
    //cout << params <<std::endl;
    string my_nonce = getTonce();
    string my_signature;
    string my_verb = "POST";
    APIauthentication(my_nonce, my_signature, my_verb, endPoint, params);
    //cout << my_signature << std::endl;
    return DoPOSTrequest(endPoint, params, my_nonce, my_signature, result);
    
    
    
}





