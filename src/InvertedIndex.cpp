#include "InvertedIndex.hpp"
#include "Entry.hpp"
 
#include <sstream>
#include <thread>


vector<string> InvertedIndex::splitIntoWords(const string& text)
{
    vector<string> words;
    stringstream ss(text);
    string word;
    while (ss >> word)
    {
        words.push_back(word);
    }
    return words;
}

map<string, size_t> InvertedIndex::countWordsInDocument(const string& text)
{
    map<string, size_t> wordCount;
    vector<string> words = splitIntoWords(text);
    for (const auto& word : words)
    {
        wordCount[word]++;
    }
    return wordCount;
}
 

    void InvertedIndex::UpdateDocumentBase(const vector<string>& input_docs)
    {
        docs = input_docs;
        freq_dictionary.clear();

        vector<map<string, size_t>> docWordCounts(docs.size());
        vector<thread> threads;

        for (size_t i = 0; i < docs.size(); i++)
        {
            threads.emplace_back([this, i, &docWordCounts]
                {
                    docWordCounts[i] = countWordsInDocument(docs[i]);
                });
        }

        for (auto& t : threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        // Объединение результатов
        for (size_t doc_id = 0; doc_id < docWordCounts.size(); doc_id++)
        {
            for (const auto& [word, count] : docWordCounts[doc_id])
            {
                freq_dictionary[word].push_back({ doc_id, count });
            }
        }
    }

    vector<Entry> InvertedIndex::GetWordCount(const string& word) 
    {
        auto it = freq_dictionary.find(word);
        if (it != freq_dictionary.end())
        {
            return it->second;
        }
        return {};
    }

    size_t InvertedIndex::GetDocsCount() const
    {
        return docs.size();
    }

   