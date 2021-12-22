#pragma once

#include <string>
#include <map>
#include "OrderBookEntry.hpp"


class Wallet
{
    public:
        Wallet();

        /** insert currency to the wallet*/
        void insertCurrency(std::string type, double amount);
    
        /** remove currency from the wallet*/
        bool removeCurrency(std::string type, double amount);

        /**check if the wallet contains certain amount of currency or more*/
        bool containsCurrency(std::string type, double amount);
    
        /** checks if the wallet can cope with this ask or bid*/
        bool canFulfillOrder(OrderBookEntry order);
    
        /** update the contents of the wallet. Assumes the order was made by the owner of the wallet*/
        void processSale(OrderBookEntry& sale);
        
        /**generate a string representation o the wallet*/ //print out all currencies in the wallert and how much you have
        std::string toString();
    
        //tells you how much you have in that currency
        double  checkCurrencies(std::string _currencies);

    private:
    //mapping from string to double
    std::map<std::string,double> currencies;
};
