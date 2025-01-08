#pragma once
#include <string>
#include <chrono>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <omp.h>

class Forecaster {
public:
    Forecaster(int numThreads = 8) : m_numThreads(numThreads), m_spotPrice(0.0f) {
        omp_set_num_threads(m_numThreads);
    }

    void setSpotPrice(float price) {
        m_spotPrice = price;
    }

    ~Forecaster() = default;

    bool forecast(const std::string& dataPath) {
        const auto beginTime = std::chrono::system_clock::now();

        // Allocate memory for stock price matrices
        float** stock = new float* [inLoops];
        for (int32_t i = 0; i < inLoops; i++) {
            stock[i] = new float[timeSteps];
        }

        float** avgStock = new float* [outLoops];
        for (int32_t i = 0; i < outLoops; i++) {
            avgStock[i] = new float[timeSteps];
        }

        // Calculate volatility first
        const float volatility = calculateVolatility(m_spotPrice, timeSteps, dataPath);
        if (volatility < 0) {
            // Clean up and return on error
            cleanup(stock, avgStock, nullptr);
            return false;
        }

        // Print initial information
        std::cout << "==============================================\n";
        std::cout << "           Stock Forecasting Tool\n";
        std::cout << "==============================================\n\n";
        std::cout << "Using market volatility: " << volatility << "\n";
        std::cout << "Using spot price: " << m_spotPrice << "\n";
        std::cout << "Using " << m_numThreads << " thread(s)\n\n";
        std::cout << "Computing forecasts... ";

        static constexpr float riskRate = 0.001f;

        // Main computation loop
        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic)
            for (int32_t i = 0; i < outLoops; i++) {
                for (int32_t j = 0; j < inLoops; j++) {
                    float* prices = runBlackScholesModel(m_spotPrice, timeSteps, riskRate, volatility);
                    for (int32_t k = 0; k < timeSteps; k++) {
                        stock[j][k] = prices[k];
                    }
                    delete[] prices;
                }
                float* meanPrices = find2dMean(stock, inLoops, timeSteps);
                for (int32_t k = 0; k < timeSteps; k++) {
                    avgStock[i][k] = meanPrices[k];
                }
                delete[] meanPrices;
            }
        }

        // Calculate final average
        float* optStock = find2dMean(avgStock, outLoops, timeSteps);

        // Write results to file
        std::ofstream filePtr;
        filePtr.open("D:/low-latency-trading-system/Forecaster/forecast.csv", std::ofstream::out);
        if (!filePtr.is_open()) {
            std::cerr << "Couldn't open forecast.csv! Exiting..\n";
            cleanup(stock, avgStock, optStock);
            return false;
        }

        for (int32_t i = 0; i < timeSteps; i++) {
            filePtr << optStock[i] << "\n";
        }
        filePtr.close();

        // Clean up allocated memory
        cleanup(stock, avgStock, optStock);

        std::cout << "done!\nTime taken: " <<
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - beginTime).count() << "s\n";

        return true;
    }

    void setNumThreads(int threads) {
        m_numThreads = threads;
        omp_set_num_threads(m_numThreads);
    }

private:
    float calculateVolatility(float spotPrice, int32_t timeSteps, const std::string& dataPath) {
        std::ifstream filePtr;
        filePtr.open(dataPath, std::ifstream::in);
        if (!filePtr.is_open()) {
            std::cerr << "Cannot open " << dataPath << "! Exiting..\n";
            return -1.0f;
        }

        std::string line;
        if (!std::getline(filePtr, line)) {
            std::cerr << "Cannot read from " << dataPath << "! Exiting..\n";
            filePtr.close();
            return -1.0f;
        }
        filePtr.close();

        int32_t i = 0, len = timeSteps - 1;
        std::unique_ptr<float[]> priceArr = std::make_unique<float[]>(timeSteps - 1);
        std::istringstream iss(line);
        std::string token;

        while (std::getline(iss, token, ',')) {
            priceArr[i++] = std::stof(token);
        }

        float sum = spotPrice;
        for (i = 0; i < len; i++) {
            sum += priceArr[i];
        }
        float meanPrice = sum / (len + 1);

        sum = std::powf((spotPrice - meanPrice), 2.0f);
        for (i = 0; i < len; i++) {
            sum += std::powf((priceArr[i] - meanPrice), 2.0f);
        }

        float stdDev = std::sqrtf(sum);
        return stdDev / 100.0f;
    }

    float* find2dMean(float** matrix, int32_t numLoops, int32_t timeSteps) {
        float* avg = new float[timeSteps];

        for (int32_t i = 0; i < timeSteps; i++) {
            float sum = 0.0f;
            #pragma omp parallel for reduction(+:sum)
            for (int32_t j = 0; j < numLoops; j++) {
                sum += matrix[j][i];
            }
            avg[i] = sum / numLoops;
        }

        return avg;
    }

    float genRand(float mean, float stdDev) {
        static thread_local std::random_device rd;
        static thread_local std::mt19937 generator(rd());
        std::normal_distribution<float> distribution(mean, stdDev);
        return distribution(generator);
    }

    float* runBlackScholesModel(float spotPrice, int32_t timeSteps, float riskRate, float volatility) {
        static constexpr float mean = 0.0f, stdDev = 1.0f;
        float deltaT = 1.0f / timeSteps;
        std::unique_ptr<float[]> normRand = std::make_unique<float[]>(timeSteps - 1);
        float* stockPrice = new float[timeSteps];
        stockPrice[0] = spotPrice;

        for (int32_t i = 0; i < timeSteps - 1; i++) {
            normRand[i] = genRand(mean, stdDev);
        }

        for (int32_t i = 0; i < timeSteps - 1; i++) {
            stockPrice[i + 1] = stockPrice[i] * exp(
                ((riskRate - (std::powf(volatility, 2.0f) / 2.0f)) * deltaT) +
                (volatility * normRand[i] * std::sqrtf(deltaT))
            );
        }

        return stockPrice;
    }

    void cleanup(float** stock, float** avgStock, float* optStock) {
        if (stock) {
            for (int32_t i = 0; i < inLoops; i++) {
                delete[] stock[i];
            }
            delete[] stock;
        }

        if (avgStock) {
            for (int32_t i = 0; i < outLoops; i++) {
                delete[] avgStock[i];
            }
            delete[] avgStock;
        }

        if (optStock) {
            delete[] optStock;
        }
    }

    int m_numThreads;
    float m_spotPrice;
    static constexpr int32_t inLoops = 100;     // Inner loop iterations
    static constexpr int32_t outLoops = 10000;  // Outer loop iterations
    static constexpr int32_t timeSteps = 180;   // Stock market time-intervals (min)
};