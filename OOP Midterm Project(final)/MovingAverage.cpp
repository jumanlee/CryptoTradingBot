
#include "MovingAverage.hpp"


//instances of this class are used to store all the minimum ask prices for each specific product, and these prices will be used to compute the moving average price by deviding the number of timeperiods specified. One instance represents one product type and stores all its relevant moving average data. 
MovingAverage::MovingAverage(std::vector<OrderBookEntry> _initial_obeVector, int _shortTime, int _longTime)

: initial_obeVector(_initial_obeVector),

    //period of the short moving average
  shortTime(_shortTime),

    //period of the long moving average
  longTime(_longTime)
  
{
    //name of product
    product = initial_obeVector[0].product;

    //initialise with the first state of obe's of the obeArray
    updatePrices(initial_obeVector);
}

//get total of all prices
void MovingAverage::updatePrices(std::vector<OrderBookEntry> obeVector)
{
    //container for product prices of the obe vector passed in.
    std::vector<double> prodPrices;
    
    //container for storing the minimum price in the prodPrices vector
    double minPrice;
    
    //retrieve all prices of this new OBE vector
    for(OrderBookEntry& e : obeVector)
    {
        prodPrices.push_back(e.price);
    }
    
    //get the minimum price of all the obe's for this particular product from the obe vector passed in.
    minPrice = *min_element(prodPrices.begin(), prodPrices.end());
    
    //begin to update shortMinPrices
    if(shortMinPrices.size() < shortTime)
    {
        //just push the elements into the vector if the vector size is less than the maximum allowed
        shortMinPrices.push_back(minPrice);
    }
    else if(shortMinPrices.size() == shortTime)
    {
        //the vector size now equals the maximum allowed based on shortTime. Now, erase the first element of the shortMinPrices vector, which will reduce its size by one
        shortMinPrices.erase(shortMinPrices.begin());
        
        //then push the latest minimum price retrieved into the shortMinPrices as the last element of the vector
        shortMinPrices.push_back(minPrice);
    }
    else
    {
        //stop the programme when the update fails, which means something somewhere has gone seriously wrong and the simulation cannot continue.
        std::cout << "Error: unable to update shortMinPrices vector for " <<product<<". please investigate!" << std::endl;
        throw std::exception{};
    }
    
    //begin to update longMinPrices.
    if(longMinPrices.size() < longTime)
    {
        //just push the elements into the vector if the vector size is less than the maximum allowed
        longMinPrices.push_back(minPrice);
    }
    else if(longMinPrices.size() == longTime)
    {
        //the vector size now equals the maximum allowed based on longTime. Now, erase the first element of the longMinPrices vector, which will reduce its size by one
        longMinPrices.erase(longMinPrices.begin());
        
        //then push the latest minimum price retrieved into the longMinPrices as the last element of the vector
        longMinPrices.push_back(minPrice);
    }
    else
    {
        //stop the programme when the update fails, which means something somewhere has gone wrong and the simulation cannot continue.
        std::cout << "Error: unable to update longMinPrices vector for " <<product<<". please investigate!" << std::endl;
        throw std::exception{};
    }
}

//calculate short moving average
double MovingAverage::shortMovingAverage()
{
    double totalPrices = 0;
    unsigned short int vector_size = shortMinPrices.size();
    
    if(vector_size <= shortTime)
    {
        for(double& d : shortMinPrices)
        {
            totalPrices += d;
        }
        
        return (double) totalPrices/vector_size;
    }
    else
    {
        //crash the programme if there's an error in the moving average as the simulation must not continue in this situation.
        std::cout << "Error: shortMinPrices size is more than shortTime" << std::endl;
        
        throw std::exception{};
    }
}

//calculate long moving average
double MovingAverage::longMovingAverage()
{
    double totalPrices = 0;
    unsigned short int vector_size = longMinPrices.size();
    
    if(vector_size <= longTime)
    {
        for(double& d : longMinPrices)
        {
            totalPrices += d;
        }
        
        return (double) totalPrices/vector_size;
    }
    else
    {
        //crash the programme if there's an error in the moving average as the simulation must not continue in this situation.
        std::cout << "Error: longMinPrices size is more than longTime" << std::endl;
        
        throw std::exception{};
    }
    
}




