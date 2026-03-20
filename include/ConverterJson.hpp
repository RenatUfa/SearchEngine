#pragma once 

#include <sstream>
#include <thread>
#include <nlohmann/json.hpp>
#include "InvertedIndex.hpp"
#include "SearchServer.hpp"


using namespace std;
using json = nlohmann::json;

class ConverterJSON
{
  private:
    const string configFile = "config.json";
    const string requestsFile = "requests.json";
    const string answersFile = "answers.json";

    json loadJSON(const string& filename);
    

    // Mетод для создания конфигурационного файла
    void createConfigFile();
   

    // Mетод для создания файла запросов
    void createRequestsFile();
    

   public:
    ConverterJSON()
    {
        // При создании объекта запрашиваем данные и создаем файлы

        createConfigFile();
       
        createRequestsFile();

    }

    vector<string> GetTextDocuments();
    

    int GetResponsesLimit();
    

    vector<string> GetRequests();
    

    void putAnswers(const vector<vector<RelativeIndex>>& answers);
    
};