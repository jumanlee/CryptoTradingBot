#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.hpp"
#include "OrderBook.hpp"
#include <map>



class MovingAverage
{
    public:
        MovingAverage(std::vector<OrderBookEntry> _initial_obeVector,
                      int _shortTime,
                      int _longTime);
        
        //vector that contains all obe's for one specific product for the current timestamp
        std::vector<OrderBookEntry> initial_obeVector;
        
        //update the object with the new prices from the obe Vector that is passed in.
        void updatePrices(std::vector<OrderBookEntry> _obeVector);
        
        //function to calculate short moving average
        double shortMovingAverage();
        
        //function to calculate short moving average
        double longMovingAverage();
    
        //name of product
        std::string product;
        
        //map for short moving average
        std::vector<double> shortMinPrices;
        
        //map for short moving average
        std::vector<double> longMinPrices;
        
    private:
    
        //short time period for calculating short moving average
        int shortTime;
        
        //long time period for calculating long moving average
        int longTime;

};
