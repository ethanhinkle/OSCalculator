#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <string>

typedef void CURL;

class HttpClient {

public:
    HttpClient();
    ~HttpClient();
    int get(const std::string& url);
    void setVerbose(bool v);
    std::string getResponse() const;
    static size_t onData(void* contents, size_t size, size_t nmemb, HttpClient* client);

private:
    CURL* curl;
    std::string readBuffer;
    bool verbose;

};


#endif