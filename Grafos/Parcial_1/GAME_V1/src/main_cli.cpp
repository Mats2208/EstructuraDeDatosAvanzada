#include "GrafosGameV2Base.h"

#include <iostream>

int main()
{
    ggv2::GraphGameV2 game;
    const std::string fileName = "GAME_V1/data/progra_villa_v2.txt";

    if (!game.loadFromFile(fileName))
    {
        std::cerr << "No se pudo cargar " << fileName << "\n";
        return 1;
    }

    game.showCitySummary();
    game.showAdjacencyList();
    game.showConnectivityReport();

    const ggv2::SearchResult bfs = game.findSupplyRouteBFS("R", "H");
    const ggv2::SearchResult dfs = game.findSafeRouteDFS("N", "Z");
    const ggv2::SearchResult dijkstra = game.findLowestDangerDijkstra("R", "H");

    game.printSearchResult("Demo BFS", bfs, "Calles");
    game.printSearchResult("Demo DFS", dfs, "Saltos");
    game.printSearchResult("Demo Dijkstra", dijkstra, "Peligro");
    game.showAsciiMap(dijkstra.path);
    return 0;
}
