#include "OrderBook.hpp"
#include "CSVReader.hpp"
#include <map>
#include <algorithm>
#include <iostream>
#include <iterator>


/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    std::vector<OrderBookEntry> entireOrders = CSVReader::readCSV(filename); //orders is the vector of all obe's.
    
    //copies all entireOrders into map that contains the obe's according to their timestamps.
    for(OrderBookEntry e : entireOrders)
    {
        orders[e.timestamp].push_back(e);
    }
    
    //push in the unique timestamps into a queue, which allows for much faster processing.
    for(std::pair<std::string, std::vector<OrderBookEntry>> pair : orders)
    {
        timestamp_queue.push(pair.first);
    }
    
    //clear entireOrders container as no longer needed.
    entireOrders.clear();
}

/** return vector of all known products in the database */
std::vector<std::string> OrderBook::getKnownProducts(std::string _currentTime)
{
    std::vector<std::string> products;
    
    std::map<std::string, bool> prodMap;
    
    for(OrderBookEntry& e : orders[_currentTime])
    {
        //this stores UNIQUE entries
        //e.product is the key of the map as opposed to value. Note that .product is a property of OBE, and that's the reason why you only get the product names and not other properties of the OBE's.
        prodMap[e.product] = true;
        
    }
    
    //now flatten the map to a vector of strings
    for(auto const& e : prodMap)
    {
        //this is to prepare for the print out of all known products in Merkle Main. Note that .first gets you the first elemt of the pair in mmap.
        products.push_back(e.first);
    }
    
    return products;
}

/** return vector of Orders according to the sent filters */
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                      std::string product,
                                      std::string timestamp)
{
    std::vector<OrderBookEntry> orders_container;
    for(OrderBookEntry& e : orders[timestamp])
    {
        if(e.orderType == type &&
           e.product == product &&
           e.timestamp == timestamp)
        {
            //When we iterate over a vector, as in the getOrders function, even though the OrderBookEntry is set to const reference, when we push it the vector, a copy of the object is made.
            orders_container.push_back(e);
        }
    }
    return orders_container;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for(OrderBookEntry& e : orders)
    {
        if(e.price> max)max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for(OrderBookEntry& e : orders)
    {
        if(e.price< min)min = e.price;
    }
    return min;
}


std::string OrderBook::getEarliestTime()
{
    //get the earliest unique key of the orders map, which is the earliest timestamp
    return orders.begin()->first;
}

std::string OrderBook::getNextTime()
{
    //make string variable to store next timestamp
    std::string next_timestamp;
    
    //make string variable to store the first element of the queue
    std::string container;
    
    //get the first element of the queue and store it in container
    container = timestamp_queue.front();
    
    //remove the last element of the queue
    timestamp_queue.pop();
    
    //get the first element of the queue stored in container and push it into the back of the queue.
    timestamp_queue.push(container);
    
    //Now, get the first element of the queue. This is element is now the next timestamp
    next_timestamp = timestamp_queue.front();
    
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    //push the order into the vector mapped to by the order's timestamp
    orders[order.timestamp].push_back(order);
    
    //orders.end() is the past-the-end element because imagine in a vector, it starts from the first stroke of the box in the first erlement box, but you don't end the vector in the first stroke of the last box, but you end it in the last stroke of the last box.
    std::sort(orders[order.timestamp].begin(), orders[order.timestamp].end(), OrderBookEntry::compareByTimestamp);
}

void OrderBook::withdrawOrder(OrderBookEntry& order)
{
    //find the order in the vector (which is in a map) and remove the order.
    for(int i = 0; i < orders[order.timestamp].size(); i++)
    {
        //go through the corresponding map element to find the order with the exact same criteria as specified by the order OBE that has been passed in as an argument
        if(orders[order.timestamp][i].product == order.product &&
           orders[order.timestamp][i].amount == order.amount &&
           orders[order.timestamp][i].timestamp == order.timestamp &&
           orders[order.timestamp][i].username == order.username &&
           orders[order.timestamp][i].orderType == order.orderType)
        {
            //delete the chosen element from the orders map vector using .erase()
            orders[order.timestamp].erase(orders[order.timestamp].begin() + i);

            std::cout << "have withdrawn " << order.product << " from " << order.timestamp << std::endl;
            
            break;
        }
    }
}

double OrderBook::getMeanPrice(std::string _timestamp,
                               std::string _product,
                               OrderBookType _orderType)
{
    //filter for all orders based on the product name, timesamp and orderbooktype
    std::vector<OrderBookEntry> entries = getOrders(_orderType,
                                                    _product,
                                                    _timestamp);
    //container to store the sum of all prices
    double totalPrice = 0;
    
    //get all prices from the entries and sum them all up
    for(OrderBookEntry& e : entries)
    {
        totalPrice += e.price;
    }
    
    //calculate the mean price
    double meanPrice = (double) totalPrice/entries.size();

    return meanPrice;
}

//this matches the highest bid price to the lowest ask price. This returns a vector of obe's matched.
std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    //make a vector sales to store all successful OBE trades.
    std::vector<OrderBookEntry> sales;
    
    //Filter for all asks acording to the following filter criteria
    std::vector <OrderBookEntry> asks = getOrders(OrderBookType::ask,
                                                  product,
                                                  timestamp);
    
    //filter for all bids according to the following filter criteria
    std::vector <OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);
    
    //this is to prevent the case where the product does not exist in the current time stamp for either asks or bids. 
    if(asks.empty() || bids.empty())
    {
        throw std::exception{};
    }
    else{
    
    //sort asks from lowest to highest
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    
    //sort bids from highest to lowest
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    
    //for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    //for ask in asks
    for(OrderBookEntry& ask : asks)
    {
        //for bid in bids:
        for(OrderBookEntry& bid : bids)
        {
            //if bid.price >= ask.price # we have a match
            if(bid.price >= ask.price)
            {
                
               //set to default value, which is asksale
                OrderBookEntry sale{ask.price, 0, timestamp, product, OrderBookType::asksale};

                //Look for sale that has "simuser" as username as this belongs to the bot/user
                if(bid.username == "simuser")
                {
                    //assigning to the new obe named sale. This is determining whether the user is an asker or bidder
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                    
                }
                
                //Look for sale that has "simuser" as username as this belongs to the bot/user
                if(ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }
                
                if(bid.amount == ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = 0;
                    break;
                }
                
                if(bid.amount > ask.amount)
                {
                    sale.amount = ask.amount;
                    sales.push_back(sale);
                    bid.amount = bid.amount - ask.amount;
                    break;
                }
                //note when bid.amount < ask.amount, because this involves "continue" and not "break" as in conditions above, even when bid.amount is 0, it will keep processing and push_back sale to sales. So you need to deal with this, otherwise it would keep printing out the 0 bid.amount transactions. Hence bid.amount > 0.
                if(bid.amount < ask.amount && bid.amount > 0)
                {
                    sale.amount = bid.amount;
                    sales.push_back(sale);
                    ask.amount = ask.amount - bid.amount;
                    bid.amount = 0;
                    continue;
                }
            }
        }
    }
    
    return sales;
        
    }
}
