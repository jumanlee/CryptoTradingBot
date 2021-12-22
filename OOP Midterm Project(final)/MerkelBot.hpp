#pragma once

#include "OrderBookEntry.hpp"
#include "OrderBook.hpp"
#include "MovingAverage.hpp"
#include "Logging.hpp"
#include "Wallet.hpp"
#include <string>
#include <vector>


class MerkelBot
{
    public:
    
        //array for storing moving averages of all product types seen by the bot.
        std::vector<MovingAverage> allProdMovingAverages;
        
        MerkelBot();
    
        //function to initialise and run all the other functions.
        void init();
    
    private:
        //this function repeats the decideBid, decideAsk, decideWithdraw process for every timestamp
        void automate();
        
        //generate simple moving average prices for each product, taking into account the latest timeframe and 4 previous ones.
        void updateAllProdPrices();
        
        //decide whether to make an Bid. If decided to, a Bid will be made
        void decideBid();
        
        //decide whether to make an Ask. If decided to, an Ask will be made
        void decideAsk();
        
        //decide whether to withdraw order. If decided to, the specified order will be withdrawn.
        void decideWithdraw();
        
        //process sale before going to next time frame
        void processSales();

        //go to the next time frame
        void gotoNextTimeframe();
        
        //string variable to store and keep track of the current timestamp
        std::string currentTime;
        
        //short period for calculating the short time moving average. This is the moving average for past 10 timestamps seen by the bot.
        int shortPeriod = 10;
        
        //long period for calculating the long time moving average. This is the moving average for past 50 timestamps seen by the bot.
        int longPeriod = 50;
        
        //variable for recording bid inserted by bot.
        std::vector<OrderBookEntry> insertedBid;
        
        //variable for recording ask inserted by bot.
        std::vector<OrderBookEntry> insertedAsk;
        
        //make an OrderBook object and load the csv file onto the orderBook.
        OrderBook orderBook{"20200601.csv"};
        
        //make a Logging object for recording bids, asks, sales, asset changes and successful trades. These will be published into three .txt files.
        Logging log;
        
        //create wallet object
        Wallet wallet;
};
