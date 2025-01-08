#include "./Generate_Orders/GenerateOrders.hpp"
#include "./Process_Orders/OrderPipeline.hpp"
#include "./Order_Book/Book.hpp"
#include "./Order_Book/Order.hpp"
#include "./Forecaster/Forecaster.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    Book* book = new Book();

    OrderPipeline orderPipeline(book);

    //GenerateOrders generateOrders(book);

    //generateOrders.createInitialOrders(10000, 500);

    orderPipeline.processOrdersFromFile("D:/low-latency-trading-system/initialOrders.txt");

    //generateOrders.createOrders(5000000);

    //auto start = std::chrono::high_resolution_clock::now();

    //orderPipeline.processOrdersFromFile("D:/low-latency-trading-system/orders.txt");

    //auto stop = std::chrono::high_resolution_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    //std::cout << "Time taken to process orders: " << duration.count() << " milliseconds" << std::endl;

    // forecast
    std::string response;
    std::cout << "\nWould you like to forecast the stock price? (y/n): ";
    std::getline(std::cin, response);

    if (response == "y" || response == "Y") {
        std::string dataPath;
        std::cout << "Enter the path to data.csv: ";
        std::getline(std::cin, dataPath);

        Forecaster forecaster;
        if (!forecaster.forecast(dataPath)) {
            std::cerr << "Forecasting failed!" << std::endl;
        }
        else {
            std::cout << "Forecast has been written to forecast.csv" << std::endl;
        }
    }


    delete book;
    return 0;
}

//#include "Order_Book/Book.hpp"
//#include "TestUtils/OrderExecutor.hpp"
//#include "TestUtils/OrderGenerator.hpp"
//
//#include <chrono>
//#include <iostream>
//#include <vector>
//
//int main() {
//    Book* book = new Book();
//    OrderGenerator orderGenerator(book);
//    OrderExecutor orderExecutor(book);
//
//    orderGenerator.randomInitialOrders("D:/low-latency-trading-system/generatedData/initialOrders.txt", 10000, 500);
//
//    auto start = std::chrono::high_resolution_clock::now();
//    orderExecutor.loadOrdersFromFile("D:/low-latency-trading-system/generatedData/initialOrders.txt", "D:/low-latency-trading-system/generatedData/initialPerformance.txt");
//
//    auto stop = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//    std::cerr << "Total Time for Initial Orders: " << duration.count() << " ms" << std::endl;
//
//    // create larger number of incoming orders
//    orderGenerator.randomOrders("D:/low-latency-trading-system/generatedData/randomOrders.txt", 1000000);
//    start = std::chrono::high_resolution_clock::now();
//    orderExecutor.loadOrdersFromFile("D:/low-latency-trading-system/generatedData/randomOrders.txt",
//        "D:/low-latency-trading-system/generatedData/finalPerformanceMetrics.txt");
//
//    stop = std::chrono::high_resolution_clock::now();
//    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
//    std::cerr << "Total Time to Process Orders: " << duration.count() << " ms" << std::endl;
//
//    delete book;
//    return 0;
//}