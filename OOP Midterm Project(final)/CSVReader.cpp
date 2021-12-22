#include "CSVReader.hpp"
#include <iostream>
#include <fstream>

CSVReader::CSVReader()
{
    
}

std::vector<OrderBookEntry> CSVReader::readCSV(std::string csvFilename)
{
    std::vector<OrderBookEntry> entries; //this is a vector of all OBE's.
    
    std::ifstream csvFile{csvFilename};
    std::string line;
    
    if(csvFile.is_open())
    {
        while(std::getline(csvFile, line)) //gets a line in the csvFile and put it into the line variable
        {
            //this is to catch the bad line throw std::exception{};
            try{
                OrderBookEntry obe = stringsToOBE(tokenise(line, ','));
                entries.push_back(obe);
            }catch(const std::exception& e)
            {
                std::cout <<"CSVReader::readCSV bad data" << std::endl;
            }
        } //end of while
    }
    
    std::cout << "CSVReader::readCSV read " << entries.size() << "entries" << std::endl;
    
    return entries;  //this is a vector of all OBE's.
}

std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator)
{
    std::vector<std::string> tokens;
    
    //int start, end used to delineat the position of tokens
    signed int start, end;
    std::string token;
    
    //start is pointing at t. 0 is cuz it's starting from position 0.
    start = csvLine.find_first_not_of(separator, 0);
    
    do{
        //end is pointing at separator
        end = csvLine.find_first_of(separator, start);
        
        if(start == csvLine.length() || start == end)
        {
            break;
        }
        
        if (end >= 0)
        {
            //this is substring, not substract!
            token = csvLine.substr(start, end - start);
        }
        else
        {
            token = csvLine.substr(start, csvLine.length() - start);
        }
        
        tokens.push_back(token);
        
        start = end + 1;
    }
    while(end > 0);
    
    return tokens;
}

OrderBookEntry CSVReader::stringsToOBE(std::vector<std::string> tokens) //this is another constructor function of another type
{
    //each tokens vector represents each line in the csv
    double price, amount;
    if(tokens.size() != 5) //bad data
    {
        std::cout << "Bad line" << std::endl;
        throw std::exception{};
    }
    
    //we have 5 tokens, so proceed to the following:
    
    try{
        price = std::stod(tokens[3]);
        amount = std::stod(tokens[4]);
        //std::cout << price << ":" << amount << std::endl;
        
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[3]<< std::endl;
        std::cout << " CSVReader::stringsToOBE Bad float! " << tokens[4]<< std::endl;
        throw;
    }
    
    
    OrderBookEntry obe{price,
                       amount,
                       tokens[0],
                       tokens[1],
                       OrderBookEntry::stringToOrderBookType(tokens[2])};  //remember, static function/method doesn't ned to first create an instance of the class.
    
    return obe;
}

OrderBookEntry CSVReader::stringsToOBE(std::string priceString,
                                   std::string amountString,
                                   std::string timestamp,
                                   std::string product,
                                   OrderBookType orderType
                                   )
{
    double price, amount;
    
    try{
        price = std::stod(priceString);
        amount = std::stod(amountString);
        std::cout << price << ":" << amount << std::endl;
        
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << priceString<< std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << amountString<< std::endl;
        throw;
    }
    
    //make those into an obe:
    
    OrderBookEntry obe{price,
                       amount,
                       timestamp,
                       product,
                       orderType};
    
    return obe;

}
