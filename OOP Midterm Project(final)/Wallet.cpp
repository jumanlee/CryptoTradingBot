
#include "Wallet.hpp"
#include <iostream>
#include "CSVReader.hpp"

Wallet::Wallet()
{
    
}

void Wallet::insertCurrency(std::string type, double amount)
{
    double balance;
    if(amount < 0)
    {
        //crash the program when user tries to abuse it
        throw std::exception{};
    }
    if(currencies.count(type) == 0) //not there yet
    {
        balance = 0;
    }
    else
    {
        //get the numeric value mapped to by the currency type, which is a string product name
        balance = currencies[type];
    }
    
    //add the amount to the balance
    balance += amount;
    currencies[type] = balance;
}

bool Wallet::removeCurrency(std::string type, double amount)
{
    if(amount < 0)
    {   //return false when someone tries to abuse this.
        return false;
    }
    //currency not there yet
    if(currencies.count(type) == 0)
    {
        return false;
    }
    else{
        //is there - do we have enough?
        if(containsCurrency(type, amount)) //if we have enough
        {
            currencies[type] -= amount;
            return true;
        }
        else //they have it but not enough
        {
            return false;
        }
    }
}


/**check if the wallet contains certain amount of currency or more*/
bool Wallet::containsCurrency(std::string type, double amount)
{
    if(currencies.count(type) == 0)// not there yet
        return false;
    else
        return currencies[type] >= amount;
}

std::string Wallet::toString()
{
    std::string s;
    //pair can be used to represent one of the items in the map
    for(std::pair<std::string, double> pair : currencies)
    {
        std::string currency = pair.first;
        double amount = pair.second;
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
}

bool Wallet::canFulfillOrder(OrderBookEntry order) 
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    //ask
    if(order.orderType == OrderBookType::ask)
    {
        double amount = order.amount;
        
        //if you want to sell currs[0], it needs to check if you have enough currs[0]
        std::string currency = currs[0];
        std::cout << "Wallet::canFulfillOrder for " << currency << " : " << amount << std::endl;
        return containsCurrency(currency,amount);
    }
    
    //bid
    if(order.orderType == OrderBookType::bid)
    {
        //the *1.002 is to account for the 0.2% trading fee for bids
        double amount = order.amount * (order.price*1.002);
        //if you want to buy currs[0], it needs to check if you have enough currs[1] to pay for it
        std::string currency = currs[1];
        std::cout << "Wallet::canFulfillOrder for " << currency << " : " << amount << std::endl;
        return containsCurrency(currency,amount);
    }
    return false;
}

double Wallet::checkCurrencies(std::string _currencies)
{
    return currencies[_currencies];
}

void Wallet::processSale(OrderBookEntry& sale)
{
    //note all "sale" obe stores the transaction from your persopective eg: asksale or bidsale
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    //ask
    if(sale.orderType == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        
        //if you want to sell currs[0], it needs to check if you have enough currs[0]
        std::string outgoingCurrency = currs[0];
        
        //note that this sale.price is price per unit! so incoming amount is the actual amount we are receiving
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];
        
        //map function
        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
    }
    
    //bid
    if(sale.orderType == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        
        //if you want to sell currs[0], it needs to check if you have enough currs[0]
        std::string incomingCurrency = currs[0];
        
        //note that this sale.price is price per unit! so incoming amount is the actual amount we are receiving
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];
        
        //map function
        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;
    }
}
