#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>   
#include <vector>   

std::vector<int> tokenizeBPE(const std::string& text, int numMerges);
std::string detokenize(const std::vector<int>& tokens);

#endif
