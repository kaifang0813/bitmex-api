#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

using namespace std;

// Structures used by the program
struct Contract {

	string exchange;
	string contract;  //
	string base;
	string quote;
	string symbolExchange; // do not know what is this, does it mean the symbol you use to represent the exchange, i use cexio for Cex.io
	double takerFees;
	double makerFees;
	double marginFees;      // the way cexio define margin fees is as below, and they do not have anything about initial margin or maintaince margin, once you could not pay the margin fee, they would close your position
    /*
     
     CEX.IO takes the following fees for margin trading:
     
     Position opening fee is 0.2% from position amount at current price
     Rollover fee 0.01% is taken from the rest of balance to prolong your position for next 4 hours:
     For long positions: Loan amount * 0.01%
     For short positions: Loan amount * Current price * 0.01%
     Please note that position will be closed automatically if the balance is not enough to cover the rollover fee.
     
     
     */
	double precisionPrice; //
	double precisionSize;
	double initMargin;// initMargin when the leverage is 2 --- open a position
    double maintMargin; // initMargin when the leverage is 2 --- holding a position
	double minOrderSize;
	double maxOrderSize;
	int isShort; // do not know the meaning
	double rateLimitPublic; // do not know the meaning
	double rateLimitPrivate; // do not know the meaning

};

struct Account {

	double balance;
	double nav;
	double margin;
	double availableMargin;

};

class Order {

public:

	string id;
	string clientId;
	string symbol;
	char side;
	string time;
	double timestamp;
	double price;
	double size;
	double filled;

	// Constructor
	Order(string orderId, string orderClientId, string orderSymbol, char orderSide, string orderTime, double orderTimestamp, double orderPrice, double orderSize, double orderFilled) {
		this->id = orderId;
		this->clientId = orderClientId;
		this->symbol = orderSymbol;
		this->side = orderSide;
		this->time = orderTime;
		this->timestamp = orderTimestamp;
		this->price = orderPrice;
		this->size = orderSize;
		this->filled = orderFilled;
	}
    
    

};





#endif // !STRUCTS_H

