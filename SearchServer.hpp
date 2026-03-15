#pragma once

#include <sstream>
#include <thread>

#include "InvertedIndex.hpp"

using namespace std;


// —труктура дл€ хранени€ относительной релевантности

struct RelativeIndex
{
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const
    {
        return (doc_id == other.doc_id && abs(rank - other.rank) < 1e-6);
    }
};

//  ласс поискового сервера

class SearchServer
{
private:
    InvertedIndex& _index;

    vector<string> splitIntoWords(const string& query);


    vector<string> getUniqueWords(const vector<string>& words);


public:
     SearchServer(InvertedIndex& idx) : _index(idx)
    {}

    vector<vector<RelativeIndex>> search(const vector<string>& queries_input);


};