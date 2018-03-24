#include <cstddef>
#include <cstdlib>
#include <string>

#include <iostream>
#include <chrono>
#include <ctime>

#include <algorithm>
#include <vector>
#include <random>

#include "TreeMap.h"
#include "HashMap.h"

namespace
{

    template <typename K, typename V>
    using TreeMap = aisdi::TreeMap<K, V>;
    template <typename K, typename V>
    using HashMap = aisdi::HashMap<K, V>;

    void performHashTest(std::size_t n, std::vector<int>& keys)
    {

        HashMap<int, std::string> map;

        std::chrono::time_point<std::chrono::system_clock> start, end;

        start = std::chrono::system_clock::now();
        for (auto it = keys.begin(); it !=  keys.end(); ++it)
            map[*it] = "Value";
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Adding time in HashMap: " << elapsed_seconds.count() << "s\n";



        start = std::chrono::system_clock::now();
        for (auto it = keys.begin(); it !=  keys.end(); ++it)
            map[*it] = "ChangedValue";
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "Changing every position in HashMap: " << elapsed_seconds.count() << "s\n";


        start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < n; ++i)
            map.remove(begin(map));
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "Removing all Map  in HashMap: " << elapsed_seconds.count() << "s\n";
    }




    void performTreeTest(std::size_t n, std::vector<int>& keys)
    {
        TreeMap<int, std::string> map;


        std::chrono::time_point<std::chrono::system_clock> start, end;

        start = std::chrono::system_clock::now();
        for (auto it = keys.begin(); it !=  keys.end(); ++it)
            map[*it] = "Value";
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Adding time in TreeMap: " << elapsed_seconds.count() << "s\n";

        start = std::chrono::system_clock::now();
        for (auto it = keys.begin(); it !=  keys.end(); ++it)
            map[*it] = "ChangedValue";
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "Changing every position in Treemap: " << elapsed_seconds.count() << "s\n";



        start = std::chrono::system_clock::now();
        for (std::size_t i = 0; i < n; ++i)
            map.remove(begin(map));
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "Removing all Map time in TreeMap" << elapsed_seconds.count() << "s\n";


    }

} // namespace

int main(int argc, char** argv)
{
    srand(time(NULL));
    const std::size_t numberOfRepeat = argc > 1 ? std::atoll(argv[1]) : 10000;

    std::vector<int> keys;
    for (size_t i = 0; i < numberOfRepeat;++i)
    {

        keys.push_back(i);


    }
    std::cout<<" \n Iterations: "<<argv[1]<<std::endl;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle (keys.begin(), keys.end(), std::default_random_engine(seed));

    performHashTest(numberOfRepeat, keys);
    performTreeTest(numberOfRepeat, keys);
    return 0;
}
