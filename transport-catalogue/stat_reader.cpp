// место для вашего кода
#include "stat_reader.h"
#include <iomanip>
#include <utility>

namespace transport {
namespace detail {
    
std::pair<std::string, std::string> GetCommandRequest(std::string_view request) {
    auto space_pos = request.find(' ');
    auto not_space = request.find_first_not_of(' ', space_pos);
    std::string key = std::string(request.substr(0, space_pos));
    std::string value = std::string(request.substr(not_space));
    return {key,value};
}

} //namespace detail  
    
void ParseAndPrintStatBus(const Catalogue& catalogue, std::string_view request,
                       std::ostream& output) {

    std::string key = transport::detail::GetCommandRequest(request).first;
    std::string value = transport::detail::GetCommandRequest(request).second;
    if (key == "Bus") {

    if (catalogue.FindRoute(value)) {
            output << "Bus " << value << ": " 
                << catalogue.FindInfo(value).stops_count << " stops on route, "
                << catalogue.FindInfo(value).uniq_stops_count << " unique stops, " << std::setprecision(6)
                << catalogue.FindInfo(value).route_length << " route length\n";
        }
        else {
            output << "Bus " << value << ": not found\n";
        }
    }
}
    
void ParseAndPrintStatStop(const Catalogue& catalogue, std::string_view request,
                       std::ostream& output) {

    std::string key = transport::detail::GetCommandRequest(request).first;
    std::string value = transport::detail::GetCommandRequest(request).second;
    
    if (key == "Stop") {
        if (catalogue.FindStop(value)) {
                output << "Stop " << value << ": ";
                std::set<std::string_view> buses = catalogue.FindBusesOnStop(value);
                if (!buses.empty()) {
                    output << "buses ";
                    for (const auto& bus : buses) {
                        output << bus << " ";
                    }
                    output << "\n";
                }
                else {
                    output << "no buses\n";
                }
            }
            else {
                output << "Stop " << value << ": not found\n";
            }
        }
    }   
} // namespace transport
