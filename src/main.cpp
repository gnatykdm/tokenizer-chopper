#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "../include/functions.h"

#define NUM_MERGES 20

class Tokenizer {
public:
    std::vector<int> tokenizeText(int numMerges, const std::string& textCorpus) {
        if (numMerges <= 0) {
            throw std::invalid_argument("[ERROR] -- Number of merges can't be less then zero.");
        }

        std::vector<int> tokenCorpus = tokenizeBPE(textCorpus, numMerges);
        return tokenCorpus;
    }

    std::string detokenizeText(const std::vector<int> tokens) {
        if (tokens.empty()) {
            throw std::invalid_argument("[ERROR] - Token vector is empty.");
        }

        std::string detokenizedText = detokenize(tokens);
        return detokenizedText;
    }
};

int main() {
    std::string text;

    std::cout << "Print text: ";
    getline(std::cin, text);
    
    Tokenizer tokenizer = Tokenizer();
    std::vector<int> finalTokens = tokenizer.tokenizeText(NUM_MERGES, text);

    std::cout << "Final tokens (as ints):" << std::endl;
    for (int t : finalTokens) {
        std::cout << t << " ";
    }
    std::cout << std::endl;

    std::string detokText = detokenize(finalTokens);
    std::cout << "Detokenized text: " << detokText << std::endl;
    
    return 0;
}