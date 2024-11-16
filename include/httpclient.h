#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using HttpHeaders = std::unordered_map<std::string,std::string>;

class HttpResponse {
friend class HttpClient;
public:
    HttpResponse();
    HttpResponse(const std::string &responseHeader);
    int getStatusCode() const;
    const HttpHeaders &getHeaders() const;
    const std::string &getContent() const;
    static bool tryParse(const std::string &header, HttpResponse &request);
private:
    int statusCode;
    HttpHeaders headers;
    std::string content;
    static std::vector<std::string> stringSplit(const std::string &str, char separator);
    static std::string stringTrim(const std::string &str);
    static void stringReplace(std::string &str, const std::string &target, const std::string &replacement);
};

std::ostream& operator<<(std::ostream& os, const HttpResponse& response);

typedef void CURL;

class HttpClient {

public:
    HttpClient();
    ~HttpClient();
    int get(const std::string& url, HttpResponse &response, HttpHeaders *headers = nullptr);
    int options(const std::string& url, HttpResponse &response, HttpHeaders *headers = nullptr);
    void setVerbose(bool v);
    static size_t onHeader(char* buffer, size_t size, size_t nitems, HttpClient* client);
    static size_t onData(void* contents, size_t size, size_t nmemb, HttpClient* client);
private:
    CURL* curl;
    bool verbose;
    std::string headerBuffer;
    std::string contentBuffer;
};

#endif