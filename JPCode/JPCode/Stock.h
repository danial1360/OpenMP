#pragma once
#include <iostream>
 
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream> 
#include <Boost\system\error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/exception/all.hpp>
#include <exception>
#include <boost/intrusive/list.hpp>
 

// exception structures

struct invalid_marketprice : public std::exception
{
	const char *what() const   { return "invalid marketprice"; }
};
struct invalid_dividend : public std::exception
{
	const char *what() const { return "invalid dividend"; }
};
struct invalid_quantity : public std::exception
{
	const char *what() const { return "invalid quantity"; }
};
//error codes
enum Lasterror {
	NO_ERROR = 0,
	INVALID_MARKET_PRICE,
	INVALID_DIVIDEND,
	INVALID_QUANTITY
};
// error category object
class JP_error_category :
	public boost::system::error_category
{
	public :
	const char *name() const throw(){ return "JP stock obj error"; }
	std::string message(int ev) const
	{
		switch (ev)
		{
		NO_ERROR:
			return "no error";
		break;
		INVALID_MARKET_PRICE :
			return "Invalid market price";
			break;
		INVALID_DIVIDEND :
			return "invalid dividend";
			break;
		INVALID_QUANTITY :
			return "invalid quantity";
			break;
		default:
			return "unknown error";
			break;
		}
		 
	}
};
// this enum is used to show if the stock is soled or is bought
enum BuyorSell {
	BUY =0,
	SELL=1
};
// this enum is used to show the type of stock

enum Type { 
	COMMON=0,
	PEREFERED=1	
};


/* the main Stock class. this class wraps the information of each stock and provides function sto modify the stck
values and functions to do calculations*/
class Stock : public boost::intrusive::list_base_hook<> // to make it compatible with Boost.Intrusive
{
public:
	Stock()  =delete; // we cant not create an uninitialized objects
	explicit Stock(std::string stock_symbol, BuyorSell  buy_or_sell, Type type, int  quantity, double  trade_price, double last_dividend, double fixed_Dividend, double par_value, double market_price );// explicit is used to prevent type conversion 
	~Stock(){};
private: // member variables as defined in the assignment. all the member variables start with "m_"  
	double m_last_dividend;
	double m_fixed_Dividend;
	double m_par_value;
	double m_market_price;
	double m_trade_price;
	int m_quantity;
	std::string m_stock_symbol;
	time_t m_timestamp;
	Type m_type; 
	BuyorSell m_buy_or_sell;
	mutable Lasterror m_last_error; // mutable is used and we need to change this member in const functions
public: 
	// functions for setting the member variables values. their name indicates their function
	void setLastDividend(double last_dividend){ m_last_dividend = last_dividend; };
	void setFixedDividend(double fixed_Dividend){ m_fixed_Dividend = fixed_Dividend; };
	void setParValue(double par_value){ m_par_value = par_value; };
	void setMarketPrice(double market_price){ m_market_price = market_price; };
	void setMethod(Type type){ m_type = type; };
	void setStockSymbol(std::string stock_symbol){ m_stock_symbol = stock_symbol; };
	void settimestamp(tm *timeinfo)  { 	m_timestamp = mktime(timeinfo); };
	void settimestamp(time_t timeinfo)  { m_timestamp =  timeinfo ; };
	void setBuyorSell(BuyorSell  buy_or_sell){ m_buy_or_sell = buy_or_sell; };
	void setTradePrice(double trade_price){ m_trade_price = trade_price; };
	void setQuantity(int quantity){ m_quantity = quantity; };

	// functions for retrieving the member variables. their name indicates their function 
	double getLastDividend(void) const { return m_last_dividend; };
	double getFixedDividend(void) const{ return m_fixed_Dividend; };
	double getParValue(void)const { return m_par_value; };
	double getMarketPrice(void) const { return m_market_price; };
	std::string getStockSymbol(void) const{ return m_stock_symbol; };
	time_t gettimestamp(void) const { return m_timestamp; };
	void   gettimestamp(tm& timeinfo) const { tm *temp = localtime(&m_timestamp); timeinfo = *temp; };
	Type  getMethod() const { return m_type; };
	BuyorSell getBuyorSell() const{ return m_buy_or_sell; };
	boost::system::error_code GetLastError() const 	{boost::system::error_code er{ m_last_error, JP_error_category() }; return er;};
	double getTradePrice() const { return m_trade_price; };
	int getQuantity() const { return m_quantity; }

public:
	//these functions are for calculations . these functions might throw exceptions if there is a problem in the calculations
	double calculateDividendYield() const;
	double calculateP_E_Ratio() const;
	bool RecordaTrade();
	// these functions are used to generate a string type report from the object.
	std::string  to_str() const;
	friend std::ostream & operator<<(std::ostream &os, const Stock& dy);
};

