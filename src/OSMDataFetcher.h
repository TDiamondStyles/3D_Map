#pragma once

#include <string>

class OSMDataFetcher {
public:
    OSMDataFetcher(const double latMin, const double latMax, const double lonMin, const double lonMax, const std::string& apiUrl);

    std::string FetchBuildings();

    std::string GetLastResponse() const { return m_LastResponse; }
    std::string GetLastRequest() const { return m_LastRequest; }

    void SetBoundingBox(const double latMin, const double latMax, const double lonMin, const double lonMax);
    void SetApiUrl(const std::string& apiUrl);

private:
    double m_LatMin, m_LatMax, m_LonMin, m_LonMax;
    std::string m_LastRequest;
    std::string m_LastResponse;
    std::string m_ApiUrl;

    std::string BuildQuery();
    std::string SendHTTPRequest(const std::string& url, const std::string& query);
};