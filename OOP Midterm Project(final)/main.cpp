#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.hpp"
#include "MerkelMain.hpp"
#include "CSVReader.hpp"
#include "Wallet.hpp"
#include "MerkelBot.hpp"
#include <time.h>

void merkelrex();

int main()
{
    //run the merkelrex functionality for either the bot or manual trading
    merkelrex();
}
    
void merkelrex()
{
    int input;
    
    //prompt user to select either bot or manual trading
    std::cout <<"Please type in 1 for bot automated trading or type in 2 for manual trading:" << std::endl;
    std::cin >> input;
            
    //if option is invalid, proceed to end of programme.
    if(std::cin.fail() || (input != 1 && input != 2))
    {
        std::cout << "Invalid option. Please only type 1 or 2. Unable to proceed - programme terminating now." << std::endl;
    }
    //if 1 is chosen, start bot
    else if(input == 1)
    {
        std::cout << "You have chosen 1. Bot trading simulation begins now. Please wait while data is being processed:" << std::endl;
        
        //start tracking time to measure bot performance
        time_t start = time(0);
        MerkelBot bot{};
        bot.init();
        
        //end time tracking and print performance
        double seconds_taken = difftime( time(0), start);
        std::cout << "Simulation ended. Time taken by the bot to complete simulation: " << seconds_taken << " seconds \n" <<std::endl;
    }
    //if 2 is chosen, allow manual trade
    else if(input == 2)
    {
        std::cout << "You have chosen 2. You may trade manually now: " << std::endl;
        MerkelMain app{};
        app.init();
    }
}
