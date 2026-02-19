#include "OSMDataFetcher.h"
#include "LogUtils.h"

#include <curl/curl.h>
#include <spdlog/spdlog.h>

#include <sstream>

OSMDataFetcher::OSMDataFetcher(const double latMin, const double latMax, const double lonMin, const double lonMax, const std::string& apiUrl)
	: m_LatMin(latMin), m_LatMax(latMax), m_LonMin(lonMin), m_LonMax(lonMax), m_ApiUrl(apiUrl)
{
}

std::string OSMDataFetcher::FetchBuildings()
{
	return SendHTTPRequest(m_ApiUrl, BuildQuery());
}

void OSMDataFetcher::SetBoundingBox(const double latMin, const double latMax, const double lonMin, const double lonMax)
{
	m_LatMin = latMin;
	m_LatMax = latMax;
	m_LonMin = lonMin;
	m_LonMax = lonMax;
}

void OSMDataFetcher::SetApiUrl(const std::string& apiUrl)
{
	m_ApiUrl = apiUrl;
}

std::string OSMDataFetcher::BuildQuery()
{
    std::ostringstream query;

    query << "[out:json][timeout:180];\n"
        << "(\n"
        << "  way[\"building\"]("
        << m_LatMin << "," << m_LonMin << ","
        << m_LatMax << "," << m_LonMax << ");\n"
        << "  node[\"building\"]("
        << m_LatMin << "," << m_LonMin << ","
        << m_LatMax << "," << m_LonMax << ");\n"
        << ");\n"
        << "out body;\n"
        << ">;\n"
        << "out skel qt;";
    
    m_LastRequest = query.str();
    return query.str();
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::string OSMDataFetcher::SendHTTPRequest(const std::string& url, const std::string& query) {
    LOG_INFO_FUNC_ON_ENTER();

    const int maxRetries = 5;
    const int retryDelayMs = 2000;

    CURL* curl = curl_easy_init();
    if (!curl) {
        spdlog::error("Failed to initialize CURL!");
        return std::string();
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, query.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "3D_Map/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 30L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 100L);

    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Accept-Language: en-US,en;q=0.9");
    headers = curl_slist_append(headers, "Connection: keep-alive");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");


    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        spdlog::debug("Attempt {}/{} - sending request to {}", attempt, maxRetries, url);

        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (http_code == 200) {
                spdlog::debug("Request successful on attempt {}", attempt);
                curl_easy_cleanup(curl);
                return response;
            }

            bool shouldRetry = (http_code == 408 ||
                http_code == 429 ||
                http_code == 502 ||
                http_code == 503 ||
                http_code == 504 ||
                http_code >= 500);

            if (shouldRetry && attempt < maxRetries) {
                spdlog::warn("HTTP {} on attempt {}. Retrying in {} ms...",
                    http_code, attempt, retryDelayMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
                continue;
            }

            spdlog::error("HTTP error {} on final attempt: {}", http_code, response);
            curl_easy_cleanup(curl);
            return std::string();

        }
        else {
            std::string errorMsg = curl_easy_strerror(res);

            bool shouldRetry = (res == CURLE_OPERATION_TIMEDOUT ||
                res == CURLE_COULDNT_CONNECT ||
                res == CURLE_SEND_ERROR ||
                res == CURLE_RECV_ERROR ||
                res == CURLE_GOT_NOTHING);

            if (shouldRetry && attempt < maxRetries) {
                spdlog::warn("Connection error on attempt {}: {}. Retrying in {} ms...",
                    attempt, errorMsg, retryDelayMs);
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelayMs));
                continue;
            }

            spdlog::error("CURL connection error on final attempt: {}!", errorMsg);
            curl_easy_cleanup(curl);
            return std::string();
        }
    }

    curl_easy_cleanup(curl);
    spdlog::error("All {} retry attempts failed", maxRetries);
    return std::string();
}