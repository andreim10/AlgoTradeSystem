#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>

struct PriceData {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long long volume;
};

// Funcție care calculează Media Mobilă Simplă (SMA) pentru o anumită zi
double calculateSMA(const std::vector<PriceData>& data, int currentIndex, int period) {
    if (currentIndex < period - 1) {
        return data[currentIndex].close; // Nu avem destule zile în urmă, returnăm prețul curent
    }
    
    double sum = 0.0;
    for (int i = currentIndex - period + 1; i <= currentIndex; ++i) {
        sum += data[i].close;
    }
    return sum / period;
}

int main() {
    std::cout << "=== [C++] Pornire motor de analiza financiara ===" << std::endl;

    std::string inputPath = "../data_fetcher/AAPL_data.csv";
    std::ifstream inputFile(inputPath);

    if (!inputFile.is_open()) {
        std::cerr << "[C++] Eroare: Nu s-a putut deschide fisierul de date!" << std::endl;
        return 1;
    }

    std::string line;
    std::vector<PriceData> history;

    // Ignorăm header-ul CSV-ului
    std::getline(inputFile, line);

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        PriceData row;

        std::getline(ss, row.date, ',');
        std::getline(ss, cell, ','); row.open = std::stod(cell);
        std::getline(ss, cell, ','); row.high = std::stod(cell);
        std::getline(ss, cell, ','); row.low = std::stod(cell);
        std::getline(ss, cell, ','); row.close = std::stod(cell);
        std::getline(ss, cell, ','); row.volume = std::stoll(cell);

        history.push_back(row);
    }
    inputFile.close();

    std::cout << "[C++] Date incarcate. Se ruleaza strategia SMA (Perioada: 5 zile)..." << std::endl;

    // Deschidem un nou fișier în care salvăm semnalele pentru modulul de Java
    std::ofstream outputFile("signals.csv");
    outputFile << "date,price,sma,signal\n";

    int period = 5; // Medie pe 5 zile
    int buyCount = 0, sellCount = 0;

    for (size_t i = 0; i < history.size(); ++i) {
        double currentPrice = history[i].close;
        double sma = calculateSMA(history, i, period);
        std::string signal = "HOLD"; // Implicit nu facem nimic

        // Logica strategiei:
        if (i >= (size_t)period - 1) {
            if (currentPrice < sma * 0.99) { // Dacă prețul e cu 1% mai mic decât media
                signal = "BUY";
                buyCount++;
            } else if (currentPrice > sma * 1.01) { // Dacă prețul e cu 1% mai mare decât media
                signal = "SELL";
                sellCount++;
            }
        }

        // Salvăm în CSV
        outputFile << history[i].date << "," 
                   << currentPrice << "," 
                   << sma << "," 
                   << signal << "\n";
    }
    outputFile.close();

    std::cout << "[C++] Strategie finalizata cu succes!" << std::endl;
    std::cout << "[C++] Semnale generate: " << buyCount << " CUMPARARI | " << sellCount << " VANZARI." << std::endl;
    std::cout << "[C++] Rezultatele au fost salvate in 'trading_engine/signals.csv'." << std::endl;

    return 0;
}