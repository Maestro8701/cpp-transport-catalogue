
#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    transport_catalogue::TransportCatalogue catalogue;

    reader::input::Read(catalogue, cin);
    reader::stat::Read(catalogue, cin, cout);
}
