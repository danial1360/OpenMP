// JPCode.cpp : Defines the entry point for the console application.
//

#include "Stock.h"
#include <vector>
#include <algorithm> 
#include <math.h>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <omp.h>
#include <time.h>
#include <chrono>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/shared_ptr.hpp>
 
int main(void)
{
	std::chrono::system_clock::time_point begin, end; //timers
	std::cout <<"Example of exception handling" <<std::endl;
	Stock TEMP = Stock("TEA", BuyorSell::SELL, Type::COMMON, 1, 0, 0, 0, 0, 0);
	try //example of error handling 
	{
		
		TEMP.calculateDividendYield();
	}
	catch (boost::exception &e)
	{
		std::cerr << boost::diagnostic_information(e);
	}
	auto ss = TEMP.GetLastError(); // contains information about the last error happend in the TEMP object
	 
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
	Stock TEA = Stock("TEA", BuyorSell::SELL, Type::COMMON,1,   11, 0, 0, 1, 10);
	/* we change the time stamp of this object manually so we can assume the object  
	 was created 3 hours a go*/
	currenttimeinfo->tm_hour = currenttimeinfo->tm_hour - 3;// so it was created 3 hours ago!!
	TEA.settimestamp(currenttimeinfo); // so the TEA was created 3 hours ago!!

	/* create the second objects similar to what was asked in the assignment
	I have assumed that the 5 stocks are bought and the trade price is 2000p and the market price is 11p */
	Stock POP = Stock("POP", BuyorSell::BUY, Type::COMMON,5, 5, 1, 0, 1, 10);
	/* we change the time stamp of this object manually so we can assume the object
	was created 1 hours a go*/
	currenttimeinfo = localtime(&currenttime);
	currenttimeinfo->tm_hour = currenttimeinfo->tm_hour - 1;// so the POP was created 1 hours ago!!
	POP.settimestamp(currenttimeinfo);

	/* create other objects. we don't change the time stamp of these objects so actually 
	they are created in the last 15min*/
	Stock ALE = Stock("ALE", BuyorSell::SELL, Type::COMMON,10,6, 5, 0, 1, 10);
	Stock GIN = Stock("GIN", BuyorSell::BUY, Type::PEREFERED,15,2.3 ,1, 2 / 1.1, 1.3, 10);
	Stock JOE = Stock("JOE", BuyorSell::SELL, Type::PEREFERED,1,  4, 2, 0, 1, 10);
	/* the streaming operator was overload for the Stock class. this can be used to generate a report 
	from the objects. here we create a report from one object to show how this works*/
	//std::cout <<TEA << POP << ALE << GIN << JOE;
	std::cout << "================================\n";
	// using a logger to get a log of the existing objects	 
	typedef  boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
	boost::shared_ptr<std::ostream> stream{ &std::clog,
		boost::empty_deleter{} };
	sink->locked_backend()->add_stream(stream);
	boost::log::core::get()->add_sink(sink);
	boost::log::sources::logger lg;
	BOOST_LOG(lg) <<"using asynchronous logger to log 2 the objects asynchronously \n" <<TEA << POP ;
	sink->flush();
	/* save the objects in a container. it should be noted that std::vector is not a effective container if
	number of objects are very huge!! */
	BOOST_LOG(lg) << "Storing 300000 stock objects in a memory using two different methods\n";
	begin = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < 300000; i++)
	{
		storage.push_back(TEA);
		storage.push_back(POP);
		storage.push_back(ALE);
		storage.push_back(GIN);
		storage.push_back(JOE);
		storage.push_back(ALE);
		storage.push_back(GIN);
		storage.push_back(JOE);
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "=====================\n";
	std::cout << "storage time using STL containers: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";

	//if you want to save like a huge amount of objects use Boost.Intrusive
	boost::intrusive::list<Stock> fast_storage;
	begin = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < 300000; i++)
	{
		fast_storage.push_back(TEA);
		fast_storage.push_back(POP);
		fast_storage.push_back(ALE);
		fast_storage.push_back(GIN);
		fast_storage.push_back(JOE);
		fast_storage.push_back(ALE);
		fast_storage.push_back(GIN);
		fast_storage.push_back(JOE);
	}
	end = std::chrono::high_resolution_clock::now();
	
	std::cout << "storage time using Boost.Intrusive: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";
	std::cout << "=====================\n";
	/* now we have to initialize the second container which is going to be used to store objects that were
	created in the last 15min*/
	last_15min_stock_items.reserve(storage.size());
	/* get the current time to compare the time stamp of the objects with current time
	so we can understand which objects were created in the last 15min*/
	time_t currentime = time(NULL);

	//filter the objects if they were created in the last 15 min, 
	std::cout << "Filtering objects created in the last 15min using 2 different methods\n";
	begin = std::chrono::high_resolution_clock::now();
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
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Filtering data using STL algorithm, the elapses time is: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";
	std::cout << "number of filtered objects using STL algorithm:" << last_15min_stock_items.size()<<std::endl;
	// now only those objects that were created in the last 15min ago are saved in last_15min_stock_items
	
	//we can also use boost adapter to filter the data so no extra copy are created adn there will be no need to last_15min_stock_items
	begin = std::chrono::high_resolution_clock::now();
	auto filtered_data = boost::adaptors::filter(storage, [=](Stock const stk)
	{
		double a = difftime(currentime, stk.gettimestamp());
		if (a < 15)
		{

			return true;
		}
		else
		{
			return false;
		}
	});
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Filtering data using boost adapter, the elapses time is : " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";
 
	std::cout << "number of filtered objects using boost adapter:" << std::distance(filtered_data.begin(), filtered_data.end())<<std::endl;
	std::cout << "=====================\n";
	double Geometric_Mean = 1;
	begin = std::chrono::high_resolution_clock::now();
	//...
	
	double nth = 1.0 / last_15min_stock_items.size();
#pragma omp parallel for num_threads (2) shared (last_15min_stock_items) firstprivate(nth)  reduction(*:Geometric_Mean)
	for (int it = 0; it <last_15min_stock_items .size(); ++it)
	{
		Geometric_Mean = Geometric_Mean *	pow(last_15min_stock_items.at(it).getTradePrice(), nth);
	}
/*following method  needs OpenMP3 !!! not working on visual studio 2013

	#pragma omp parallel for firstprivate(Geometric_Mean) reduction(*,Geometric_Mean)
	for (auto it = filtered_data.begin(); it != filtered_data.end(); ++it)
	{
	Geometric_Mean = Geometric_Mean*it->getTradePrice();
	}
*/
	end = std::chrono::high_resolution_clock::now();
	// Calculate the Geometric Mean 
		
	std::cout << "An Open MP approach for Geometric Mean, the  Geometric Mean is: " << Geometric_Mean << '\n';
	std::cout << "OpenMp total time is: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";
	Geometric_Mean = 1;
	begin = std::chrono::high_resolution_clock::now();
	for (std::vector<Stock>::iterator it = last_15min_stock_items.begin(); it != last_15min_stock_items.end(); ++it)
	{
		Geometric_Mean = Geometric_Mean*pow(it->getTradePrice(), nth);
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "A single thread approach for Geometric Mean, the  Geometric Mean is:" << Geometric_Mean << "ms\n";
	std::cout << "Single thread total time is: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";

	// Calculate the Volume Weighted Stock Price
	double sumn=0; // the sum for the numerator
	double sumdn = 0; // the sum for denominator
	begin = std::chrono::high_resolution_clock::now();
	for (std::vector<Stock>::iterator it = last_15min_stock_items.begin(); it != last_15min_stock_items.end(); ++it)
	{
		sumn = sumn+ (it->getTradePrice())*(it->getQuantity());
		sumdn = sumdn + it->getQuantity();
	}
	end = std::chrono::high_resolution_clock::now();
	
	std::cout << "\nVolume Weighted Stock Price is: " << (sumn / sumdn) << '\n';
	std::cout << "single thread total time is: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";

//OpenMP approach:
	 sumn = 0; // the sum for the numerator
	 sumdn = 0; // the sum for denominator
	 begin = std::chrono::high_resolution_clock::now();
#pragma omp parallel for num_threads (2) shared(last_15min_stock_items)  reduction(+:sumn) reduction(+:sumdn)
	for (int it = 0; it <last_15min_stock_items.size(); ++it)
	{
		sumn = sumn + (last_15min_stock_items.at(it).getTradePrice())*(last_15min_stock_items.at(it).getQuantity());
		sumdn = sumdn + last_15min_stock_items.at(it).getQuantity();
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Open MP approach using 2 thread, Volume Weighted Stock Price is: " << (sumn / sumdn) << '\n';
	std::cout << "OpenMp total time is: " << (std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)).count() << "ms\n";
	std::cout << "=============\n";
	std::cout << "Calculate the GBCE\nunfortualetely the formula for GBCE was not given in the homwork and I could find anyting on interent reagarding to GBCE!!\n";
	printf("Press Enter To Exit...");
	std::cin.get();

}


 
	 

 

