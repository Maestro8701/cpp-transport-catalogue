#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace reader {
    
namespace stat {

std::string_view Trim(std::string_view string);
    
void PrintBusInfo(const transport_catalogue::TransportCatalogue& catalogue, 
                  const std::string& id, std::ostream& output);
    
void PrintStopInfo(const transport_catalogue::TransportCatalogue& catalogue, 
                  const std::string& id, std::ostream& output);
    
void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& catalogue, 
                       std::string_view request, std::ostream& output);

void Read(const transport_catalogue::TransportCatalogue& catalogue, 
          std::istream& input, std::ostream& output);
    
} //namespace stat
    
} //namespace reader
