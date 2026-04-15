#include "GrafosGameV2Base.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool fileExists(const std::filesystem::path& path)
{
    std::ifstream file(path);
    return file.good();
}

std::string resolveExistingPath(const std::vector<std::filesystem::path>& candidates)
{
    for (const std::filesystem::path& path : candidates)
    {
        if (fileExists(path))
            return path.string();
    }
    return "";
}

std::string resolveMapPath(const char* executablePath)
{
    std::vector<std::filesystem::path> candidates = {
        "progra_villa_v2.txt",
        "../data/progra_villa_v2.txt",
        "data/progra_villa_v2.txt",
        "Parcial_1/game_sfml/progra_villa_v2.txt",
        "Parcial_1/data/progra_villa_v2.txt"
    };

    if (executablePath != nullptr && executablePath[0] != '\0')
    {
        const std::filesystem::path executableDir =
            std::filesystem::absolute(executablePath).parent_path();

        candidates.push_back(executableDir / "../progra_villa_v2.txt");
        candidates.push_back(executableDir / "../../data/progra_villa_v2.txt");
        candidates.push_back(executableDir / "../../../data/progra_villa_v2.txt");
        candidates.push_back(executableDir / "../data/progra_villa_v2.txt");
    }

    return resolveExistingPath(candidates);
}

} // namespace

int main(int argc, char* argv[])
{
    ggv2::GraphGameV2 game;
    const std::string fileName = resolveMapPath(argc > 0 ? argv[0] : nullptr);

    if (fileName.empty())
    {
        std::cerr << "No se encontro el mapa base progra_villa_v2.txt\n";
        return 1;
    }

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
