// Вставьте сюда решение из предыдущего спринта
#include <iostream>
#include <fstream>
#include <sstream>

#include "json_reader.h"

using namespace std;
using namespace json;

int main() {
    TransportCatalogue catalogue;
    renderer::MapRenderer renderer;
    JsonReader reader;


    istream& input = cin;
    reader.LoadJson(input);

        transport_router::TransportRouter router(reader.AddRoutingSetting(), catalogue);
    RequestHandler handler(catalogue, renderer, router);
    reader.LoadHandler(handler);
    reader.AddStopsDataToCatalogue();
    reader.AddBusesDataToCatalogue();
    //reader.AddRoutingSetting();
    reader.ParseRenderSettings(renderer);

    ostream& out = cout;
    reader.ParseAndPrintStat(handler, out);   
}
