#include <iostream>
#include <vector>
#include <thread>
#include <numeric>
#include <random>
#include <chrono>

class ArraySum {
public:
    ArraySum(size_t size) {
        array.resize(size);
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(1, 10);

        for (auto& val : array) {
            val = dist(rng);
        }
    }

    long long sequentialSum() const {
        return std::accumulate(array.begin(), array.end(), 0LL);
    }

    long long parallelSum(size_t numThreads) {
        std::vector<std::thread> threads;
        std::vector<long long> partialSums(numThreads, 0);

        size_t chunkSize = array.size() / numThreads;

        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * chunkSize;
            size_t end = (i == numThreads - 1) ? array.size() : (i + 1) * chunkSize;

            threads.emplace_back([this, &partialSums, i, start, end]() {
                long long sum = 0;
                for (size_t j = start; j < end; ++j) {
                    sum += array[j];
                }
                partialSums[i] = sum;
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        return std::accumulate(partialSums.begin(), partialSums.end(), 0LL);
    }

private:
    std::vector<int> array;
};

int main() {
    std::vector<size_t> arraySizes = {100'000, 1'000'000, 10'000'000};
    std::vector<size_t> threadCounts = {1, 4, 8, 10};

    unsigned int cores = std::thread::hardware_concurrency();
    std::cout << "Количество ядер процессора: " << cores << "\n";

    for (size_t size : arraySizes) {
        std::cout << "\nРазмер массива: " << size << "\n";
        ArraySum arr(size);

        for (size_t threads : threadCounts) {
            auto start = std::chrono::high_resolution_clock::now();
            long long sum = (threads == 1) ? arr.sequentialSum() : arr.parallelSum(threads);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end - start;
            std::cout << "Потоков: " << threads
                      << " | Сумма: " << sum
                      << " | Время: " << duration.count() << " секунд\n";
        }
    }

    return 0;
}
