#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cctype>
#include <functional>
#include "../include/bpe.h"

using namespace std;

#define END_WORD 256
#define SPACE_TOKEN 32

// Structure for hashing a pair of integers (used for counting bigrams)
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

// Global dictionary for storing merge rules: new_token_id -> (original token A, original token B)
unordered_map<int, pair<int, int>> mergeRules;

/*
   Function buildCorpus:
   - Splits the input text by spaces.
   - For each word, creates a vector of tokens: each character is represented by its ASCII/Unicode code (int),
     followed by a special END_WORD marker.
*/
vector<vector<int>> buildCorpus(const string& text) {
    if (text.empty()) {
        throw invalid_argument("[ERROR] -- Text is empty.");
    }
    
    vector<vector<int>> corpus;
    istringstream iss(text);
    string word;
    while (iss >> word) {
        vector<int> tokens;
        for (char c : word) {
            tokens.push_back(static_cast<int>(c));
        }
        tokens.push_back(END_WORD);
        corpus.push_back(tokens);
    }
    return corpus;
}

/*
   Function getStats:
   - For each word (as a vector of ints), counts the frequency of adjacent pairs (bigrams).
   - Pairs where the second token is END_WORD are skipped (to avoid merging with the end-of-word marker).
*/
unordered_map<pair<int, int>, int, PairHash> getStats(const vector<vector<int>>& corpus) {
    unordered_map<pair<int, int>, int, PairHash> stats;
    for (const auto& word : corpus) {
        for (size_t i = 0; i < word.size() - 1; i++) {
            if (word[i + 1] == END_WORD) continue;
            pair<int, int> bigram = { word[i], word[i + 1] };
            stats[bigram]++;
        }
    }
    return stats;
}

/*
   Function findBestPair:
   - Finds the most frequent adjacent pair among the counted bigrams.
*/
pair<int, int> findBestPair(const unordered_map<pair<int, int>, int, PairHash>& stats) {
    if (stats.empty()) {
        throw invalid_argument("[ERROR] -- No bigrams found.");
    }
    pair<int, int> bestPair = {0, 0};
    int maxFreq = 0;
    for (const auto& p : stats) {
        if (p.second > maxFreq) {
            bestPair = p.first;
            maxFreq = p.second;
        }
    }
    return bestPair;
}

/*
   Function mergePair:
   - For the found best pair (a, b), creates a new token by "merging" them.
   - Saves the merge rule: new_token_id -> (a, b).
   - Iterates through each word in the corpus and replaces all occurrences of the sequence {a, b} with the new token.
*/
void mergePair(vector<vector<int>>& corpus, const pair<int, int>& bestPair, unordered_map<int, pair<int, int>>& mergeRules, int& nextAvailableId) {
    string newToken = to_string(bestPair.first) + to_string(bestPair.second);

    int newTokenId = nextAvailableId++;

    mergeRules[newTokenId] = bestPair;

    for (auto& word : corpus) {
        vector<int> newWord;
        size_t i = 0;
        while (i < word.size()) {
            if (i < word.size() - 1 && word[i] == bestPair.first && word[i + 1] == bestPair.second) {
                newWord.push_back(newTokenId); 
                i += 2;
            } else {
                newWord.push_back(word[i]);
                i++;
            }
        }
        word = newWord; 
    }
}

/*
   Function trainBPE:
   - Iteratively performs the specified number of merges (numMerges).
   - On each iteration, computes bigram statistics, finds the most frequent pair, and performs the merge.
   - Returns the updated corpus.
   - Uses the global mergeRules.
*/
vector<vector<int>> trainBPE(vector<vector<int>> corpus, int numMerges) {
    int nextAvailableId = 1000;  

    for (int i = 0; i < numMerges; i++) {
        auto stats = getStats(corpus);
        if (stats.empty()) break; 

        auto bestPair = findBestPair(stats);
        mergePair(corpus, bestPair, mergeRules, nextAvailableId);
    }
    return corpus;
}

/*
   Function flattenCorpus:
   - Combines the words of the corpus into a single vector of tokens.
   - Inserts a space token (using ASCII code 32) between words.
   - The END_WORD marker is omitted.
*/
vector<int> flattenCorpus(const vector<vector<int>>& corpus) {
    vector<int> tokens;
    bool firstWord = true;
    for (const auto& word : corpus) {
        if (!firstWord) {
            tokens.push_back(SPACE_TOKEN);  
        }
        for (int t : word) {
            if (t == END_WORD) continue;
            tokens.push_back(t);
        }
        firstWord = false;
    }
    return tokens;
}

/*
   Function detokenizeToken:
   - Recursively restores the string representation of a token.
   - If the token is a result of a merge (exists in mergeRules), it detokenizes its components.
   - Otherwise, converts the token (assumed to be a basic character) to char.
*/
string detokenizeToken(int token) {
    if (mergeRules.find(token) != mergeRules.end()) {
        auto children = mergeRules[token];
        return detokenizeToken(children.first) + detokenizeToken(children.second);
    } else {
        // Handle the END_WORD marker or basic character tokens
        if (token == END_WORD) return "";
        return string(1, static_cast<char>(token));
    }
}

/*
   Function detokenize:
   - Converts a vector of tokens (ints) back to a string.
*/
string detokenize(const vector<int>& tokens) {
    string result;
    for (int t : tokens) {
        result += detokenizeToken(t);
    }
    return result;
}

/*
   Function tokenizeBPE:
   - Takes the input text and the number of merge iterations.
   - Builds the corpus, trains BPE (performs merges), and then outputs the final sequence of tokens.
*/
vector<int> tokenizeBPE(const string& text, int numMerges) {
    auto corpus = buildCorpus(text);
    corpus = trainBPE(corpus, numMerges);
    return flattenCorpus(corpus);
}

