#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace reader {
    
namespace input {

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

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

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string> Split(std::string_view string, char delim) {
    std::vector<std::string> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(std::string{substr});
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] transport_catalogue::TransportCatalogue& catalogue) const {
    for(const auto& command : commands_){
        if(command.command == "Stop"){
            geo::Coordinates coordinates = ParseCoordinates(Trim(command.description));
            catalogue.AddStop(command.id, coordinates);
        }
    }
    for(const auto& command : commands_){
        if(command.command == "Bus"){
            std::vector<std::string> route = ParseRoute(Trim(command.description));
            catalogue.AddBus(std::string{Trim(command.id)}, route);
        }
    }
}

std::unordered_map<std::string, unsigned int> ParceDistances(std::string_view str) {
    std::unordered_map<std::string, unsigned int> result;
    auto comma = str.find(',');
    if (comma == str.npos) {
        return {};
    }
    str = str.substr(comma + 1);
    comma = str.find(',');
    if (comma == str.npos) {
        return {};
    }
    str = str.substr(comma + 1);
    std::vector<std::string> stops = Split(Trim(str), ',');
    for (auto stop : stops) {
        stop = Trim(stop);
        unsigned int distance = std::stoi(stop.substr(0, stop.find('m')));
        stop = stop.substr(stop.find(' ') + 1);
        std::string reach_stop = stop.substr(stop.find(' ') + 1);
        result.insert({ reach_stop, distance });
    }

    return result;
}

void InputReader::AddDistances([[maybe_unused]] transport_catalogue::TransportCatalogue& catalogue) const {
    for (const auto& command : commands_) {
        if (command.command == "Stop") {
            std::unordered_map<std::string, unsigned int> distances = ParceDistances(command.description);
            for ( const auto& [stop,distance] : distances) {
                catalogue.AddDistances(command.id, stop, distance);
            }
        }
    }
}
    
void Read(transport_catalogue::TransportCatalogue& catalogue, std::istream& input){
    int base_request_count;
    input >> base_request_count >> std::ws;
    InputReader reader;
    for (int i = 0; i < base_request_count; ++i) {
        std::string line;
        getline(input, line);
        reader.ParseLine(line);
    }
    reader.ApplyCommands(catalogue);
    reader.AddDistances(catalogue);
}
    
} //namespace input
    
} //namespace reader
