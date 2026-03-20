#include "SearchServer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>


using namespace std;



vector<string> SearchServer:: splitIntoWords(const string& query)
{
    vector<string> words;
    stringstream ss(query);
    string word;
    while (ss >> word)
    {
        words.push_back(word);
    }
    return words;
}

vector<string> SearchServer::getUniqueWords(const vector<string>& words)
{
    vector<string> unique;
    for (const auto& w : words)
    {
        if (find(unique.begin(), unique.end(), w) == unique.end())
        {
            unique.push_back(w);
        }
    }
    return unique;
}

vector<vector<RelativeIndex>> SearchServer::search(const vector<string>& queries_input)
{
    vector<vector<RelativeIndex>> results;

    for (const auto& query : queries_input)
    {
        cout << "\nProcessing query: \"" << query << "\"" << endl;

        if (query.empty())
        {
            results.push_back({});
            continue;
        }

        // Разбиваем запрос на слова и получаем уникальные
        vector<string> words = splitIntoWords(query);
        vector<string> uniqueWords = getUniqueWords(words);

        cout << "Unique words: ";
        for (const auto& w : uniqueWords) cout << w << " ";
        cout << endl;

        // ЕСЛИ НЕТ СЛОВ - ПУСТОЙ РЕЗУЛЬТАТ
        if (uniqueWords.empty())
        {
            results.push_back({});
            continue;
        }

        // Карта для хранения суммарной частоты слов в документах
        map<size_t, size_t> docWordCount;
        // Карта для подсчета количества уникальных слов запроса в каждом документе
        map<size_t, size_t> docUniqueWordCount;

        // Обрабатываем все слова из запроса
        for (const auto& word : uniqueWords)
        {
            cout << "\nProcessing word \"" << word << "\":" << endl;

            auto wordEntries = _index.GetWordCount(word);

            if (wordEntries.empty())
            {
                cout << "  Word not found in any document" << endl;
                continue;
            }

            // Для каждого документа, содержащего текущее слово
            for (const auto& entry : wordEntries)
            {
                // Добавляем частоту слова
                docWordCount[entry.doc_id] += entry.count;
                // Увеличиваем счетчик уникальных слов для этого документа
                docUniqueWordCount[entry.doc_id]++;

                cout << "  doc " << entry.doc_id << ": count=" << entry.count
                    << ", total freq=" << docWordCount[entry.doc_id]
                    << ", unique words=" << docUniqueWordCount[entry.doc_id] << endl;
            }
        }

        // ПРОВЕРЯЕМ, ЕСТЬ ЛИ ДОКУМЕНТЫ
        if (docWordCount.empty())
        {
            cout << "\nFinal result: No documents contain any words from query" << endl;
            results.push_back({});
            continue;
        }

        cout << "\nDocuments containing words from query:" << endl;
        cout << "doc_id | total frequency | unique words | contains all words?" << endl;

        // Проверяем, какие документы содержат все слова запроса
        size_t totalUniqueWords = uniqueWords.size();

        for (const auto& [doc_id, uniqueCount] : docUniqueWordCount)
        {
            bool containsAll = (uniqueCount == totalUniqueWords);
            cout << doc_id << "      | " << docWordCount[doc_id]
                << "               | " << uniqueCount
                << "             | " << (containsAll ? "yes" : "no") << endl;
        }

        // Находим максимальную абсолютную релевантность
        size_t maxAbsRelevance = 0;
        for (const auto& [doc_id, count] : docWordCount)
        {
            if (count > maxAbsRelevance)
            {
                maxAbsRelevance = count;
            }
        }

        cout << "\nMax absolute relevance: " << maxAbsRelevance << endl;

        // Рассчитываем относительную релевантность для ВСЕХ документов
        vector<RelativeIndex> relevance;
        for (const auto& [doc_id, count] : docWordCount)
        {
            float rank = static_cast<float>(count) / maxAbsRelevance;
            relevance.push_back({ doc_id, rank });
            cout << "doc " << doc_id << ": abs=" << count << ", rel=" << rank;

            // Отмечаем, содержит ли документ все слова
            if (docUniqueWordCount[doc_id] == totalUniqueWords)
            {
                cout << " (contains all words)";
            }
            cout << endl;
        }

        // Сортируем по убыванию релевантности, но сначала документы со всеми словами
        sort(relevance.begin(), relevance.end(),
            [&docUniqueWordCount, totalUniqueWords](const RelativeIndex& a, const RelativeIndex& b)
            {
                bool a_has_all = (docUniqueWordCount.at(a.doc_id) == totalUniqueWords);
                bool b_has_all = (docUniqueWordCount.at(b.doc_id) == totalUniqueWords);

                // Сначала документы, содержащие все слова
                if (a_has_all != b_has_all)
                {
                    return a_has_all > b_has_all; // true (все слова) идет первым
                }

                // Если одинаковый статус (оба содержат все слова или оба частичные),
                // сортируем по релевантности
                if (abs(a.rank - b.rank) < 1e-6)
                {
                    return a.doc_id < b.doc_id;
                }
                return a.rank > b.rank;
            });

        // Оставляем только первые 5 документов
        const size_t MAX_RESULTS = 5;
        if (relevance.size() > MAX_RESULTS)
        {
            relevance.resize(MAX_RESULTS);
        }

        // Выводим отсортированные результаты (только первые 5)
        cout << "\nTop " << MAX_RESULTS << " results (documents with all words first, then by relevance):" << endl;
        for (const auto& rel : relevance)
        {
            cout << "doc " << rel.doc_id << ": rank=" << rel.rank;

            // Показываем, содержит ли документ все слова
            if (docUniqueWordCount.at(rel.doc_id) == totalUniqueWords)
            {
                cout << " (all words)";
            }
            else
            {
                cout << " (partial: " << docUniqueWordCount.at(rel.doc_id)
                    << "/" << totalUniqueWords << " words)";
            }
            cout << endl;
        }

        results.push_back(relevance);
    }

    return results;
}