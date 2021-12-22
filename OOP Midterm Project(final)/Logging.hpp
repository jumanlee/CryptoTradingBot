#pragma once

#include "Wallet.hpp"
#include "OrderBookEntry.hpp"
#include <vector>
#include <string>
#include <map>
#include <tuple>


class Logging
{
    public:
    Logging();
    
    //function to log balance of assets for every timestamp cycle
    void logAssets(const std::string& _timestamp, std::string _balance);
    
    //function to publish all three of the .txt files
    void publish();
    
    //function to log trade every time a Bid or an Ask is inserted into the exchange
    void logBidsOffers(const OrderBookEntry& obe);
    
    //function to log successful bidsales or asksales
    void logSales(const OrderBookEntry& obe, double meanBidPrice, double meanAskPrice);
    
    private:
    //map variable to store asset information for every timestamp
    std::map<std::string,std::string> assets;
    
    //vector to store bids the bot has made
    std::vector<OrderBookEntry> bids;
    
    //vector to store asks the bot has made
    std::vector<OrderBookEntry> asks;
    
    //vector to store successful bidsales the bot has made. Each element of the vector is a tuple, which stores the OBE and the product's corresponding mean Ask price and mean Bid price
    std::vector<std::tuple<OrderBookEntry, double, double>> bidSales;
    
    //vector to store successful asksales the bot has made. Each element of the vector is a tuple, which stores the OBE and the product's corresponding mean Ask price and mean Bid price
    std::vector<std::tuple<OrderBookEntry, double, double>> askSales;
};
