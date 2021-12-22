#pragma once

#include <vector>
#include "OrderBookEntry.hpp"
#include "OrderBook.hpp"
#include "Wallet.hpp"

class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        //void loadOrderBook();
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        void processUserOption(int userOption);
    
    std::string currentTime;

    //this is to load the csv file
    OrderBook orderBook{"20200317.csv"};
    
    Wallet wallet;
};
