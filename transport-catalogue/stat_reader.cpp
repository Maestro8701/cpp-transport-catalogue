#include "stat_reader.h"

#include <iomanip>

namespace reader {
    
namespace stat {

/**
* Удаляет пробелы в начале и конце строки
*/
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}
    
void PrintBusInfo(const transport_catalogue::TransportCatalogue& catalogue, 
                  const std::string& id, std::ostream& output) {
    
    output << "Bus " << id << ": ";
    const transport_catalogue::Bus* bus = catalogue.FindBus(id);
    if (!bus){
        output << "not found" << std::endl;
    }
    else {
        transport_catalogue::BusInfo info = catalogue.GetBusInfo(bus);
        output << info.stops << " stops on route, " <<
            info.unique_stops << " unique stops, " <<
            info.route_length << " route length, " <<
            info.curvature << " curvature" << std::endl;
    }
}
    
void PrintStopInfo(const transport_catalogue::TransportCatalogue& catalogue, 
                  const std::string& id, std::ostream& output) {
    
    output << "Stop " << id << ": ";
    const transport_catalogue::Stop* stop = catalogue.FindStop(id);
    if (!stop) {
        output << "not found" << std::endl;
    }
    else{
        std::set<std::string> info = catalogue.GetStopInfo(stop);
        if(info.empty()) {
            output << "no buses" << std::endl;
        }
        else {
            output << "buses";
            for(const auto& bus : info){
                output << " " << bus;
            }
            output << std::endl;
        }
    }
}

void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& catalogue, 
                       std::string_view request, std::ostream& output) {
    
    request = Trim(request);
    auto space_pos = request.find(' ');
    auto not_space = request.find_first_not_of(' ', space_pos);
    
    std::string command = std::string{request.substr(0, space_pos)};
    std::string id = std::string{request.substr(not_space)};
    
    if(command == "Bus"){
        PrintBusInfo(catalogue, id, output);
    }
    else if(command == "Stop"){
        PrintStopInfo(catalogue, id, output);
    }
}
    
void Read(const transport_catalogue::TransportCatalogue& catalogue, 
          std::istream& input, std::ostream& output) {
    
    int stat_request_count;
    input >> stat_request_count >> std::ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        getline(input, line);
        ParseAndPrintStat(catalogue, line, output);
    }
}
    
} //namespace stat
    
} //namespace reader
