#include "ConverterJson.hpp"
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




using namespace std;
using json = nlohmann::json;




    json ConverterJSON::loadJSON(const string& filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            throw runtime_error("config file is missing");
        }
        json j;
        file >> j;
        return j;
    }

    // Mетод для создания конфигурационного файла
    void ConverterJSON::createConfigFile()
    {
        json config;

        // Создаем конфигурацию
        config["config"]["name"] = "SearchEngine";
        config["config"]["version"] = "0.1";
        config["config"]["max_responses"] = 5;

        // Запрашиваем количество файлов
        int fileCount;
        cout << "Enter number of files to analyze: ";
        cin >> fileCount;
        cin.ignore();

        // Запрашиваем пути к файлам
        vector<string> files;
        for (int i = 0; i < fileCount; i++)
        {
            string filePath;
            cout << "Enter path for file " << i + 1 << ": ";
            getline(cin, filePath);
            files.push_back(filePath);
        }


        config["files"] = files;

        // Записываем в файл
        ofstream file(configFile);
        if (file.is_open())
        {
            file << config.dump(2);
            cout << "Config file created successfully with " << fileCount << " files." << endl;
        }
        else
        {
            cerr << "Error: Cannot create config.json" << endl;
            throw runtime_error("Failed to create config file");
        }
    }

    // Mетод для создания файла запросов
    void ConverterJSON::createRequestsFile()
    {
        json requests;

        // Запрашиваем количество запросов
        int requestCount;
        cout << "Enter number of search queries: ";
        cin >> requestCount;
        cin.ignore();

        // Запрашиваем сами запросы
        vector<string> requestList;
        for (int i = 0; i < requestCount; i++)
        {
            string query;
            cout << "Enter query " << i + 1 << ": ";
            getline(cin, query);
            requestList.push_back(query);
        }

        // Добавляем запросы в JSON
        requests["requests"] = requestList;

        // Записываем в файл
        ofstream file(requestsFile);
        if (file.is_open())
        {
            file << requests.dump(2);
            cout << "Requests file created successfully with " << requestCount << " queries." << endl;
        }
        else
        {
            cerr << "Error: Cannot create requests.json" << endl;
            throw runtime_error("Failed to create requests file");
        }
    }


        vector<string> ConverterJSON::GetTextDocuments()
    {
        try
        {
            json config = loadJSON(configFile);

            if (!config.contains("config"))
            {
                throw runtime_error("config file is empty");
            }

            if (config["config"].contains("name") && config["config"].contains("version"))
            {
                string name = config["config"]["name"];
                string version = config["config"]["version"];
                cout << "Starting " << name << " version " << version << endl;

                const string EXPECTED_VERSION = "0.1";
                if (version != EXPECTED_VERSION)
                {
                    cerr << "Warning: config.json has incorrect file version" << endl;
                }
            }

            if (!config.contains("files") || !config["files"].is_array())
            {
                return {};
            }

            vector<string> documents;
            for (const auto& filepath : config["files"])
            {
                string path = filepath.get<string>();
                ifstream file(path);
                if (file.is_open())
                {
                    stringstream buffer;
                    buffer << file.rdbuf();
                    documents.push_back(buffer.str());
                }
                else
                {
                    cerr << "Warning: File not found - " << path << endl;
                    documents.push_back("");
                }
            }

            return documents;
        }
        catch (const exception& e)
        {
            cerr << "Error loading config: " << e.what() << endl;
            throw;
        }
    }

    int ConverterJSON::GetResponsesLimit()
    {
        try
        {
            json config = loadJSON(configFile);
            if (config.contains("config") && config["config"].contains("max_responses"))
            {
                return config["config"]["max_responses"];
            }
        }
        catch (...)
        {
            // Игнорируем ошибки, возвращаем значение по умолчанию
        }
        return 5;
    }

    vector<string>ConverterJSON::GetRequests()
    {
        try
        {
            json requests = loadJSON(requestsFile);
            if (requests.contains("requests") && requests["requests"].is_array())
            {
                vector<string> result;
                for (const auto& req : requests["requests"])
                {
                    result.push_back(req.get<string>());
                }
                return result;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error loading requests: " << e.what() << endl;
        }
        return {};
    }

    void ConverterJSON::putAnswers(const vector<vector<RelativeIndex>>& answers)
    {
        json output;
        output["answers"] = json::object();

        for (size_t i = 0; i < answers.size(); i++)
        {
            string requestId = "request" + string(3 - to_string(i + 1).length(), '0') + to_string(i + 1);

            if (answers[i].empty())
            {
                output["answers"][requestId] = { {"result", "false"} };
            }
            else
            {
                json relevance = json::array();
                for (const auto& rel : answers[i])
                {
                    relevance.push_back({
                        {"docid", rel.doc_id},
                        {"rank", rel.rank}
                        });
                }
                output["answers"][requestId] = {
                    {"result", "true"},
                    {"relevance", relevance}
                };
            }
        }

        ofstream file(answersFile);
        if (file.is_open())
        {
            file << output.dump(2);
            cout << "Results saved to " << answersFile << endl;
        }
        else
        {
            cerr << "Error: Cannot create answers.json" << endl;
        }
    };
