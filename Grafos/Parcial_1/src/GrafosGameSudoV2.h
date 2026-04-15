#ifndef GRAFOS_GAME_SUDO_V2_H
#define GRAFOS_GAME_SUDO_V2_H

#include "GrafosGameV2.h"

namespace ggsudov2 {

class GrafosGameSudoV2 {
public:
    void run()
    {
        int option = -1;
        while (true)
        {
            printMenu();
            if (!(std::cin >> option))
                break;
            if (option == 0)
                break;
            handleOption(option);
        }
    }

private:
    ggv2::GraphGameV2 game_;

    void printMenu() const
    {
        std::cout << "\nPROGRA-VILLA | GrafosGameSudoV2\n";
        std::cout << "1. Cargar ciudad\n";
        std::cout << "2. Dashboard general\n";
        std::cout << "3. Lista + matriz + mapa ASCII\n";
        std::cout << "4. Buscar ruta BFS y compararla con Dijkstra\n";
        std::cout << "5. Verificar ruta segura DFS\n";
        std::cout << "6. Analizar conectividad\n";
        std::cout << "7. Bloquear una zona roja\n";
        std::cout << "8. Desbloquear una zona\n";
        std::cout << "9. Simular corte critico de una zona\n";
        std::cout << "10. Informe tactico completo\n";
        std::cout << "0. Salir\n";
        std::cout << "Opcion: ";
    }

    void handleOption(int option)
    {
        if (option == 1)
        {
            std::string fileName;
            std::cout << "Archivo: ";
            std::cin >> fileName;
            std::cout << (game_.loadFromFile(fileName) ? "Ciudad cargada.\n" : "No se pudo cargar.\n");
        }
        else if (option == 2)
        {
            showDashboard();
        }
        else if (option == 3)
        {
            game_.showAdjacencyList();
            game_.showAdjacencyMatrix();
            game_.showAsciiMap();
        }
        else if (option == 4)
        {
            compareBfsAndDijkstra();
        }
        else if (option == 5)
        {
            std::string north;
            std::string south;
            std::cout << "Zona norte: ";
            std::cin >> north;
            std::cout << "Zona sur: ";
            std::cin >> south;

            ggv2::SearchResult result = game_.findSafeRouteDFS(north, south);
            game_.printSearchResult("Ruta de rescate DFS", result, "Saltos del camino");
            if (result.found)
                game_.showAsciiMap(result.path);
        }
        else if (option == 6)
        {
            game_.showConnectivityReport();
        }
        else if (option == 7 || option == 8)
        {
            std::string id;
            std::cout << "Nodo: ";
            std::cin >> id;
            const bool ok = option == 7 ? game_.blockNode(id) : game_.unblockNode(id);
            std::cout << (ok ? "Operacion realizada.\n" : "Nodo no encontrado.\n");
        }
        else if (option == 9)
        {
            simulateCriticalCut();
        }
        else if (option == 10)
        {
            showFullReport();
        }
        else
        {
            std::cout << "Opcion invalida.\n";
        }
    }

    void showDashboard() const
    {
        game_.showCitySummary();
        game_.showConnectivityReport();
        game_.showAsciiMap();
    }

    void compareBfsAndDijkstra()
    {
        std::string startId;
        std::string targetId;
        std::cout << "Refugio inicial: ";
        std::cin >> startId;
        std::cout << "Hospital objetivo: ";
        std::cin >> targetId;

        const ggv2::SearchResult bfs = game_.findSupplyRouteBFS(startId, targetId);
        const ggv2::SearchResult dijkstra = game_.findLowestDangerDijkstra(startId, targetId);

        game_.printSearchResult("Resultado BFS", bfs, "Calles");
        game_.printSearchResult("Resultado Dijkstra", dijkstra, "Peligro");

        if (bfs.found)
            game_.showAsciiMap(bfs.path);
        if (dijkstra.found)
            game_.showAsciiMap(dijkstra.path);

        std::cout << "\nAnalisis comparativo\n";
        if (!bfs.found || !dijkstra.found)
        {
            std::cout << "No se pudo comparar ambas rutas porque alguna no existe.\n";
            return;
        }

        std::cout << "BFS minimiza calles; Dijkstra minimiza zombies.\n";
        std::cout << "BFS: " << game_.formatPath(bfs.path) << "\n";
        std::cout << "Dijkstra: " << game_.formatPath(dijkstra.path) << "\n";
        if (bfs.path == dijkstra.path)
            std::cout << "Ambos algoritmos eligieron la misma ruta.\n";
        else
            std::cout << "Las rutas difieren porque optimizan metricas distintas.\n";
    }

    void simulateCriticalCut()
    {
        if (game_.empty())
        {
            std::cout << "Primero carga una ciudad.\n";
            return;
        }

        std::string nodeId;
        std::cout << "Nodo a convertir temporalmente en zona roja: ";
        std::cin >> nodeId;

        const bool blocked = game_.blockNode(nodeId);
        if (!blocked)
        {
            std::cout << "Nodo no encontrado.\n";
            return;
        }

        std::cout << "\nSimulacion aplicada sobre " << nodeId << "\n";
        game_.showConnectivityReport();
        game_.showAsciiMap();
        std::cout << "La zona queda bloqueada hasta que la desbloquees manualmente desde el menu.\n";
    }

    void showFullReport()
    {
        if (game_.empty())
        {
            std::cout << "Primero carga una ciudad.\n";
            return;
        }

        std::cout << "\nINFORME TACTICO COMPLETO\n";
        game_.showCitySummary();
        game_.showAdjacencyList();
        game_.showAdjacencyMatrix();
        game_.showConnectivityReport();
        game_.showAsciiMap();

        const std::vector<std::string> hospitals = game_.hospitalIds();
        if (!game_.nodes().empty() && !hospitals.empty())
        {
            const std::string startId = game_.nodes().front().id;
            const std::string hospitalId = hospitals.front();
            const ggv2::SearchResult bfs = game_.findSupplyRouteBFS(startId, hospitalId);
            const ggv2::SearchResult dijkstra = game_.findLowestDangerDijkstra(startId, hospitalId);
            game_.printSearchResult("Ruta sugerida BFS desde el primer nodo al primer hospital",
                                    bfs,
                                    "Calles");
            game_.printSearchResult("Ruta sugerida Dijkstra desde el primer nodo al primer hospital",
                                    dijkstra,
                                    "Peligro");
        }
        else
        {
            std::cout << "No hay datos suficientes para generar una ruta sugerida automatica.\n";
        }
    }
};

inline void runGrafosGameSudoV2()
{
    GrafosGameSudoV2 app;
    app.run();
}

} // namespace ggsudov2

#endif
