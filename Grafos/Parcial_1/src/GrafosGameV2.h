#ifndef GRAFOS_GAME_V2_H
#define GRAFOS_GAME_V2_H

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace ggv2 {

struct Edge {
    int to = -1;
    int danger = 0;
};

struct Node {
    std::string id;
    std::string kind;
    bool redZone = false;
    int x = -1;
    int y = -1;
};

struct SearchResult {
    bool found = false;
    int cost = 0;
    std::vector<int> path;
    std::string message;
};

class GraphGameV2 {
public:
    bool loadFromFile(const std::string& fileName)
    {
        std::ifstream file(fileName.c_str());
        if (!file.is_open())
            return false;

        clear();

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream iss(line);
            std::string command;
            iss >> command;
            if (command.empty())
                continue;

            if (command == "TIPO" || command == "TYPE")
            {
                std::string token;
                iss >> token;
                directed_ = normalizeGraphType(token);
            }
            else if (command == "N" || command == "NODE")
            {
                std::string id;
                std::string kind;
                int blocked = 0;
                int x = -1;
                int y = -1;

                iss >> id >> kind >> blocked;
                if (iss >> x >> y)
                    addNode(id, kind, blocked != 0, x, y);
                else
                    addNode(id, kind, blocked != 0, -1, -1);
            }
            else if (command == "A" || command == "EDGE")
            {
                std::string from;
                std::string to;
                int danger = 0;
                int bidirectional = 0;
                iss >> from >> to >> danger >> bidirectional;
                addEdge(from, to, danger, bidirectional != 0);
            }
        }

        return !nodes_.empty();
    }

    void clear()
    {
        directed_ = false;
        nodes_.clear();
        adjacency_.clear();
        indexById_.clear();
    }

    bool empty() const
    {
        return nodes_.empty();
    }

    bool addNode(const std::string& id, const std::string& kind, bool redZone, int x, int y)
    {
        if (id.empty() || indexById_.count(id))
            return false;

        Node node;
        node.id = id;
        node.kind = kind;
        node.redZone = redZone;
        node.x = x;
        node.y = y;

        indexById_[id] = static_cast<int>(nodes_.size());
        nodes_.push_back(node);
        adjacency_.push_back({});
        return true;
    }

    bool addEdge(const std::string& from, const std::string& to, int danger, bool bidirectional)
    {
        const int u = getIndex(from);
        const int v = getIndex(to);
        if (u == -1 || v == -1 || danger < 0)
            return false;

        connectOneWay(u, v, danger);
        if (!directed_ || bidirectional)
            connectOneWay(v, u, danger);
        return true;
    }

    bool blockNode(const std::string& id)
    {
        const int idx = getIndex(id);
        if (idx == -1)
            return false;
        nodes_[idx].redZone = true;
        return true;
    }

    bool unblockNode(const std::string& id)
    {
        const int idx = getIndex(id);
        if (idx == -1)
            return false;
        nodes_[idx].redZone = false;
        return true;
    }

    void showAdjacencyList(std::ostream& out = std::cout) const
    {
        if (empty())
        {
            out << "Mapa vacio\n";
            return;
        }

        out << "\nPROGRA-VILLA V2 | Lista de adyacencia\n";
        out << "ID   Tipo           Estado      Coordenadas   Conexiones\n";
        out << "-------------------------------------------------------------\n";

        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            const Node& node = nodes_[i];
            out << std::left << std::setw(4) << node.id
                << std::setw(15) << node.kind
                << std::setw(12) << (node.redZone ? "ZONA ROJA" : "SEGURO");

            std::ostringstream coord;
            if (node.x >= 0 && node.y >= 0)
                coord << "(" << node.x << "," << node.y << ")";
            else
                coord << "auto";
            out << std::setw(14) << coord.str();

            if (adjacency_[i].empty())
            {
                out << "sin calles";
            }
            else
            {
                for (size_t j = 0; j < adjacency_[i].size(); ++j)
                {
                    const Edge& edge = adjacency_[i][j];
                    out << nodes_[edge.to].id << "[" << edge.danger << "]";
                    if (j + 1 < adjacency_[i].size())
                        out << " ";
                }
            }
            out << "\n";
        }
    }

    void showAdjacencyMatrix(std::ostream& out = std::cout) const
    {
        if (empty())
        {
            out << "Mapa vacio\n";
            return;
        }

        out << "\nPROGRA-VILLA V2 | Matriz de adyacencia\n";
        out << std::setw(6) << " ";
        for (const Node& node : nodes_)
            out << std::setw(6) << node.id;
        out << "\n";

        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            out << std::setw(6) << nodes_[i].id;
            for (size_t j = 0; j < nodes_.size(); ++j)
            {
                out << std::setw(6) << edgeWeight(static_cast<int>(i), static_cast<int>(j));
            }
            out << "\n";
        }
    }

    void showAsciiMap(const std::vector<int>& highlightPath = {}, std::ostream& out = std::cout) const
    {
        if (empty())
        {
            out << "Mapa vacio\n";
            return;
        }

        const int width = 72;
        const int height = 22;
        std::vector<std::string> canvas(height, std::string(width, ' '));
        std::set<std::pair<int, int> > highlightedSegments;

        for (size_t i = 1; i < highlightPath.size(); ++i)
        {
            highlightedSegments.insert(normalizeSegment(highlightPath[i - 1], highlightPath[i]));
        }

        std::vector<std::pair<int, int> > positions = buildLayout(width, height);

        for (size_t u = 0; u < adjacency_.size(); ++u)
        {
            for (const Edge& edge : adjacency_[u])
            {
                if (!directed_ && static_cast<int>(u) > edge.to)
                    continue;

                const std::pair<int, int> segment =
                    normalizeSegment(static_cast<int>(u), edge.to);
                const char brush =
                    highlightedSegments.count(segment) ? '*' : '.';
                drawLine(canvas,
                         positions[u].first,
                         positions[u].second,
                         positions[edge.to].first,
                         positions[edge.to].second,
                         brush);
            }
        }

        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            int x = positions[i].first;
            int y = positions[i].second;
            if (y >= 0 && y < height && x >= 0 && x < width)
                canvas[y][x] = nodes_[i].redZone ? 'X' : 'O';

            const std::string label = "[" + nodes_[i].id + "]";
            for (size_t k = 0; k < label.size() && x + 2 + static_cast<int>(k) < width; ++k)
            {
                if (x + 2 + static_cast<int>(k) >= 0 && y >= 0 && y < height)
                    canvas[y][x + 2 + static_cast<int>(k)] = label[k];
            }
        }

        out << "\nPROGRA-VILLA V2 | Mapa ASCII\n";
        out << "Leyenda: O seguro | X zona roja | * ruta resaltada\n";
        for (const std::string& row : canvas)
            out << row << "\n";
    }

    SearchResult findSupplyRouteBFS(const std::string& startId, const std::string& targetId) const
    {
        SearchResult result;
        const int start = getIndex(startId);
        const int target = getIndex(targetId);

        if (!validateSearchEndpoints(start, target, result))
            return result;

        std::vector<int> prev(nodes_.size(), -1);
        std::vector<int> distance(nodes_.size(), -1);
        std::vector<bool> visited(nodes_.size(), false);
        std::queue<int> pending;

        pending.push(start);
        visited[start] = true;
        distance[start] = 0;

        while (!pending.empty())
        {
            int u = pending.front();
            pending.pop();

            if (u == target)
                break;

            for (const Edge& edge : adjacency_[u])
            {
                if (nodes_[edge.to].redZone || visited[edge.to])
                    continue;
                visited[edge.to] = true;
                prev[edge.to] = u;
                distance[edge.to] = distance[u] + 1;
                pending.push(edge.to);
            }
        }

        if (!visited[target])
        {
            result.message = "No existe una ruta BFS evitando zonas rojas.";
            return result;
        }

        result.found = true;
        result.cost = distance[target];
        result.path = rebuildPath(prev, target);
        result.message = "Ruta BFS encontrada.";
        return result;
    }

    SearchResult findSafeRouteDFS(const std::string& startId, const std::string& targetId) const
    {
        SearchResult result;
        const int start = getIndex(startId);
        const int target = getIndex(targetId);

        if (!validateSearchEndpoints(start, target, result))
            return result;

        std::vector<bool> visited(nodes_.size(), false);
        std::vector<int> path;

        if (!dfsPath(start, target, visited, path))
        {
            result.message = "No existe una ruta segura por DFS.";
            return result;
        }

        result.found = true;
        result.path = path;
        result.cost = static_cast<int>(path.size()) - 1;
        result.message = "Ruta segura DFS encontrada.";
        return result;
    }

    SearchResult findLowestDangerDijkstra(const std::string& startId, const std::string& targetId) const
    {
        SearchResult result;
        const int start = getIndex(startId);
        const int target = getIndex(targetId);

        if (!validateSearchEndpoints(start, target, result))
            return result;

        const int inf = std::numeric_limits<int>::max() / 4;
        std::vector<int> dist(nodes_.size(), inf);
        std::vector<int> prev(nodes_.size(), -1);
        std::vector<bool> used(nodes_.size(), false);

        dist[start] = 0;

        for (size_t iter = 0; iter < nodes_.size(); ++iter)
        {
            int u = -1;
            for (size_t i = 0; i < nodes_.size(); ++i)
            {
                if (nodes_[i].redZone || used[i])
                    continue;
                if (u == -1 || dist[i] < dist[u])
                    u = static_cast<int>(i);
            }

            if (u == -1 || dist[u] == inf)
                break;

            used[u] = true;
            for (const Edge& edge : adjacency_[u])
            {
                if (nodes_[edge.to].redZone)
                    continue;
                if (dist[u] + edge.danger < dist[edge.to])
                {
                    dist[edge.to] = dist[u] + edge.danger;
                    prev[edge.to] = u;
                }
            }
        }

        if (dist[target] == inf)
        {
            result.message = "No existe una ruta de menor peligro.";
            return result;
        }

        result.found = true;
        result.cost = dist[target];
        result.path = rebuildPath(prev, target);
        result.message = "Ruta Dijkstra encontrada.";
        return result;
    }

    int connectedComponents(bool ignoreRedZones = true) const
    {
        if (empty())
            return 0;

        std::vector<bool> visited(nodes_.size(), false);
        int components = 0;

        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            if (visited[i] || (ignoreRedZones && nodes_[i].redZone))
                continue;

            ++components;
            std::queue<int> pending;
            pending.push(static_cast<int>(i));
            visited[i] = true;

            while (!pending.empty())
            {
                int u = pending.front();
                pending.pop();
                for (const Edge& edge : adjacency_[u])
                {
                    if (visited[edge.to] || (ignoreRedZones && nodes_[edge.to].redZone))
                        continue;
                    visited[edge.to] = true;
                    pending.push(edge.to);
                }
            }
        }

        return components;
    }

    std::vector<std::vector<std::string> > listComponents(bool ignoreRedZones = true) const
    {
        std::vector<std::vector<std::string> > components;
        std::vector<bool> visited(nodes_.size(), false);

        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            if (visited[i] || (ignoreRedZones && nodes_[i].redZone))
                continue;

            std::vector<std::string> component;
            std::queue<int> pending;
            pending.push(static_cast<int>(i));
            visited[i] = true;

            while (!pending.empty())
            {
                int u = pending.front();
                pending.pop();
                component.push_back(nodes_[u].id);
                for (const Edge& edge : adjacency_[u])
                {
                    if (visited[edge.to] || (ignoreRedZones && nodes_[edge.to].redZone))
                        continue;
                    visited[edge.to] = true;
                    pending.push(edge.to);
                }
            }

            std::sort(component.begin(), component.end());
            components.push_back(component);
        }

        return components;
    }

    void showConnectivityReport(std::ostream& out = std::cout) const
    {
        if (empty())
        {
            out << "Mapa vacio\n";
            return;
        }

        const std::vector<std::vector<std::string> > components = listComponents(true);
        out << "\nPROGRA-VILLA V2 | Conectividad\n";
        out << "Componentes activas: " << components.size() << "\n";
        if (components.size() <= 1)
            out << "La ciudad sigue conectada.\n";
        else
            out << "Existen refugios o zonas utiles aisladas.\n";

        for (size_t i = 0; i < components.size(); ++i)
        {
            out << "Componente " << (i + 1) << ": ";
            for (size_t j = 0; j < components[i].size(); ++j)
            {
                out << components[i][j];
                if (j + 1 < components[i].size())
                    out << ", ";
            }
            out << "\n";
        }
    }

    void showCitySummary(std::ostream& out = std::cout) const
    {
        out << "\nPROGRA-VILLA V2 | Resumen\n";
        out << "Tipo de grafo: " << (directed_ ? "dirigido" : "no dirigido") << "\n";
        out << "Nodos: " << nodes_.size() << "\n";
        out << "Calles registradas: " << edgeCount() << "\n";
        out << "Zonas rojas: " << countRedZones() << "\n";
        out << "Hospitales: " << countByKind("HOSPITAL") << "\n";
        out << "Supermercados: " << countByKind("SUPERMERCADO") << "\n";
        out << "Farmacias: " << countByKind("FARMACIA") << "\n";
        out << "Estaciones de policia: " << countByKind("POLICIA") << "\n";
    }

    void printSearchResult(const std::string& title,
                           const SearchResult& result,
                           const std::string& metricLabel,
                           std::ostream& out = std::cout) const
    {
        out << "\n" << title << "\n";
        if (!result.found)
        {
            out << result.message << "\n";
            return;
        }

        out << "Ruta: " << formatPath(result.path) << "\n";
        out << metricLabel << ": " << result.cost << "\n";
    }

    std::string formatPath(const std::vector<int>& path) const
    {
        if (path.empty())
            return "(sin ruta)";

        std::ostringstream out;
        for (size_t i = 0; i < path.size(); ++i)
        {
            out << nodes_[path[i]].id;
            if (i + 1 < path.size())
                out << " -> ";
        }
        return out.str();
    }

    std::vector<std::string> hospitalIds() const
    {
        std::vector<std::string> ids;
        for (const Node& node : nodes_)
        {
            if (normalize(node.kind) == "HOSPITAL")
                ids.push_back(node.id);
        }
        return ids;
    }

    const std::vector<Node>& nodes() const
    {
        return nodes_;
    }

    const std::vector<std::vector<Edge> >& adjacency() const
    {
        return adjacency_;
    }

    int nodeIndex(const std::string& id) const
    {
        return getIndex(id);
    }

private:
    bool directed_ = false;
    std::vector<Node> nodes_;
    std::vector<std::vector<Edge> > adjacency_;
    std::map<std::string, int> indexById_;

    static std::string normalize(const std::string& value)
    {
        std::string result = value;
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
            if (c == '-')
                return '_';
            return static_cast<char>(std::toupper(c));
        });
        return result;
    }

    static bool normalizeGraphType(const std::string& token)
    {
        const std::string t = normalize(token);
        return t == "1" || t == "DIRIGIDO" || t == "DIRECTED";
    }

    int getIndex(const std::string& id) const
    {
        std::map<std::string, int>::const_iterator it = indexById_.find(id);
        if (it == indexById_.end())
            return -1;
        return it->second;
    }

    void connectOneWay(int u, int v, int danger)
    {
        for (Edge& edge : adjacency_[u])
        {
            if (edge.to == v)
            {
                edge.danger = danger;
                return;
            }
        }
        Edge edge;
        edge.to = v;
        edge.danger = danger;
        adjacency_[u].push_back(edge);
    }

    int edgeWeight(int u, int v) const
    {
        for (const Edge& edge : adjacency_[u])
        {
            if (edge.to == v)
                return edge.danger;
        }
        return 0;
    }

    int edgeCount() const
    {
        int count = 0;
        for (const std::vector<Edge>& edges : adjacency_)
            count += static_cast<int>(edges.size());
        if (!directed_)
            count /= 2;
        return count;
    }

    int countRedZones() const
    {
        int count = 0;
        for (const Node& node : nodes_)
        {
            if (node.redZone)
                ++count;
        }
        return count;
    }

    int countByKind(const std::string& expected) const
    {
        int count = 0;
        for (const Node& node : nodes_)
        {
            if (normalize(node.kind) == expected)
                ++count;
        }
        return count;
    }

    bool validateSearchEndpoints(int start, int target, SearchResult& result) const
    {
        if (empty())
        {
            result.message = "Mapa vacio.";
            return false;
        }
        if (start == -1 || target == -1)
        {
            result.message = "Nodo de origen o destino invalido.";
            return false;
        }
        if (nodes_[start].redZone || nodes_[target].redZone)
        {
            result.message = "No se permite iniciar o terminar en zona roja.";
            return false;
        }
        return true;
    }

    std::vector<int> rebuildPath(const std::vector<int>& prev, int target) const
    {
        std::vector<int> path;
        for (int current = target; current != -1; current = prev[current])
            path.push_back(current);
        std::reverse(path.begin(), path.end());
        return path;
    }

    bool dfsPath(int current,
                 int target,
                 std::vector<bool>& visited,
                 std::vector<int>& path) const
    {
        if (nodes_[current].redZone || visited[current])
            return false;

        visited[current] = true;
        path.push_back(current);

        if (current == target)
            return true;

        for (const Edge& edge : adjacency_[current])
        {
            if (dfsPath(edge.to, target, visited, path))
                return true;
        }

        path.pop_back();
        return false;
    }

    static std::pair<int, int> normalizeSegment(int a, int b)
    {
        if (a > b)
            std::swap(a, b);
        return std::make_pair(a, b);
    }

    std::vector<std::pair<int, int> > buildLayout(int width, int height) const
    {
        bool allPlaced = true;
        int minX = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int minY = std::numeric_limits<int>::max();
        int maxY = std::numeric_limits<int>::min();

        for (const Node& node : nodes_)
        {
            if (node.x < 0 || node.y < 0)
            {
                allPlaced = false;
                break;
            }
            minX = std::min(minX, node.x);
            maxX = std::max(maxX, node.x);
            minY = std::min(minY, node.y);
            maxY = std::max(maxY, node.y);
        }

        std::vector<std::pair<int, int> > positions(nodes_.size());
        if (allPlaced)
        {
            const int spanX = std::max(1, maxX - minX);
            const int spanY = std::max(1, maxY - minY);
            for (size_t i = 0; i < nodes_.size(); ++i)
            {
                const int x = 2 + ((nodes_[i].x - minX) * (width - 8)) / spanX;
                const int y = 1 + ((nodes_[i].y - minY) * (height - 3)) / spanY;
                positions[i] = std::make_pair(x, y);
            }
            return positions;
        }

        const int cols = std::max(2, static_cast<int>(std::sqrt(static_cast<double>(nodes_.size()))) + 1);
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            const int row = static_cast<int>(i) / cols;
            const int col = static_cast<int>(i) % cols;
            const int x = 4 + col * 18;
            const int y = 2 + row * 5;
            positions[i] = std::make_pair(std::min(width - 6, x), std::min(height - 2, y));
        }
        return positions;
    }

    static void drawLine(std::vector<std::string>& canvas,
                         int x1,
                         int y1,
                         int x2,
                         int y2,
                         char brush)
    {
        const int dx = x2 - x1;
        const int dy = y2 - y1;
        const int steps = std::max(std::abs(dx), std::abs(dy));
        if (steps == 0)
            return;

        for (int i = 0; i <= steps; ++i)
        {
            const int x = x1 + dx * i / steps;
            const int y = y1 + dy * i / steps;
            if (y >= 0 && y < static_cast<int>(canvas.size()) &&
                x >= 0 && x < static_cast<int>(canvas[y].size()) &&
                canvas[y][x] == ' ')
            {
                canvas[y][x] = brush;
            }
        }
    }
};

inline void printMainMenuV2()
{
    std::cout << "\nPROGRA-VILLA | GrafosGameV2\n";
    std::cout << "1. Cargar ciudad desde archivo\n";
    std::cout << "2. Ver resumen\n";
    std::cout << "3. Ver lista de adyacencia\n";
    std::cout << "4. Ver matriz de adyacencia\n";
    std::cout << "5. Ver mapa ASCII\n";
    std::cout << "6. Bloquear zona\n";
    std::cout << "7. Desbloquear zona\n";
    std::cout << "8. Buscar suministros BFS\n";
    std::cout << "9. Ruta segura DFS\n";
    std::cout << "10. Ver conectividad\n";
    std::cout << "11. Ruta de menor peligro Dijkstra\n";
    std::cout << "0. Salir\n";
    std::cout << "Opcion: ";
}

inline void runGrafosGameV2()
{
    GraphGameV2 game;
    int option = -1;

    while (true)
    {
        printMainMenuV2();
        if (!(std::cin >> option))
            break;

        if (option == 0)
            break;

        if (option == 1)
        {
            std::string fileName;
            std::cout << "Archivo: ";
            std::cin >> fileName;
            if (game.loadFromFile(fileName))
                std::cout << "Ciudad cargada correctamente.\n";
            else
                std::cout << "No se pudo cargar el archivo.\n";
        }
        else if (option == 2)
        {
            game.showCitySummary();
        }
        else if (option == 3)
        {
            game.showAdjacencyList();
        }
        else if (option == 4)
        {
            game.showAdjacencyMatrix();
        }
        else if (option == 5)
        {
            game.showAsciiMap();
        }
        else if (option == 6 || option == 7)
        {
            std::string nodeId;
            std::cout << "Nodo: ";
            std::cin >> nodeId;
            const bool ok = option == 6 ? game.blockNode(nodeId) : game.unblockNode(nodeId);
            std::cout << (ok ? "Operacion realizada.\n" : "Nodo no encontrado.\n");
        }
        else if (option == 8)
        {
            std::string startId;
            std::string targetId;
            std::cout << "Refugio inicial: ";
            std::cin >> startId;
            std::cout << "Hospital objetivo: ";
            std::cin >> targetId;
            SearchResult result = game.findSupplyRouteBFS(startId, targetId);
            game.printSearchResult("Exploracion de suministros BFS", result, "Calles atravesadas");
            if (result.found)
                game.showAsciiMap(result.path);
        }
        else if (option == 9)
        {
            std::string startId;
            std::string targetId;
            std::cout << "Zona norte: ";
            std::cin >> startId;
            std::cout << "Zona sur: ";
            std::cin >> targetId;
            SearchResult result = game.findSafeRouteDFS(startId, targetId);
            game.printSearchResult("Ruta de rescate DFS", result, "Saltos explorados");
            if (result.found)
                game.showAsciiMap(result.path);
        }
        else if (option == 10)
        {
            game.showConnectivityReport();
        }
        else if (option == 11)
        {
            std::string startId;
            std::string targetId;
            std::cout << "Origen: ";
            std::cin >> startId;
            std::cout << "Destino: ";
            std::cin >> targetId;
            SearchResult result = game.findLowestDangerDijkstra(startId, targetId);
            game.printSearchResult("Ruta de menor peligro Dijkstra", result, "Peligro acumulado");
            if (result.found)
                game.showAsciiMap(result.path);
        }
        else
        {
            std::cout << "Opcion invalida.\n";
        }
    }
}

} // namespace ggv2

#endif
