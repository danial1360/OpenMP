#include "Stock.h"
 
Stock::Stock(std::string stock_symbol, BuyorSell  buy_or_sell, Type type, int  quantity, double  trade_price, double last_dividend, double fixed_Dividend, double par_value, double market_price) :
m_stock_symbol{ stock_symbol },
m_quantity{quantity},
m_trade_price{ trade_price },
m_buy_or_sell{buy_or_sell},
m_last_dividend{ last_dividend },
m_fixed_Dividend{ fixed_Dividend },
m_par_value{ par_value },
m_market_price{ market_price },
m_last_error{ Lasterror(0) },
m_type{ type }  
{
	// make sure that the quantity is not 0 or negative . if it is set the last error code and throw and exception
	if (quantity <= 0)
	{
		m_last_error = Lasterror::INVALID_QUANTITY;
		BOOST_THROW_EXCEPTION(invalid_quantity{});
		 
		//throw std::exception("the quantity cant be negative or zero!!\n");

	}
	m_timestamp = time(NULL); // set the time stamp

}
// this function calculates the Dividend Yield
double Stock::calculateDividendYield()  const
{
	if (m_market_price == 0) 
	{
		m_last_error = INVALID_MARKET_PRICE;
		//throw std::overflow_error("Divide by zero exception -  the market price cant be 0!!");
		BOOST_THROW_EXCEPTION(invalid_marketprice{});
	}
	if (m_type == Type::COMMON)
	{
		//Common 
				return m_last_dividend / m_market_price;
	 }
	else
	{
		//Preferred
		return m_fixed_Dividend*m_par_value / m_market_price;

	}
}
// this function calculates the P_E_Ratio
double Stock::calculateP_E_Ratio()  const
{
	if (m_last_dividend == 0)
	{
		m_last_error = INVALID_DIVIDEND;
		//throw std::overflow_error("Divide by zero exception -  the dividend cant be 0!!");
		BOOST_THROW_EXCEPTION(invalid_dividend{});
	}
	return   m_market_price / m_last_dividend;
 }
// this is use to generate a sting type report from the object
std::string  Stock::to_str() const
{
	std::stringstream buffer;
	 buffer << *this;
	 return buffer.str();
}
// this is use to generate a sting type report from the object
std::ostream & operator<<(std::ostream &os, const Stock& dy)
{
	auto temp = dy.gettimestamp();// to solve rvalue problem in localtime function
	return os <<
		"The Stock name is:  " << dy.getStockSymbol() << '\n'<<
		"it was " <<( (dy.getBuyorSell()==BuyorSell::BUY ) ? "bought " : "sold ") << "at:" <<
		asctime(localtime(&temp)) << 
		"the trade price was: " << dy.getTradePrice() << "p\n" <<
		"The stock symbol is: " << (dy.getMethod() ? "Common \n" : "Preferred \n") <<
		"the Last Dividend value is:" << dy.getLastDividend() << '\n'<<
		"the Fixed Dividend value is: " << dy.getFixedDividend() << '\n' <<
		"the Par Value is:" << dy.getParValue() << '\n' <<
		"The trade price is: " << dy.getTradePrice() << '\n' <<
		"The Dividend Yield is: " <<  dy.calculateDividendYield() << '\n' <<
		"The P/E Ratio is:" <<
		((dy.getLastDividend() == 0) ? "Dividend is 0 so it is not possible to calculate the P/E Ratio" : std::to_string(dy.calculateP_E_Ratio())) << '\n';
}

 
