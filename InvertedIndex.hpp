#pragma once
#include <string>
#include <vector>
#include <map>

#include "Entry.hpp"
using namespace std;

// Класс для инвертированного индекса
class InvertedIndex
{
private:
    vector<string> docs;
    map<string, vector<Entry>> freq_dictionary;

    vector<string> splitIntoWords(const string& text);

    map<string, size_t> countWordsInDocument(const string& text);

public:
    InvertedIndex() = default;

    void UpdateDocumentBase(const vector<string>& input_docs);

    vector<Entry> GetWordCount(const string& word);

    size_t GetDocsCount() const;
};