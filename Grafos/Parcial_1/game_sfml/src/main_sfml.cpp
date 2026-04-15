#include "GrafosGameV2Base.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct VisualNode {
    sf::Vector2f position;
    float radius = 24.0f;
};

struct CategoryStyle {
    sf::Color fill;
    sf::Color outline;
    std::string shortLabel;
};

struct GameUiState {
    int startIndex = -1;
    int targetIndex = -1;
    std::string mode = "Dijkstra";
    ggv2::SearchResult result;
    std::string commandBuffer;
    std::vector<std::string> consoleLog;
};

std::string trim(const std::string& value)
{
    const size_t begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return "";
    const size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::string toUpper(std::string value)
{
    for (char& c : value)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return value;
}

std::vector<std::string> splitWords(const std::string& input)
{
    std::istringstream iss(input);
    std::vector<std::string> parts;
    std::string token;
    while (iss >> token)
        parts.push_back(token);
    return parts;
}

bool fileExists(const std::filesystem::path& path)
{
    std::ifstream test(path);
    return test.good();
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
        candidates.push_back(executableDir / "../data/progra_villa_v2.txt");
    }

    return resolveExistingPath(candidates);
}

sf::Font loadBestEffortFont()
{
    sf::Font font;
    const std::vector<std::string> candidates = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttf",
        "/Library/Fonts/Arial.ttf"
    };

    for (const std::string& path : candidates)
    {
        if (font.openFromFile(path))
            return font;
    }

    return font;
}

sf::Text makeText(const sf::Font& font,
                  const std::string& value,
                  unsigned int size,
                  sf::Color color,
                  sf::Vector2f position)
{
    sf::Text text(font, value, size);
    text.setFillColor(color);
    text.setPosition(position);
    return text;
}

CategoryStyle styleForKind(const std::string& kind)
{
    const std::string normalized = toUpper(kind);
    if (normalized == "HOSPITAL")
        return {sf::Color(207, 60, 84), sf::Color(255, 220, 225), "H"};
    if (normalized == "CASA")
        return {sf::Color(204, 154, 88), sf::Color(255, 236, 200), "C"};
    if (normalized == "REFUGIO")
        return {sf::Color(71, 148, 255), sf::Color(220, 236, 255), "R"};
    if (normalized == "SUPERMERCADO")
        return {sf::Color(76, 190, 124), sf::Color(218, 255, 231), "S"};
    if (normalized == "FARMACIA")
        return {sf::Color(42, 198, 193), sf::Color(210, 255, 252), "F"};
    if (normalized == "POLICIA")
        return {sf::Color(78, 109, 220), sf::Color(226, 233, 255), "P"};
    if (normalized == "NORTE")
        return {sf::Color(108, 132, 255), sf::Color(228, 232, 255), "N"};
    if (normalized == "SUR")
        return {sf::Color(255, 164, 62), sf::Color(255, 236, 205), "S"};
    if (normalized == "BARRICADA")
        return {sf::Color(170, 112, 55), sf::Color(255, 223, 185), "B"};
    if (normalized == "ZONAROJA")
        return {sf::Color(161, 42, 54), sf::Color(255, 211, 217), "X"};
    return {sf::Color(93, 105, 125), sf::Color(230, 235, 245), "?"};
}

void pushLog(GameUiState& ui, const std::string& line)
{
    if (line.empty())
    {
        ui.consoleLog.clear();
        return;
    }
    ui.consoleLog.push_back(line);
    if (ui.consoleLog.size() > 14)
        ui.consoleLog.erase(ui.consoleLog.begin());
}

void recalcRoute(ggv2::GraphGameV2& game, GameUiState& ui)
{
    if (ui.startIndex < 0 || ui.targetIndex < 0)
    {
        ui.result = ggv2::SearchResult();
        return;
    }

    const std::string startId = game.nodes()[ui.startIndex].id;
    const std::string targetId = game.nodes()[ui.targetIndex].id;

    if (ui.mode == "BFS")
        ui.result = game.findSupplyRouteBFS(startId, targetId);
    else if (ui.mode == "DFS")
        ui.result = game.findSafeRouteDFS(startId, targetId);
    else
        ui.result = game.findLowestDangerDijkstra(startId, targetId);
}

std::map<int, VisualNode> buildLayout(const ggv2::GraphGameV2& game, sf::Vector2u size)
{
    std::map<int, VisualNode> layout;
    const std::vector<ggv2::Node>& nodes = game.nodes();

    const float left = 40.0f;
    const float top = 70.0f;
    const float rightPadding = 360.0f;
    const float bottomPadding = 230.0f;
    const float mapWidth = std::max(300.0f, static_cast<float>(size.x) - rightPadding - left);
    const float mapHeight = std::max(240.0f, static_cast<float>(size.y) - bottomPadding - top);

    bool allHaveCoordinates = true;
    int minX = 1000000;
    int maxX = -1000000;
    int minY = 1000000;
    int maxY = -1000000;

    for (const ggv2::Node& node : nodes)
    {
        if (node.x < 0 || node.y < 0)
        {
            allHaveCoordinates = false;
            break;
        }
        minX = std::min(minX, node.x);
        maxX = std::max(maxX, node.x);
        minY = std::min(minY, node.y);
        maxY = std::max(maxY, node.y);
    }

    if (allHaveCoordinates)
    {
        const float spanX = static_cast<float>(std::max(1, maxX - minX));
        const float spanY = static_cast<float>(std::max(1, maxY - minY));
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            VisualNode vn;
            vn.position.x = left + ((nodes[i].x - minX) / spanX) * mapWidth;
            vn.position.y = top + ((nodes[i].y - minY) / spanY) * mapHeight;
            layout[static_cast<int>(i)] = vn;
        }
        return layout;
    }

    const float cx = left + mapWidth * 0.5f;
    const float cy = top + mapHeight * 0.5f;
    const float radius = std::min(mapWidth, mapHeight) * 0.35f;
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const float angle = -1.57f + (6.28318f * static_cast<float>(i)) / std::max<size_t>(1, nodes.size());
        VisualNode vn;
        vn.position.x = cx + std::cos(angle) * radius;
        vn.position.y = cy + std::sin(angle) * radius;
        layout[static_cast<int>(i)] = vn;
    }
    return layout;
}

bool containsEdge(const std::vector<int>& path, int a, int b)
{
    for (size_t i = 1; i < path.size(); ++i)
    {
        const int u = path[i - 1];
        const int v = path[i];
        if ((u == a && v == b) || (u == b && v == a))
            return true;
    }
    return false;
}

bool containsNode(const std::vector<int>& path, int nodeIndex)
{
    return std::find(path.begin(), path.end(), nodeIndex) != path.end();
}

int pickNode(const std::map<int, VisualNode>& layout, sf::Vector2f mouse)
{
    for (std::map<int, VisualNode>::const_iterator it = layout.begin(); it != layout.end(); ++it)
    {
        const sf::Vector2f d = mouse - it->second.position;
        if ((d.x * d.x) + (d.y * d.y) <= it->second.radius * it->second.radius)
            return it->first;
    }
    return -1;
}

std::string consoleHelp()
{
    return "help | addnode ID TIPO X Y [0/1] | addedge A B P [0/1] | block ID | unblock ID | bfs A B | dfs A B | dijkstra A B | clear";
}

bool executeCommand(const std::string& rawCommand,
                    ggv2::GraphGameV2& game,
                    GameUiState& ui,
                    const std::string& mapFile)
{
    const std::string command = trim(rawCommand);
    if (command.empty())
        return false;

    pushLog(ui, "> " + command);
    const std::vector<std::string> parts = splitWords(command);
    const std::string action = toUpper(parts[0]);

    if (action == "HELP")
    {
        pushLog(ui, consoleHelp());
        return true;
    }

    if (action == "RESET")
    {
        if (game.loadFromFile(mapFile))
        {
            ui.startIndex = game.nodeIndex("R");
            ui.targetIndex = game.nodeIndex("H");
            ui.mode = "Dijkstra";
            recalcRoute(game, ui);
            pushLog(ui, "Mapa recargado desde archivo.");
        }
        else
        {
            pushLog(ui, "No se pudo recargar el mapa.");
        }
        return true;
    }

    if (action == "CLEAR")
    {
        ui.result = ggv2::SearchResult();
        ui.mode.clear();
        pushLog(ui, "Ruta actual limpiada.");
        return true;
    }

    if (action == "ADDNODE")
    {
        if (parts.size() < 5)
        {
            pushLog(ui, "Uso: addnode ID TIPO X Y [bloqueado]");
            return false;
        }

        const std::string id = parts[1];
        const std::string type = toUpper(parts[2]);
        const int x = std::atoi(parts[3].c_str());
        const int y = std::atoi(parts[4].c_str());
        const bool blocked = parts.size() >= 6 ? std::atoi(parts[5].c_str()) != 0 : false;

        if (game.addNode(id, type, blocked, x, y))
        {
            pushLog(ui, "Nodo agregado: " + id);
            if (ui.startIndex < 0)
                ui.startIndex = game.nodeIndex(id);
        }
        else
        {
            pushLog(ui, "No se pudo agregar el nodo.");
        }
        recalcRoute(game, ui);
        return true;
    }

    if (action == "ADDEDGE")
    {
        if (parts.size() < 4)
        {
            pushLog(ui, "Uso: addedge A B peligro [bidireccional]");
            return false;
        }

        const std::string a = parts[1];
        const std::string b = parts[2];
        const int danger = std::atoi(parts[3].c_str());
        const bool bidirectional = parts.size() >= 5 ? std::atoi(parts[4].c_str()) != 0 : true;

        if (game.addEdge(a, b, danger, bidirectional))
            pushLog(ui, "Arista agregada: " + a + " -> " + b);
        else
            pushLog(ui, "No se pudo agregar la arista.");
        recalcRoute(game, ui);
        return true;
    }

    if (action == "BLOCK" || action == "UNBLOCK")
    {
        if (parts.size() < 2)
        {
            pushLog(ui, "Uso: block ID | unblock ID");
            return false;
        }
        const bool ok = action == "BLOCK" ? game.blockNode(parts[1]) : game.unblockNode(parts[1]);
        pushLog(ui, ok ? "Estado actualizado para " + parts[1] : "Nodo no encontrado.");
        recalcRoute(game, ui);
        return ok;
    }

    if (action == "BFS" || action == "DFS" || action == "DIJKSTRA")
    {
        if (parts.size() < 3)
        {
            pushLog(ui, "Uso: bfs A B | dfs A B | dijkstra A B");
            return false;
        }

        ui.startIndex = game.nodeIndex(parts[1]);
        ui.targetIndex = game.nodeIndex(parts[2]);
        if (ui.startIndex < 0 || ui.targetIndex < 0)
        {
            pushLog(ui, "Origen o destino invalido.");
            return false;
        }

        ui.mode = action == "BFS" ? "BFS" : action == "DFS" ? "DFS"
                                                           : "Dijkstra";
        recalcRoute(game, ui);
        pushLog(ui, ui.result.found ? "Ruta calculada." : ui.result.message);
        return ui.result.found;
    }

    pushLog(ui, "Comando desconocido.");
    pushLog(ui, consoleHelp());
    return false;
}

int pathDanger(const ggv2::GraphGameV2& game, const std::vector<int>& path)
{
    const auto& adj = game.adjacency();
    int total = 0;
    for (size_t i = 1; i < path.size(); ++i)
    {
        const int from = path[i - 1];
        const int to = path[i];
        for (const auto& edge : adj[from])
        {
            if (edge.to == to)
            {
                total += edge.danger;
                break;
            }
        }
    }
    return total;
}

std::string makeInfo(const ggv2::GraphGameV2& game, const GameUiState& ui)
{
    std::ostringstream out;
    out << "Progra-Villa\n";
    out << "Origen: " << (ui.startIndex >= 0 ? game.nodes()[ui.startIndex].id : "-") << "\n";
    out << "Destino: " << (ui.targetIndex >= 0 ? game.nodes()[ui.targetIndex].id : "-") << "\n";
    out << "Algoritmo: " << (ui.mode.empty() ? "-" : ui.mode) << "\n";
    out << "Nodos: " << game.nodes().size() << "\n";
    out << "Componentes: " << game.connectedComponents(true) << "\n\n";

    out << "Atajos\n";
    out << "B = BFS (menos calles)\n";
    out << "D = DFS (explorar)\n";
    out << "J = Dijkstra (min peligro)\n";
    out << "Space = ciclar modo\n";
    out << "N = bloquear/desbloquear origen\n";
    out << "R = recargar mapa\n\n";

    if (!ui.mode.empty())
    {
        out << (ui.result.found ? "Ruta" : "Sin ruta") << "\n";
        if (ui.result.found)
        {
            const int peligro = pathDanger(game, ui.result.path);
            const int calles = static_cast<int>(ui.result.path.size()) - 1;
            out << "Peligro: " << peligro << " zombies\n";
            out << "Calles: " << calles << " cuadras\n";
            out << game.formatPath(ui.result.path) << "\n";
        }
        else
        {
            out << ui.result.message << "\n";
        }
    }

    return out.str();
}

void drawConsole(sf::RenderWindow& window,
                 const sf::Font& font,
                 const GameUiState& ui,
                 sf::Vector2u size)
{
    sf::RectangleShape consoleBg({static_cast<float>(size.x), 180.0f});
    consoleBg.setPosition({0.0f, static_cast<float>(size.y) - 180.0f});
    consoleBg.setFillColor(sf::Color(17, 21, 31));
    window.draw(consoleBg);

    sf::RectangleShape inputBox({static_cast<float>(size.x) - 40.0f, 36.0f});
    inputBox.setPosition({20.0f, static_cast<float>(size.y) - 48.0f});
    inputBox.setFillColor(sf::Color(25, 31, 43));
    inputBox.setOutlineThickness(2.0f);
    inputBox.setOutlineColor(sf::Color(82, 96, 125));
    window.draw(inputBox);

    float y = static_cast<float>(size.y) - 168.0f;
    for (const std::string& line : ui.consoleLog)
    {
        sf::Text logLine = makeText(font, line, 16, sf::Color(214, 220, 231), {24.0f, y});
        window.draw(logLine);
        y += 18.0f;
    }

    sf::Text prompt = makeText(font,
                               "Consola > " + ui.commandBuffer + "_",
                               18,
                               sf::Color(240, 242, 247),
                               {28.0f, static_cast<float>(size.y) - 41.0f});
    window.draw(prompt);
}

void drawGraph(sf::RenderWindow& window,
               const sf::Font& font,
               const ggv2::GraphGameV2& game,
               const GameUiState& ui,
               const std::map<int, VisualNode>& layout)
{
    const std::vector<ggv2::Node>& nodes = game.nodes();
    const std::vector<std::vector<ggv2::Edge> >& adjacency = game.adjacency();

    for (size_t u = 0; u < adjacency.size(); ++u)
    {
        for (const ggv2::Edge& edge : adjacency[u])
        {
            if (static_cast<int>(u) > edge.to)
                continue;

            const bool highlighted = ui.result.found && containsEdge(ui.result.path, static_cast<int>(u), edge.to);
            sf::Vertex line[] = {
                {layout.at(static_cast<int>(u)).position,
                 highlighted ? sf::Color(255, 220, 90) : sf::Color(84, 96, 118)},
                {layout.at(edge.to).position,
                 highlighted ? sf::Color(255, 220, 90) : sf::Color(84, 96, 118)}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);

            const sf::Vector2f mid = (layout.at(static_cast<int>(u)).position + layout.at(edge.to).position) * 0.5f;
            sf::Text danger = makeText(font,
                                       std::to_string(edge.danger),
                                       15,
                                       highlighted ? sf::Color(255, 220, 90) : sf::Color(182, 192, 206),
                                       {mid.x + 5.0f, mid.y - 8.0f});
            window.draw(danger);
        }
    }

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const CategoryStyle style = styleForKind(nodes[i].redZone ? "ZONAROJA" : nodes[i].kind);

        sf::CircleShape circle(layout.at(static_cast<int>(i)).radius);
        circle.setOrigin({layout.at(static_cast<int>(i)).radius, layout.at(static_cast<int>(i)).radius});
        circle.setPosition(layout.at(static_cast<int>(i)).position);
        circle.setFillColor(style.fill);
        circle.setOutlineThickness(containsNode(ui.result.path, static_cast<int>(i)) ? 5.0f : 2.0f);
        circle.setOutlineColor(containsNode(ui.result.path, static_cast<int>(i)) ? sf::Color(255, 220, 90)
                                                                                 : style.outline);
        window.draw(circle);

        sf::Text icon = makeText(font,
                                 style.shortLabel,
                                 18,
                                 sf::Color::White,
                                 {layout.at(static_cast<int>(i)).position.x - 6.0f,
                                  layout.at(static_cast<int>(i)).position.y - 13.0f});
        window.draw(icon);

        const std::string name = nodes[i].id + " | " + nodes[i].kind;
        sf::Text label = makeText(font,
                                  name,
                                  15,
                                  sf::Color(236, 239, 246),
                                  {layout.at(static_cast<int>(i)).position.x + 28.0f,
                                   layout.at(static_cast<int>(i)).position.y - 12.0f});
        window.draw(label);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    const std::string mapFile = resolveMapPath(argc > 0 ? argv[0] : nullptr);

    if (mapFile.empty())
    {
        std::cerr << "No se encontro el mapa base.\n";
        return 1;
    }

    ggv2::GraphGameV2 game;
    if (!game.loadFromFile(mapFile))
    {
        std::cerr << "No se pudo cargar el mapa.\n";
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode({1440u, 900u}), "Progra-Villa | GAME_V1");
    window.setFramerateLimit(60);

    sf::Font font = loadBestEffortFont();
    const bool hasFont = font.getInfo().family != "";
    if (!hasFont)
    {
        std::cerr << "No se pudo cargar una fuente del sistema.\n";
        return 1;
    }

    GameUiState ui;
    ui.startIndex = game.nodeIndex("R");
    ui.targetIndex = game.nodeIndex("H");
    ui.mode = "Dijkstra";
    recalcRoute(game, ui);
    pushLog(ui, "GAME_V1 cargado sobre GrafosGameV2.");
    pushLog(ui, "Escribe 'help' en la consola para ver comandos.");

    while (window.isOpen())
    {
        const std::map<int, VisualNode> layout = buildLayout(game, window.getSize());

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const sf::Event::TextEntered* text = event->getIf<sf::Event::TextEntered>())
            {
                if (text->unicode >= 32 && text->unicode < 127)
                    ui.commandBuffer.push_back(static_cast<char>(text->unicode));
            }

            if (const sf::Event::KeyPressed* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                else if (key->code == sf::Keyboard::Key::Backspace)
                {
                    if (!ui.commandBuffer.empty())
                        ui.commandBuffer.pop_back();
                }
                else if (key->code == sf::Keyboard::Key::Enter)
                {
                    executeCommand(ui.commandBuffer, game, ui, mapFile);
                    ui.commandBuffer.clear();
                }
                else if (key->code == sf::Keyboard::Key::Space)
                {
                    if (ui.mode == "BFS")
                        ui.mode = "DFS";
                    else if (ui.mode == "DFS")
                        ui.mode = "Dijkstra";
                    else
                        ui.mode = "BFS";
                    recalcRoute(game, ui);
                    pushLog(ui, "Modo actual: " + ui.mode);
                }
                else if (key->code == sf::Keyboard::Key::B)
                {
                    ui.mode = "BFS";
                    recalcRoute(game, ui);
                    ui.consoleLog.clear();
                }
                else if (key->code == sf::Keyboard::Key::D)
                {
                    ui.mode = "DFS";
                    recalcRoute(game, ui);
                    ui.consoleLog.clear();
                }
                else if (key->code == sf::Keyboard::Key::J)
                {
                    ui.mode = "Dijkstra";
                    recalcRoute(game, ui);
                    ui.consoleLog.clear();
                }
                else if (key->code == sf::Keyboard::Key::R)
                {
                    game.loadFromFile(mapFile);
                    ui.startIndex = game.nodeIndex("R");
                    ui.targetIndex = game.nodeIndex("H");
                    ui.mode = "Dijkstra";
                    recalcRoute(game, ui);
                    pushLog(ui, "Mapa recargado.");
                }
                else if (key->code == sf::Keyboard::Key::N)
                {
                    if (ui.startIndex < 0)
                    {
                        pushLog(ui, "Selecciona un origen con click izquierdo antes de bloquear.");
                    }
                    else
                    {
                        const std::string nodeId = game.nodes()[ui.startIndex].id;
                        if (game.nodes()[ui.startIndex].redZone)
                        {
                            game.unblockNode(nodeId);
                            pushLog(ui, "Nodo desbloqueado: " + nodeId);
                        }
                        else
                        {
                            game.blockNode(nodeId);
                            pushLog(ui, "Nodo bloqueado: " + nodeId);
                        }
                        recalcRoute(game, ui);
                    }
                }
            }

            if (const sf::Event::MouseButtonPressed* mousePress =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                const sf::Vector2f mouse = window.mapPixelToCoords(mousePress->position);
                const int picked = pickNode(layout, mouse);
                if (picked >= 0)
                {
                    if (mousePress->button == sf::Mouse::Button::Left)
                    {
                        ui.startIndex = picked;
                        pushLog(ui, "Origen: " + game.nodes()[picked].id);
                    }
                    else if (mousePress->button == sf::Mouse::Button::Right)
                    {
                        ui.targetIndex = picked;
                        pushLog(ui, "Destino: " + game.nodes()[picked].id);
                    }
                    recalcRoute(game, ui);
                }
            }
        }

        window.clear(sf::Color(11, 15, 23));

        sf::RectangleShape sidePanel({320.0f, static_cast<float>(window.getSize().y) - 180.0f});
        sidePanel.setPosition({static_cast<float>(window.getSize().x) - 320.0f, 0.0f});
        sidePanel.setFillColor(sf::Color(21, 27, 39));
        window.draw(sidePanel);

        sf::RectangleShape mapArea({static_cast<float>(window.getSize().x) - 320.0f,
                                    static_cast<float>(window.getSize().y) - 180.0f});
        mapArea.setPosition({0.0f, 0.0f});
        mapArea.setFillColor(sf::Color(14, 19, 28));
        window.draw(mapArea);

        drawGraph(window, font, game, ui, layout);
        drawConsole(window, font, ui, window.getSize());

        sf::Text title = makeText(font,
                                  "Progra-Villa | Render 2D editable",
                                  28,
                                  sf::Color(242, 245, 250),
                                  {24.0f, 18.0f});
        window.draw(title);

        sf::Text panelInfo = makeText(font,
                                      makeInfo(game, ui),
                                      18,
                                      sf::Color(220, 226, 236),
                                      {static_cast<float>(window.getSize().x) - 300.0f, 24.0f});
        window.draw(panelInfo);

        sf::Text footer = makeText(font,
                                   "Mouse: izq=origen | der=destino. Teclado: N=bloquear origen | B=BFS | D=DFS | J=Dijkstra | Space=ciclar | R=recargar | Enter=ejecuta.",
                                   15,
                                   sf::Color(174, 182, 196),
                                   {20.0f, static_cast<float>(window.getSize().y) - 198.0f});
        window.draw(footer);

        window.display();
    }

    return 0;
}
