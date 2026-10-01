#include "benchmark/bench.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;

    // Размеры множества.
    // Для Tree_nat большие N + sorted insertion могут работать очень долго.
    constexpr std::array<size_t, 5> SIZES = {
        100,
        1000,
        5000,
        10000,
        50000
    };

    // Количество запросов для query-бенчмарков.
    constexpr size_t QUERY_COUNT = 100000;

    // Сколько раз повторяем измерение.
    constexpr int REPEATS = 5;

    enum class Distribution
    {
        Random,
        Ascending,
        Descending,
        Clustered
    };

    const char* DistributionName(Distribution d)
    {
        switch (d)
        {
            case Distribution::Random:     return "random";
            case Distribution::Ascending:  return "ascending";
            case Distribution::Descending: return "descending";
            case Distribution::Clustered:  return "clustered";
        }

        return "unknown";
    }

    constexpr std::array<Distribution, 4> DISTRIBUTIONS = {
        Distribution::Random,
        Distribution::Ascending,
        Distribution::Descending,
        Distribution::Clustered
    };

    // Глобальный checksum не даёт компилятору считать результаты запросов
    // ненужными.
    uint64_t g_checksum = 0;

    std::mt19937_64 rng(123456789);

    std::vector<int> GenerateKeys(size_t n, Distribution distribution)
    {
        std::vector<int> result(n);

        switch (distribution)
        {
            case Distribution::Ascending:
            {
                std::iota(result.begin(), result.end(), 0);
                break;
            }

            case Distribution::Descending:
            {
                std::iota(result.begin(), result.end(), 0);
                std::reverse(result.begin(), result.end());
                break;
            }

            case Distribution::Random:
            {
                std::iota(result.begin(), result.end(), 0);
                std::shuffle(result.begin(), result.end(), rng);
                break;
            }

            case Distribution::Clustered:
            {
                // Ключи находятся в компактном диапазоне около нуля,
                // но остаются уникальными.
                std::iota(result.begin(), result.end(), 0);

                for (size_t i = 0; i < n; ++i)
                {
                    result[i] = static_cast<int>(result[i] * 2) -
                                static_cast<int>(n);
                }

                std::shuffle(result.begin(), result.end(), rng);
                break;
            }
        }

        return result;
    }

    std::vector<int> GenerateQueryKeys(
        size_t n,
        Distribution distribution)
    {
        std::vector<int> result(QUERY_COUNT);

        switch (distribution)
        {
            case Distribution::Random:
            {
                std::uniform_int_distribution<int> dist(
                    -static_cast<int>(n),
                    static_cast<int>(2 * n));

                for (int& x : result)
                    x = dist(rng);

                break;
            }

            case Distribution::Ascending:
            {
                for (size_t i = 0; i < result.size(); ++i)
                    result[i] = static_cast<int>(i % (n + 1));

                break;
            }

            case Distribution::Descending:
            {
                for (size_t i = 0; i < result.size(); ++i)
                    result[i] =
                        static_cast<int>(n - (i % (n + 1)));

                break;
            }

            case Distribution::Clustered:
            {
                std::uniform_int_distribution<int> dist(
                    -static_cast<int>(n),
                    static_cast<int>(n));

                for (int& x : result)
                    x = dist(rng);

                break;
            }
        }

        return result;
    }

    double Median(std::vector<double> values)
    {
        std::sort(values.begin(), values.end());

        const size_t n = values.size();

        if (n % 2 == 1)
            return values[n / 2];

        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    }

    void PrepareTree(
        Tree& tree,
        const std::vector<int>& keys)
    {
        tree.Clear();

        for (int x : keys)
            tree.Insert(x);
    }

    double MeasureInsert(
        Tree& tree,
        const std::vector<int>& keys)
    {
        std::vector<double> results;
        results.reserve(REPEATS);

        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            tree.Clear();

            const auto start = Clock::now();

            for (int x : keys)
                tree.Insert(x);

            const auto end = Clock::now();

            const double seconds =
                std::chrono::duration<double>(end - start).count();

            const double throughput =
                static_cast<double>(keys.size()) / seconds;

            results.push_back(throughput);
        }

        return Median(results);
    }

    double MeasureSmaller(
        Tree& tree,
        size_t n,
        Distribution distribution)
    {
        PrepareTree(tree, GenerateKeys(n, distribution));

        const std::vector<int> queries =
            GenerateQueryKeys(n, distribution);

        std::vector<double> results;
        results.reserve(REPEATS);

        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            uint64_t checksum = 0;

            const auto start = Clock::now();

            for (int x : queries)
                checksum += tree.Key_smaller(x);

            const auto end = Clock::now();

            g_checksum += checksum;

            const double seconds =
                std::chrono::duration<double>(end - start).count();

            const double throughput =
                static_cast<double>(QUERY_COUNT) / seconds;

            results.push_back(throughput);
        }

        return Median(results);
    }

    double MeasureKth(
        Tree& tree,
        size_t n,
        Distribution distribution)
    {
        PrepareTree(tree, GenerateKeys(n, distribution));

        std::uniform_int_distribution<size_t> dist(1, n);

        std::vector<size_t> queries(QUERY_COUNT);

        for (size_t& x : queries)
            x = dist(rng);

        std::vector<double> results;
        results.reserve(REPEATS);

        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            uint64_t checksum = 0;

            const auto start = Clock::now();

            for (size_t i : queries)
                checksum +=
                    static_cast<uint64_t>(tree.Keys_small_number(
                        static_cast<int>(i)));

            const auto end = Clock::now();

            g_checksum += checksum;

            const double seconds =
                std::chrono::duration<double>(end - start).count();

            const double throughput =
                static_cast<double>(QUERY_COUNT) / seconds;

            results.push_back(throughput);
        }

        return Median(results);
    }

    double MeasureRange(
        Tree& tree,
        size_t n,
        Distribution distribution)
    {
        PrepareTree(tree, GenerateKeys(n, distribution));

        std::uniform_int_distribution<int> dist(
            0,
            static_cast<int>(n - 1));

        std::vector<std::pair<int, int>> queries(QUERY_COUNT);

        for (auto& [l, r] : queries)
        {
            l = dist(rng);
            r = dist(rng);

            if (l > r)
                std::swap(l, r);

            if (l == r)
                ++r;
        }

        std::vector<double> results;
        results.reserve(REPEATS);

        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            uint64_t checksum = 0;

            const auto start = Clock::now();

            for (const auto& [l, r] : queries)
                checksum += tree.Keys_in_interval(l, r);

            const auto end = Clock::now();

            g_checksum += checksum;

            const double seconds =
                std::chrono::duration<double>(end - start).count();

            const double throughput =
                static_cast<double>(QUERY_COUNT) / seconds;

            results.push_back(throughput);
        }

        return Median(results);
    }

    double MeasureMixed(
        Tree& tree,
        size_t n,
        Distribution distribution)
    {
        std::vector<int> initial =
            GenerateKeys(n, distribution);

        PrepareTree(tree, initial);

        // 50% insert
        // 20% range
        // 15% kth
        // 15% smaller

        constexpr size_t MIXED_COUNT = 100000;

        std::uniform_int_distribution<int> keyDist(
            -static_cast<int>(n),
            static_cast<int>(2 * n));

        std::uniform_int_distribution<size_t> indexDist(
            1, n);

        std::uniform_int_distribution<int> operationDist(0, 99);

        std::vector<double> results;
        results.reserve(REPEATS);

        for (int repeat = 0; repeat < REPEATS; ++repeat)
        {
            tree.Clear();

            for (int x : initial)
                tree.Insert(x);

            uint64_t checksum = 0;
            size_t inserted = n;

            const auto start = Clock::now();

            for (size_t i = 0; i < MIXED_COUNT; ++i)
            {
                const int operation = operationDist(rng);

                if (operation < 50)
                {
                    // Insert.
                    tree.Insert(
                        static_cast<int>(inserted + i + 1));
                }
                else if (operation < 70)
                {
                    // Range query.
                    int l = keyDist(rng);
                    int r = keyDist(rng);

                    if (l > r)
                        std::swap(l, r);

                    checksum += tree.Keys_in_interval(l, r);
                }
                else if (operation < 85)
                {
                    // kth.
                    const size_t currentSize =
                        inserted + i + 1;

                    std::uniform_int_distribution<size_t>
                        currentIndexDist(1, currentSize);

                    const int index =
                        static_cast<int>(
                            currentIndexDist(rng));

                    checksum += static_cast<uint64_t>(
                        tree.Keys_small_number(index));
                }
                else
                {
                    // smaller.
                    checksum += tree.Key_smaller(
                        keyDist(rng));
                }
            }

            const auto end = Clock::now();

            g_checksum += checksum;

            const double seconds =
                std::chrono::duration<double>(
                    end - start).count();

            const double throughput =
                static_cast<double>(MIXED_COUNT) / seconds;

            results.push_back(throughput);
        }

        return Median(results);
    }

    void WriteHeader(std::ofstream& file)
    {
        file << "# N random ascending descending clustered\n";
    }

    template <typename BenchmarkFunction>
    void RunBenchmarkFile(
        Tree& tree,
        const std::string& path,
        BenchmarkFunction function)
    {
        std::ofstream file(path);

        if (!file)
        {
            std::cerr
                << "Failed to open: "
                << path << '\n';

            return;
        }

        WriteHeader(file);

        for (size_t n : SIZES)
        {
            file << n;

            for (Distribution distribution : DISTRIBUTIONS)
            {
                const double throughput =
                    function(n, distribution);

                file << ' ' << throughput;
            }

            file << '\n';

            std::cout
                << "N = " << n
                << " finished\n";
        }
    }
}

void RunBenchmarks(
    Tree& tree,
    const std::string& tree_name)
{
    const std::string directory =
        "benchmarks/data/" + tree_name;

    std::error_code error;
    std::filesystem::create_directories(
        directory,
        error);

    if (error)
    {
        std::cerr
            << "Failed to create benchmark directory: "
            << directory << '\n';

        return;
    }

    std::cout
        << "Running benchmarks for: "
        << tree_name << '\n';

    // INSERT
    RunBenchmarkFile(
        tree,
        directory + "/insert.txt",
        [&](size_t n, Distribution distribution)
        {
            const auto keys =
                GenerateKeys(n, distribution);

            return MeasureInsert(tree, keys);
        });

    // n x
    RunBenchmarkFile(
        tree,
        directory + "/smaller.txt",
        [&](size_t n, Distribution distribution)
        {
            return MeasureSmaller(
                tree,
                n,
                distribution);
        });

    // m i
    RunBenchmarkFile(
        tree,
        directory + "/kth.txt",
        [&](size_t n, Distribution distribution)
        {
            return MeasureKth(
                tree,
                n,
                distribution);
        });

    // q l r
    RunBenchmarkFile(
        tree,
        directory + "/range.txt",
        [&](size_t n, Distribution distribution)
        {
            return MeasureRange(
                tree,
                n,
                distribution);
        });

    // MIXED
    RunBenchmarkFile(
        tree,
        directory + "/mixed.txt",
        [&](size_t n, Distribution distribution)
        {
            return MeasureMixed(
                tree,
                n,
                distribution);
        });

    tree.Clear();

    std::cout
        << "All benchmarks finished.\n";

    std::cout
        << "Checksum: "
        << g_checksum << '\n';
}