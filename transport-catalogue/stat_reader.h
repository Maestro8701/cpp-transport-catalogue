// место для вашего кода
#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

#include <iostream>

namespace transport {

void ParseAndPrintStatBus(const Catalogue& catalogue, std::string_view request,
                       std::ostream& output);
void ParseAndPrintStatStop(const Catalogue& catalogue, std::string_view request,
                       std::ostream& output);
    
} // namespace transport
