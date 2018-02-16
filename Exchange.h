#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <string>
#include <stdio.h>
#include <vector>
/*
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/client.hpp>

#include "curl\curl.h"
#include "openssl\sha.h"
#include "openssl\hmac.h"
 */
#include "base64.h"
#include "hex_str.hpp"
//#include "utils.h"



#include "structs.h"
// #include "Websocket.h"

using namespace std;
/*
typedef void(*websocketpp_callbak_open)();
typedef void(*websocketpp_callbak_close)();
typedef void(*websocketpp_callbak_message)(const char *message);

//typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::client<websocketpp::config::asio_tls> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef client::connection_ptr connection_ptr;
*/
#define MAX_RETRY_COUNT		10000

enum CONNECTION_STATE {
	CONNECTION_STATE_UNKNOWN,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_DISCONNECTED,
	CONNECTION_STATE_CONNECTED
};


class Exchange {

public:

    // Exchange info
    string name;
    string restUrl;
    string wsUrl;
    string fixUrl;
    string publicKey;
    string privateKey;
    string symbol = "";
    double precisionPrice;
    double precisionSize;
    
    // Log file
    string outputFolder;
    string logFile; // not sure whether this is for rest, so i add logRest
    string logRest;
    string logTicker;
    string logTrades;
    string logBook;
    string logBookL3;
    string logLevel2;
    string logLevel3;
    string logMessage;
    string logFix;
    string logOrders;
    int nLevels = 10;
    
    FILE *fidLog;
    FILE *fidTicker;
    FILE *fidTrades;
    FILE *fidLevel2;
    FILE *fidLevel3;
    FILE *fidMessage;
    FILE *fidFix;
    FILE *fidOrders;
    
    /*   come from websocket
    // Ticker
    UINT64 lastSequence = 0;
    UINT64 sequence = 0;
    string time;
    double unixTime;
    double bidPrice;
    double bidSize;
    int bidNum;
    double askPrice;
    double askSize;
    int askNum;
    double lastTime;
    double lastPrice;
    double lastSize;
    UINT64 tradeId = 0;
    */
    // Level 2 order book
    vector<double> bidsPrice;
    vector<double> bidsSize;
    vector<int> bidsNum;// do not know what is this
    vector<double> asksPrice;
    vector<double> asksSize;
    vector<int> asksNum;// do not know what is this
    int nBids = 0;
    int nAsks = 0;
    /*
    // Level 3 order book   ------- do not find this information in rest api
    UINT64 bookSequence;
    bool isBookGood = false;
    bool isUpdateLevel2 = false;
    string lastTakerId = "";
    string lastReceivedId = "";
    UINT64 lastReceivedSequence;
    vector<double> bidOrdersPrice;
    vector<double> bidOrdersSize;
    vector<string> bidOrdersId;
    vector<double> askOrdersPrice;
    vector<double> askOrdersSize;
    vector<string> askOrdersId;
    int nBidOrders = 0;
    int nAskOrders = 0;
    */
    
    // Trades  -- should come from websocket?
    /*
    int nTrades;
    vector<double> tradesPrice;
    vector<double> tradesSize;
    */
    
    // Contracts  --- should be a vector rather than pointer, or initialized using dynamic allocation in constructor,
                     // do you mean all available contract in the exchange here?
    Contract *contracts;
    int nContracts;
    
    // Account
    double balance;
    double nav;    // to compute the nav, we need to have the price from websocket combined with the Cposition map to compute market value of the position, please refer to the getAccount function for more information
    
    double margin;
    double availableMargin;
    
    // Position   -- why this is uninitialized pointer?  in cexio, i have a map to represent the position called CPosition, maybe this is for your strategy implementation, for any position information, you could retrieve it from the Cposition map
    string *symbols;
    double *targetPosition;
    double *position;
    double *quantity;
    
    // Orders  --- it is better to use map or un_ordered_map here so as to make it easier to search for order when we cancel or place order
    vector<Order> orders;  // would update only if you use the getAllOrders function
    int nOrders = 0;
    string orderId; // i think this should be a vector for we have a vector of orders, but we already have this information in the vector<Order>
    
	// CURL for REST api
	CURL *curl;
	CURLcode res;
/*
	// Websocket client
	// WebSocket *wsClient;
	HANDLE hThread;

	// FIX client
	// TODO

	// Methods
	// Websocket API
*/

    
    
    // REST API or FIX API depending on the exchange
    // Retrieve the contracts
    virtual int getContracts() = 0;
    
    // Retrieve a snapshot of the order book
    virtual int getOrderBook(string symbol, int depth) = 0;
    
    // Retrieve the account info (balance, nav, etc.)
    virtual int getAccount() = 0;
    
    // Send a new order
    virtual int sendOrder(string symbol, double price, double quantity, string tif, bool isMarket) = 0;
    
    
    
    // Cancel order
    virtual int cancelOrder(string orderId) = 0;
    
    // Cancel all orders
    virtual int cancelAllOrders() = 0;
    
    // Get order status
    virtual int getOrder(string orderId) = 0;
    
    // Get all orders
    virtual int getAllOrders() = 0;
    
    // Retrieve position
    virtual int getPositions() = 0;
    virtual double getPosition(string symbol) = 0;

	// REST methods
	virtual int publicRequest(const string &method, const string &UrlEndPoint, const string &params, string &result) = 0;
	virtual int privateRequest(const string &method, const string &UrlEndPoint, const string &params, string &result) = 0;
    
	//static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);//  define as private function in api
	//static string getTonce();	// defined as private function in api
 

};


#endif // EXCHANGE_H
