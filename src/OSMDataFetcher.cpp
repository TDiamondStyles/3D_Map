#include "OSMDataFetcher.h"

#include <httplib.h>

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

std::string OSMDataFetcher::SendHTTPRequest(const std::string& url, const std::string& query)
{
    size_t protocol_end = url.find("://");
    if (protocol_end == std::string::npos) {
        return "ERROR: Invalid URL format";
    }

    size_t host_start = protocol_end + 3;
    size_t path_start = url.find('/', host_start);
    std::string host = url.substr(host_start, path_start - host_start);
    std::string path = (path_start == std::string::npos) ? "/" : url.substr(path_start);

    httplib::Client cli(host.c_str());
    cli.set_connection_timeout(3);
    cli.set_read_timeout(3);
    cli.set_follow_location(true);

    std::string full_path = path + "?data=" + query;

    cli.set_error_logger([](const httplib::Error& err, const httplib::Request* req) {
        std::cerr << "! ";
        if (req) {
            std::cerr << req->method << " " << req->path << " ";
        }
        std::cerr << "failed: " << httplib::to_string(err);
        std::cerr << std::endl;
        });

    auto res = cli.Get(full_path.c_str());

    if (res) {
        if (res->status == 200) {
            return res->body;
        }
        else {
            return "ERROR: HTTP " + std::to_string(res->status);
        }
    }

    return "ERROR: Request failed - no response";
}