#pragma once

#include "OrderBookEntry.hpp"
#include "CSVReader.hpp"
#include <string>
#include <vector>
#include <map>
#include <queue>

//this acts like a database to store all CSV data
class OrderBook
{
    public:
     /** construct, reading a csv data file  */
    OrderBook(std::string filename);
    
    /** return vector of all known products in the database */
    std::vector<std::string> getKnownProducts(std::string _currentTime);
    
    /** return vector of Orders according to the sent filters */
    std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                          std::string product,
                                          std::string timestamp);
    
    /** returns the earliest time in the orderbook*/
    std::string getEarliestTime();
    
    /** returns the next time after the
     * sent time in the orderbook
     * If there is no next timestamp, wraps around to the start
     * */
    std::string getNextTime();
    
    void insertOrder(OrderBookEntry& order);
    
    //matching engine that matches the lowest ask price to the highest bidder
    std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);
    
    //function to withdraw an inserted order from the exchange
    void withdrawOrder(OrderBookEntry& order);
    
    //function to get the mean price of a product in a timestamp based on the orderbooktype
    double getMeanPrice(std::string _timestamp, std::string _product, OrderBookType _orderType);
    
    //highest price
    static double getHighPrice(std::vector<OrderBookEntry>& orders); //passed by reference. this "orders" is what we sent in into the function when we call it. does not need to look at data stored inside the class. Therefore static is used here.
    
    //lowest price
    static double getLowPrice(std::vector<OrderBookEntry>& orders);
    
    private:

    std::map<std::string, std::vector<OrderBookEntry>> orders;
    
    std::queue<std::string> timestamp_queue;
    
    
};
