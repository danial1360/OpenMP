// JPCode.cpp : Defines the entry point for the console application.
//

#include "Stock.h"
#include <vector>
#include <algorithm> 
#include <math.h>
 
int main(void)
{
	/*we get the current time and we will use it to compare the time stamp of the created stock objects with the current time
	/ that is used for section iv:	Calculate Volume Weighted Stock Price based on trades in past 15 minutes*/
	std::time_t currenttime = std::time(NULL);
	tm * currenttimeinfo;
	currenttimeinfo = localtime(&currenttime);
	// the storage store all the created stock objects
	std::vector<Stock> storage;
	//last_15min_stock_items store those objects that were created in the last 15 min
	std::vector<Stock> last_15min_stock_items;

	/* create the first objects similar to what was asked in the assignment 
	I have assumed that the 1 stock is soled and the price is 2000p and the market price is 11p */
	Stock TEA = Stock("TEA", BuyorSell::SELL, Type::COMMON,1,   2000, 0, 0, 100, 11);
	/* we change the time stamp of this object manually so we can assume the object  
	 was created 3 hours a go*/
	currenttimeinfo->tm_hour = currenttimeinfo->tm_hour - 3;// so it was created 3 hours ago!!
	TEA.settimestamp(currenttimeinfo); // so the TEA was created 3 hours ago!!

	/* create the second objects similar to what was asked in the assignment
	I have assumed that the 5 stocks are bought and the trade price is 2000p and the market price is 11p */
	Stock POP = Stock("POP", BuyorSell::BUY, Type::COMMON,5, 2000, 8, 0, 100, 11);
	/* we change the time stamp of this object manually so we can assume the object
	was created 1 hours a go*/
	currenttimeinfo = localtime(&currenttime);
	currenttimeinfo->tm_hour = currenttimeinfo->tm_hour - 1;// so the POP was created 1 hours ago!!
	POP.settimestamp(currenttimeinfo);

	/* create other objects. we don't change the time stamp of these objects so actually 
	they are created in the last 15min*/
	Stock ALE = Stock("ALE", BuyorSell::SELL, Type::COMMON,10,2000, 23, 0, 60, 11);
	Stock GIN = Stock("GIN", BuyorSell::BUY, Type::PEREFERED,15,  2000 ,8, 2 / 100, 100, 11);
	Stock JOE = Stock("JOE", BuyorSell::SELL, Type::PEREFERED,1,  2000, 13, 0, 100, 11);
	/* the streaming operator was overload for the Stock class. this can be used to generate a report 
	from the objects. here we create a report from one object to show how this works*/
	std::cout <<TEA << POP << ALE << GIN << JOE;
	std::cout << "================================\n";
	/* save the objects in a container. it should be noted that std::vector is not a effective container if
	number of objects are very huge!! if you want to save like a huge amount of objects use Boost.Intrusive  */
	storage.push_back(TEA);
	storage.push_back(POP);
	storage.push_back(ALE);
	storage.push_back(GIN);
	storage.push_back(JOE);
	/* now we have to initialize the second container which is going to be used to store objects that were
	created in the last 15min*/
	last_15min_stock_items.reserve(storage.size());
	/* get the current time to compare the time stamp of the objects with current time
	so we can understand which objects were created in the last 15min*/
	time_t currentime = time(NULL);

	//filter the objects if they were created in the last 15 min
	std::remove_copy_if(storage.begin(), storage.end(), std::back_inserter(last_15min_stock_items),
		[=](Stock const stk)
	{
		double a = difftime(currentime, stk.gettimestamp());
		if (a < 15)
		{

			return false;
		}
		else
		{
			return true;
		}
	 }

	);
	// now only those objects that were created in the last 15min ago are saved in last_15min_stock_items
	
	
	// Calculate the Geometric Mean 
	double Geometric_Mean = 1;
	for (std::vector<Stock>::iterator it = last_15min_stock_items.begin(); it != last_15min_stock_items.end(); ++it)
	{
		Geometric_Mean = Geometric_Mean*it->getTradePrice();
	}
	Geometric_Mean = std::pow(Geometric_Mean, last_15min_stock_items.size());
	std::cout << "the Geometric Mean is: " << Geometric_Mean << '\n';

	// Calculate the Volume Weighted Stock Price
	double sumn=0; // the sum for the numerator
	double sumdn = 0; // the sum for denominator

	for (std::vector<Stock>::iterator it = last_15min_stock_items.begin(); it != last_15min_stock_items.end(); ++it)
	{
		sumn = sumn+ (it->getTradePrice())*(it->getQuantity());
		sumdn = sumdn + it->getQuantity();
	}
	std::cout << "Volume Weighted Stock Price is: " << (sumn / sumdn) << '\n';
	 
 
	printf("Press Any Key To Exit...");
	std::cin.get();

}


 
	 

 

