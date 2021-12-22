#pragma once

#include "OrderBookEntry.hpp"
#include <vector>
#include <string>

class CSVReader
{
    public:
        CSVReader();
    
        static std::vector<OrderBookEntry> readCSV(std::string csvFile);
    
        //below function is moved from private to public because of MerkelMain::enterAsk() wanting to use it
        static std::vector<std::string> tokenise(std::string csvLine, char separator);
    
        //for make an Ask function in merkelrex. Make string that user inputs into OBE. Not that in C++ you're allowed to have two functions with the same name eg: stringsToOBE. The compiler can differentiate based on the arguments passed in
        static OrderBookEntry stringsToOBE(std::string price,
                                           std::string amount,
                                           std::string timestamp,
                                           std::string product,
                                           OrderBookType OrderBookType
                                           );
    private:
   
        static OrderBookEntry stringsToOBE(std::vector<std::string> strings);
    
};
