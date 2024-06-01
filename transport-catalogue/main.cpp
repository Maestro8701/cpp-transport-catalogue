#include <cstdio>

#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
    TC::TransportCatalogue catalogue;
    MapRenderer renderer;
    RequestHandler request_handler(catalogue, renderer);
    JsonReader json_reader(json::Load(std::cin));
    renderer.SetSettings(json_reader.GetRenderSettings());
    renderer.CreateDocument(request_handler.GetRoutes(), request_handler.GetStops(), request_handler.GetStopToBuses());
    json::Print(json_reader.ApplyCommands(request_handler, catalogue), std::cout);
}
