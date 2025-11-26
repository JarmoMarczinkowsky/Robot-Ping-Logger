#include <iostream>
#include "icmplib.h"
#include <fstream>
#include <iomanip>

int pingRepeat(std::string address, std::string &resolved);
std::string getNowTime();

// Function to perform a single ping and log the result
int pingRepeat(std::string address, std::string &resolved, std::ofstream &fileOut)
{
    int ret = EXIT_SUCCESS;

    std::cout << "Pinging " << (resolved.empty() ? address : resolved + " [" + address + "]")
              << " with " << ICMPLIB_PING_DATA_SIZE << " bytes of data:" << std::endl;
    
    // Capture the time when the ping is sent
    auto time_sent = std::chrono::system_clock::now();

    // Perform the ping
    auto result = icmplib::Ping(address, ICMPLIB_TIMEOUT_1S);
    auto time_received = std::chrono::system_clock::now();
    
    switch (result.response) {
        case icmplib::PingResponseType::Failure:
        std::cout << "Network error." << std::endl;
        ret = EXIT_FAILURE;
        break;
        
        case icmplib::PingResponseType::Timeout:
        std::cout << "Request timed out." << std::endl;
        break;
        
        default:
        std::cout << "Reply from " << static_cast<std::string>(result.address) << ": ";
        
        switch (result.response) 
        {
            
            case icmplib::PingResponseType::Success:
                std::cout << "time=" << result.delay;
                if (result.address.GetType() != icmplib::IPAddress::Type::IPv6) {
                    std::cout << " TTL=" << static_cast<unsigned>(result.ttl);
                }
                
                // Log success of ping with details of ping, sent time and received time in epoch format
                fileOut << address << "," << "Success" << "," << result.delay << "," << static_cast<unsigned>(result.code) << "," << static_cast<unsigned>(result.ttl) << "," << time_sent.time_since_epoch().count() << "," << time_received.time_since_epoch().count() << "\n";
                break;
            
            case icmplib::PingResponseType::Unreachable:
                std::cout << "Destination unreachable.";
                fileOut << address << "," << "Unreachable" <<  ",,,\n";
                break;
            
            case icmplib::PingResponseType::TimeExceeded:
                std::cout << "Time exceeded.";
                fileOut << address << "," << "TimeExceeded" << ",,,\n";
                break;
            
            default:
                std::cout << "Response not supported.";
                fileOut << address << "," << "Unsupported" << ",,,\n";
                break;
            
        }
        
        std::cout << std::endl;
    }
    return ret;

}

// Function to get the current time as a formatted string (not used now, but kept for reference)
std::string getNowTime()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    return oss.str();
}


/// @brief 
///Default to Google's public DNS server.
///Can be overridden by command line argument, 
///otherwise, second argument can specify number of pings.
///Usage: ./ping_logger [address] [count]
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char *argv[])
{

    std::string address = "8.8.8.8", resolved;
    int loop_count = 4;
    if (argc > 1) 
    { 
        address = argv[1]; 
        if (argc > 2) { loop_count = std::stoi(argv[2]); }
    }
    try 
    {
        if (!icmplib::IPAddress::IsCorrect(address, icmplib::IPAddress::Type::Unknown)) {
            resolved = address; address = icmplib::IPAddress(address);
        }
    } 
    catch (...) 
    {
        std::cout << "Ping request could not find host " << address << ". Please check the name and try again." << std::endl;
        return 1;
    }

    int ret = EXIT_SUCCESS;

    // Open CSV file for logging ping results
    std::ofstream fileOut;
    fileOut.open("ping_log.csv", std::ios::out | std::ios::app);
    if (!fileOut.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return EXIT_FAILURE;
    }

    //check if file is empty to write header
    fileOut.seekp(0, std::ios::end);
    if (fileOut.tellp() == 0)
    {
        fileOut << "address,response_type,delay_ms,code,ttl,time_sent,time_received\n";
    }
    
    // Loop given amount to send multiple ping requests after each other
    for(int i = 0; i < loop_count; ++i)
    {
        ret = pingRepeat(address, resolved, fileOut);
    }

    // Close the CSV file after logging is done
    fileOut.close();
    return ret;
}

