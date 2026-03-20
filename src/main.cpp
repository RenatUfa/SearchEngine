#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>

#include "ConverterJson.hpp"
#include "InvertedIndex.hpp"
#include "SearchServer.hpp"
#include "Entry.hpp"

using json = nlohmann::json;
using namespace std;



int main()
{
    try
    {
         
        ConverterJSON converter;  

        int limit = converter.GetResponsesLimit();
        cout << "Responses limit: " << limit << endl;

        vector<string> documents = converter.GetTextDocuments();
        cout << "Loaded " << documents.size() << " documents" << endl;

        vector<string> queryRequests = converter.GetRequests();
        cout << "Loaded " << queryRequests.size() << " requests" << endl;

        if (!documents.empty() && !queryRequests.empty())
        {
            InvertedIndex idx;
            idx.UpdateDocumentBase(documents);

            SearchServer srv(idx);
            auto searchResults = srv.search(queryRequests);

            converter.putAnswers(searchResults);

            // Вывод результатов
            for (size_t i = 0; i < searchResults.size(); i++)
            {
                cout << "Request " << i + 1 << ": ";
                if (searchResults[i].empty())
                {
                    cout << "No results" << endl;
                }
                else
                {
                    cout << searchResults[i].size() << " results" << endl;
                }
            }
        }

    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
