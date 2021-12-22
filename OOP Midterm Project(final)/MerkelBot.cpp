
#include "MerkelBot.hpp"
#include <iostream>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <stdlib.h>
#include "OrderBookEntry.hpp"
#include "OrderBook.hpp"
#include "CSVReader.hpp"

MerkelBot::MerkelBot()
{

}


void MerkelBot::init()
{
    //get the earliest time in the orderBook and pass it to the currentTime variable
    currentTime = orderBook.getEarliestTime();
    
    //insert initial currencies into the wallet
    wallet.insertCurrency("BTC", 10);
    wallet.insertCurrency("USDT", 1000);
    
    //publish to console the total initial balance
    std::cout << "Inital balance: \n" <<wallet.toString()<< std::endl;

    //log the initial balance of the wallet. This balance will be included in the .txt file of the assets record. 
    log.logAssets(currentTime, wallet.toString());
    
    std::cout << "Current timestamp being processed: " <<currentTime<< "\n"<< std::endl;
    
    //get all the unique known products from the current timestamp.
    for(std::string const& p : orderBook.getKnownProducts(currentTime))
    {
        //filter for a vector of obe's of one product type
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                  p,
                                  currentTime);
        
        //if the entries are not empty, then proceed to add the initial minimum ask prices to the short moving average and the long moving average calculations.
        if(!entries.empty())
        {
            //add the product moving average object to allProdMovingAverages vector
            allProdMovingAverages.push_back(MovingAverage(entries, shortPeriod, longPeriod));
            std::cout << "Product: " << allProdMovingAverages[allProdMovingAverages.size() - 1].product << std::endl;
            std::cout << "Short Moving Average: " << allProdMovingAverages[allProdMovingAverages.size() - 1].shortMovingAverage() << std::endl;
            std::cout << "Long Moving Average: " << allProdMovingAverages[allProdMovingAverages.size() - 1].longMovingAverage() << "\n" << std::endl;
        }
    }
    
    //automate and run the trading functions repeatedly until the end of the data timestamp is reached.
    automate();
}

void MerkelBot::automate()
{
    //record the earliest timestamp
    std::string earliestTime = currentTime;
    
    //variable to keep track of how many timestamps have passed.
    unsigned long int timestampCount = 0;
    
    do
    {
        //only allow trading when timestampCount is multiple of 3. This is to prevent consecutive bids or asks from being inserted in every timestamp. Also, only after timestampcount has passed 50 can trading be allowed. This is to ensure only trading is allowed only after the longMovingAverage (50 timestamps) is available.
        if(timestampCount % 3 == 0 && timestampCount > 50)
        {
            //print cout to let user know this particular timestamp is being processed.
            std::cout << "Processing trading for " <<currentTime<< "\n" <<std::endl;
            
            //run alogrithm to decide if bid should be made in this timestamp.
            decideBid();
            
            //run algorithm to decide if ask should be made in this timeastamp.
            decideAsk();
            
            //run algorithm to decide if any ask or bid should be withdrawn in this timeastamp.
            decideWithdraw();
            
            //then proceed to process sale
            processSales();
        }
        else
        {
            //let user know when this timestamp has been skipped for trading.
            std::cout << "Skipped trading for " <<currentTime<< "\n" <<std::endl;
        }

        //proceed to the next timeframe
        gotoNextTimeframe();
        
        //add to the time count.
        timestampCount++;
    }
    while(earliestTime != currentTime); //stop when earliestTime is equal to currentTime.
    
    //publish all assets, BidsOffers and SuccessTrade .txt files when simulation ends.
    log.publish();

    std::cout << "Final balance: \n" <<wallet.toString()<< std::endl;
    
    std::cout << "Trading results have been published to the following 3 files: assets.txt, bidsoffers.txt, successTrades.txt  \n" <<std::endl;
}

void MerkelBot::updateAllProdPrices()
{
    //get all the known unique products in this timestamp
    for(std::string const& p : orderBook.getKnownProducts(currentTime))
    {
        //filter for OrderBookEntry data based on the following criteria:
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
                                  p,
                                  currentTime);
        
        //making sure entries is not empty
        if(!entries.empty())
        {
            //this "count" is for keeping track of whether the product is in the existing allProdMovingAverages array. If it isn't, then push_back the new product
            unsigned long int count = 0;
            
            //update the moving average in the appropriate element in allProdMovingAverages
            for(MovingAverage& m : allProdMovingAverages)
            {
                if(entries.at(0).product == m.product)
                {
                    m.updatePrices(entries);
                    
                    std::cout << "Product: " << m.product << std::endl;
                    std::cout << "Short Moving Average: " << m.shortMovingAverage() << std::endl;
                    std::cout << "Long Moving Average: " << m.longMovingAverage() << "\n" << std::endl;
                  
                    break;
                }
                
                //if not found in the allProdMovingAverages array, push in the new entries. Note that, due to the "break" in the above if statement content, this condition below will only ever need to be implemented if the item is not found in the existing array.
                if(count == allProdMovingAverages.size() - 1)
                {
                    allProdMovingAverages.push_back(MovingAverage(entries, shortPeriod, longPeriod));
                    
                    std::cout << "pushed "<<entries[0].product<< "into allProdMovingAverages as a new product that didn't exist before"<< std::endl;
                }
                
                count++;
            }
        }
    }
}

void MerkelBot::decideBid()
{
    //traverse all products' moving averages in allProdMovingAverages to decide whether to place a bid.
    for(MovingAverage& m : allProdMovingAverages)
    {
        //calculate shortLongExcess to see if shortMovingAverage is greater than longMovingAverage
        double shortLongExcess = (double) m.shortMovingAverage() - m.longMovingAverage();
        
        //if so, then make a bid.
        if(shortLongExcess > 0)
        {
            std::string product = m.product;
            std::vector<std::string> tokenised_product = CSVReader::tokenise(product, '/');
            
            //get the latest minimum price for bidding and target to pay a 20% premium over the minimum price to increase chance of winning the bid.
            double targetPrice = m.shortMinPrices[m.shortMinPrices.size() - 1]*1.2;
            
            //aiming to buy all the amount with all of the exchange currency that the bot owns. eg: if the bot wants to buy the product ETH/BTC, the amount of ETH it targets to buy will be: 10% of all the BTCs that it owns divided by the amount of BTC it needs to pay to buy 1 ETH. Note: wallet.checCurrencies allows the bot to know how much of the tokenised_product it currently owns.
            double targetAmount = (wallet.checkCurrencies(tokenised_product[1])/targetPrice)*0.1;
            
            
            //make this into an OBE
            OrderBookEntry obe(targetPrice, targetAmount, currentTime, product, OrderBookType::bid, "simuser");
            
            //check if wallet can fulfill this bid
            if(wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good for " << product <<std::endl;
                orderBook.insertOrder(obe);
                std::cout << "inserted order \n" << std::endl;
                
                //record this in the MerkelBot object memory
                insertedBid.push_back(obe);
                
                //log this obe
                log.logBidsOffers(obe);
                
                //this is to ensure only one bid is made in one time stamp.
                break;
            }
            else
            {
                //let user know that the wallet can't fulfill it
                std::cout << "Wallet has insufficient funds for " << product <<std::endl;
            }
        }
    }
}

void MerkelBot::decideAsk()
{
    //traverse all products' moving averages in allProdMovingAverages to decide whether to place an ask.
    for(MovingAverage& m : allProdMovingAverages)
    {
        //calculate shortLongExcess to see if shortMovingAverage is less than longMovingAverage
        double shortLongExcess = (double) m.shortMovingAverage() - m.longMovingAverage();
        
        //if so, then place an ask.
        if(shortLongExcess < 0)
        {
            std::string product = m.product;
            std::vector<std::string> tokenised_product = CSVReader::tokenise(product, '/');
            
            //get the latest minimum ask price and target to sell for 5% more than the lowest asking price in the market for that timestamp.
            double targetPrice = m.shortMinPrices[m.shortMinPrices.size() - 1]*1.05;
            
            //aiming to sell 10% of the currency in question that the bot currently owns. Note: wallet.checkCurrencies tells the bot how much of the currency in question it currently owns.
            double targetAmount = wallet.checkCurrencies(tokenised_product[0])*0.1;
            
            //make this into an OBE
            OrderBookEntry obe(targetPrice, targetAmount, currentTime, product, OrderBookType::ask, "simuser");
            
            //check if wallet can fulfill this
            if(wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good for " << product <<std::endl;
                orderBook.insertOrder(obe);
                std::cout << "inserted order" << std::endl;
                
                //record this in the MerkelBot object memory
                insertedAsk.push_back(obe);
                
                //log this obe
                log.logBidsOffers(obe);
                
                //this is to ensure only one bid is made in one time stamp.
                break;
            }
            else
            {
                std::cout << "Wallet has insufficient funds for " << product <<std::endl;
            }
        }
    }
}

void MerkelBot::decideWithdraw()
{
    //check if there are only at most one ask and at most one bid being placed.
    if(insertedBid.size() == 1 && insertedAsk.size() == 1)
    {
        std::vector<std::string> bidCur = CSVReader::tokenise(insertedBid.at(0).product, '/');
        std::vector<std::string> askCur = CSVReader::tokenise(insertedAsk.at(0).product, '/');
        
        //checks whether the bot is trying to buy the Bid product with a currency that it is also trying to sell at the same time. If so, then withdraws either Bid or Ask randomly. More explanation about this in section R2D of the report.
        if(bidCur[1] == askCur[0])
        {
            //generate a random number and Mod 2
            int rand = std::rand() % 2;
            if(rand == 0)
            {
                //withdraw the Ask order if rand if 0
                orderBook.withdrawOrder(insertedAsk.at(0));
            }
            else
            {
                //withdraw the Bid order if rand is 1
                orderBook.withdrawOrder(insertedBid.at(0));
            }
        }
        else
        {
            std::cout << "Nothing to withdraw \n" <<std::endl;
        }
    }
    else if(insertedBid.size() > 1 || insertedAsk.size() > 1)
    {
        std::cout<<"Error: insertedBid or insertedAsk should only contain at most 1 element as at most one bid and one ask can be inserted. Please investigate the root cause of the error \n" << std::endl;
        
        //crash the programme as the simulation cannot be allowed to continue due to the serious error
        throw std::exception{};
    }
    else
    {
        std::cout <<"no inserted order to be withdrawn. Proceed to processing order" << std::endl;
    }
}

void MerkelBot::processSales()
{
    //get all known products in this timestamp for matching
    for (std::string& p : orderBook.getKnownProducts(currentTime))
    {
        std::cout << "matching " << p << std::endl;
            
        //if the product doesn't exist in the current time stamp, orderBook.matchAsksToBids will throw an exception.
        try{
            //sales here is a vector of matched obe's
            std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);

            std::cout << "Sales: " << sales.size() << "\n" << std::endl;
            for (OrderBookEntry& sale : sales)
            {
                std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
                
                //added wallet.canfulfill to double check it really has the currency for the transaction.
                if(sale.username == "simuser")
                {
                    //trading fee of 0.5% to be deducted from sale. If this is a bidsale, sale.price will 0.5% higher to account for the extra fee payment. If this is an asksale, sale.price will be 0.5% lower as some of the proceeds will be deducted fore the fee payment.
                    if(sale.orderType == OrderBookType::asksale)
                    {
                        sale.price = (double) sale.price*0.998;
                    }
                    else if(sale.orderType == OrderBookType::bidsale)
                    {
                        sale.price = (double) sale.price*1.002;
                    }
                    

                    //update the wallet accordingly
                    wallet.processSale(sale);
     
                    
                   //logging the successful sale. First argument is the sale obe, the second argument is the mean price of all the Bids and the third argument is the mean price of all the Asks in the current timestamp.
                    log.logSales(sale,
                                 orderBook.getMeanPrice(currentTime, sale.product, OrderBookType::bid),
                                 orderBook.getMeanPrice(currentTime, sale.product, OrderBookType::ask)
                                 );
                }
            }
        }catch(const std::exception& e)
        {
            std::cout << p << " is not in " <<currentTime << std::endl;
        }
     }
    
    //remove all contents of recorded inserted bids and asks
    insertedAsk.clear();
    insertedBid.clear();
    
    //log balance of currencies
    log.logAssets(currentTime, wallet.toString());
}

void MerkelBot::gotoNextTimeframe()
{
    //update current time to next time frame
    currentTime = orderBook.getNextTime();
    
    std::cout << "Current timestamp is: " <<currentTime<< "\n" << std::endl;
    
    //then update all product prices corresponding to the next timeframe
    MerkelBot::updateAllProdPrices();
}


