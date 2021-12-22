#include "Logging.hpp"
#include "Wallet.hpp"
#include <iostream>
#include <fstream>
#include <string>

Logging::Logging()
{
    
}

void Logging::logAssets(const std::string& _timestamp, std::string _balance)
{
    //map each timestamp to the currency balance in that timestamp
    assets[_timestamp] = _balance;
}

void Logging::publish()
{
    //make assetsFile variable to manipulate assets.txt
    std::ofstream assetsFile;
    
    //open assets.txt
    assetsFile.open("assets.txt");
    
    //write all content in the assets vector into the assets.txt file
    for(std::pair<std::string, std::string> pair : assets)
    {
        assetsFile << pair.first << " : \n" <<pair.second<< std::endl;
    }
    
    //close file
    assetsFile.close();
    
    //make variable BidsOffers to manipulate the bidsoffers.txt file
    std::ofstream bidsOffersFile;
    
    //open file
    bidsOffersFile.open("bidsoffers.txt");
    
    //first write all bids in the bids vector into the file
    for(const OrderBookEntry& e : bids)
    {
        bidsOffersFile << "Bid made: " << e.timestamp<< ", " <<e.product<< ", Price: " <<e.price<< ", Amount: " <<e.amount<< std::endl;
    }
    
    bidsOffersFile << "\n" << std::endl;
    
    //then write all asks in the asks vector into the file
    for(const OrderBookEntry& e : asks)
    {
        bidsOffersFile << "Offer made: " << e.timestamp<< ", " <<e.product<< ", Price: " <<e.price<< ", Amount: " <<e.amount<<  std::endl;
    }
    
    //close file
    bidsOffersFile.close();
    
    //make variable to manipulate successTrades.txt
    std::ofstream successFile;
    
    successFile.open("successTrades.txt");
    
    //First write all successful bidsales stored in the bidSales vector into the file
    for(const std::tuple<OrderBookEntry, double, double>& e : bidSales)
    {
        
        successFile << "Successful bidsale: " << std::get<0>(e).timestamp<< ", " <<std::get<0>(e).product<< ", Price Paid: " <<std::get<0>(e).price<< ", Amount: " <<std::get<0>(e).amount<< "\n" << "Mean Bid Price: " << std::get<1>(e) << ", Mean Ask Price: " << std::get<2>(e) << std::endl;
        
        successFile << "\n" << std::endl;
    }
    
    //First write all successful asksales stored in the askSales vector into the file
    for(const std::tuple<OrderBookEntry, double, double>& e : askSales)
    {
        successFile << "Successful asksale: " << std::get<0>(e).timestamp<< ", " <<std::get<0>(e).product<< ", Price Paid: " <<std::get<0>(e).price<< ", Amount: " <<std::get<0>(e).amount<< "\n" << "Mean Bid Price: " << std::get<1>(e) << ", Mean Ask Price: " << std::get<2>(e) << std::endl;
        
        successFile << "\n" << std::endl;
    }
    
    successFile.close();
    
}

void Logging::logBidsOffers(const OrderBookEntry& obe)
{
    //record all Bids into the bids vector and record all Asks into the asks vector
    if(obe.orderType == OrderBookType::bid)
    {
        bids.push_back(obe);
    }
    else if(obe.orderType == OrderBookType::ask)
    {
        asks.push_back(obe);
    }
}

void Logging::logSales(const OrderBookEntry& obe, double meanBidPrice, double meanAskPrice)
{
    //Bind each successful bidsale with its product's corresponding mean bid price and mean ask price into a tuple and push it into the bidSales vector
    if(obe.orderType == OrderBookType::bidsale)
    {
        bidSales.push_back(std::make_tuple(obe, meanBidPrice, meanAskPrice));
    }
    //Bind each successful asksale with its product's corresponding mean bid priceand mean ask price into a tuple and push it into the bidSales vector
    else if(obe.orderType == OrderBookType::asksale)
    {
        askSales.push_back(std::make_tuple(obe, meanBidPrice, meanAskPrice));
    }
}



