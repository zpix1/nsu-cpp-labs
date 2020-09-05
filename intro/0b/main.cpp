#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <utility>

int main(int argc, char** argv) {
    std::ifstream text_file(argv[1]);
    std::ofstream csv_file(argv[2]);

    std::string current;

    std::unordered_map<std::string, int> counter;
    int total_words = 0;
    while (!text_file.eof()) {
        std::getline(text_file, current);

        const std::regex word_regex("[\\w\\d]+");  
        std::sregex_iterator next(current.begin(), current.end(), word_regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            std::string match_str = match.str();
            std::for_each(match_str.begin(), match_str.end(), [](char & c){
                c = std::tolower(c);
            });
            counter[match_str] += 1;
            total_words += 1;
            next++;
        }
    }

    std::vector< std::pair<int, std::string> > items;

    for (auto k: counter) {
        items.push_back(std::make_pair(k.second, k.first));
    }
    std::sort(items.begin(), items.end());
    std::reverse(items.begin(), items.end());
    for (auto v: items) {
        float percent = v.first * 100.f / total_words;
        csv_file << v.second << ", " << v.first << ", " << percent << std::endl;
    }
}