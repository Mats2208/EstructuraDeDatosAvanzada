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
#include <utility>
#include <vector>

namespace {

struct VisualNode {
    sf::Vector2f position;
    float radius = 36.0f;
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

struct CameraState {
    sf::Vector2f center = {0.0f, 0.0f};
    float zoom = 1.0f;
    bool dragging = false;
    sf::Vector2f lastMouse = {0.0f, 0.0f};
};

constexpr float kMinZoom = 0.24f;
constexpr float kMaxZoom = 2.3f;

struct GameAssets {
    std::map<std::string, sf::Texture> textures;

    bool load(const std::string& key, const std::string& path)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
            return false;
        texture.setSmooth(false);
        if (key == "background")
            texture.setRepeated(true);
        textures.emplace(key, std::move(texture));
        return true;
    }

    const sf::Texture* get(const std::string& key) const
    {
        const std::map<std::string, sf::Texture>::const_iterator it = textures.find(key);
        if (it == textures.end())
            return nullptr;
        return &it->second;
    }
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

std::string resolveMapPath(int argc, char* argv[])
{
    if (argc > 1 && argv[1] != nullptr && argv[1][0] != '\0')
    {
        const std::filesystem::path requested(argv[1]);
        const std::vector<std::filesystem::path> requestedCandidates = {
            requested,
            std::filesystem::current_path() / requested,
            "Parcial_1/data" / requested
        };
        const std::string resolved = resolveExistingPath(requestedCandidates);
        if (!resolved.empty())
            return resolved;
    }

    std::vector<std::filesystem::path> candidates = {
        "progra_villa_v2.txt",
        "../data/progra_villa_v2.txt",
        "data/progra_villa_v2.txt",
        "Parcial_1/game_sfml/progra_villa_v2.txt",
        "Parcial_1/data/progra_villa_v2.txt"
    };

    if (argc > 0 && argv[0] != nullptr && argv[0][0] != '\0')
    {
        const std::filesystem::path executableDir =
            std::filesystem::absolute(argv[0]).parent_path();

        candidates.push_back(executableDir / "../progra_villa_v2.txt");
        candidates.push_back(executableDir / "../../data/progra_villa_v2.txt");
        candidates.push_back(executableDir / "../../../data/progra_villa_v2.txt");
        candidates.push_back(executableDir / "../data/progra_villa_v2.txt");
    }

    return resolveExistingPath(candidates);
}

std::string resolveAssetPath(const char* executablePath, const std::string& relativePath)
{
    std::vector<std::filesystem::path> candidates = {
        relativePath,
        "assets/sprites/" + relativePath,
        "Parcial_1/PROGRA_VILLA_UI/game_sfml/assets/sprites/" + relativePath,
        "../assets/sprites/" + relativePath,
        "../../assets/sprites/" + relativePath
    };

    if (executablePath != nullptr && executablePath[0] != '\0')
    {
        const std::filesystem::path executableDir =
            std::filesystem::absolute(executablePath).parent_path();

        candidates.push_back(executableDir / "../assets/sprites" / relativePath);
        candidates.push_back(executableDir / "../../assets/sprites" / relativePath);
        candidates.push_back(executableDir / "../../../assets/sprites" / relativePath);
    }

    return resolveExistingPath(candidates);
}

GameAssets loadAssets(const char* executablePath)
{
    GameAssets assets;
    const std::vector<std::pair<std::string, std::string> > files = {
        {"background", "city_map.png"},
        {"barricade", "barricade.png"},
        {"danger_zone", "danger_zone.png"},
        {"hospital", "hospital.png"},
        {"house", "house.png"},
        {"marker_end", "marker_end.png"},
        {"marker_start", "marker_start.png"},
        {"north_checkpoint", "north_checkpoint.png"},
        {"pharmacy", "pharmacy.png"},
        {"police_station", "police_station.png"},
        {"refuge", "refuge.png"},
        {"road_blocked", "road_blocked.png"},
        {"road_low_danger", "road_low_danger.png"},
        {"road_medium_danger", "road_medium_danger.png"},
        {"road_safe", "road_safe.png"},
        {"road_selected", "road_selected.png"},
        {"south_exit", "south_exit.png"},
        {"supermarket", "supermarket.png"}
    };

    for (const std::pair<std::string, std::string>& file : files)
    {
        const std::string path = resolveAssetPath(executablePath, file.second);
        if (!path.empty())
            assets.load(file.first, path);
    }

    return assets;
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

void drawPanel(sf::RenderWindow& window,
               sf::FloatRect bounds,
               sf::Color fill = sf::Color(18, 24, 36, 232),
               sf::Color outline = sf::Color(72, 88, 116, 190))
{
    sf::RectangleShape panel(bounds.size);
    panel.setPosition(bounds.position);
    panel.setFillColor(fill);
    panel.setOutlineThickness(1.0f);
    panel.setOutlineColor(outline);
    window.draw(panel);
}

void drawSectionTitle(sf::RenderWindow& window,
                      const sf::Font& font,
                      const std::string& title,
                      sf::Vector2f position)
{
    sf::Text text = makeText(font, title, 15, sf::Color(255, 223, 96), position);
    text.setStyle(sf::Text::Style::Bold);
    window.draw(text);
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

std::string textureKeyForKind(const std::string& kind, bool redZone)
{
    if (redZone)
        return "danger_zone";

    const std::string normalized = toUpper(kind);
    if (normalized == "HOSPITAL")
        return "hospital";
    if (normalized == "CASA")
        return "house";
    if (normalized == "REFUGIO")
        return "refuge";
    if (normalized == "SUPERMERCADO")
        return "supermarket";
    if (normalized == "FARMACIA")
        return "pharmacy";
    if (normalized == "POLICIA")
        return "police_station";
    if (normalized == "NORTE")
        return "north_checkpoint";
    if (normalized == "SUR")
        return "south_exit";
    if (normalized == "BARRICADA")
        return "barricade";
    if (normalized == "ZONAROJA")
        return "danger_zone";
    return "house";
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

sf::FloatRect mapViewport(sf::Vector2u size)
{
    return sf::FloatRect({0.0f, 0.0f},
                         {static_cast<float>(size.x) - 320.0f,
                          static_cast<float>(size.y) - 180.0f});
}

std::map<int, sf::Vector2f> buildWorldLayout(const ggv2::GraphGameV2& game)
{
    std::map<int, sf::Vector2f> layout;
    const std::vector<ggv2::Node>& nodes = game.nodes();
    const float unit = 30.0f;

    bool allHaveCoordinates = true;
    for (const ggv2::Node& node : nodes)
    {
        if (node.x == -1 && node.y == -1)
        {
            allHaveCoordinates = false;
            break;
        }
    }

    if (allHaveCoordinates)
    {
        for (size_t i = 0; i < nodes.size(); ++i)
            layout[static_cast<int>(i)] = {nodes[i].x * unit, nodes[i].y * unit};
        return layout;
    }

    const int cols = std::max(2, static_cast<int>(std::sqrt(static_cast<double>(nodes.size()))) + 1);
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const int row = static_cast<int>(i) / cols;
        const int col = static_cast<int>(i) % cols;
        layout[static_cast<int>(i)] = {col * 180.0f, row * 130.0f};
    }
    return layout;
}

sf::Vector2f graphWorldCenter(const ggv2::GraphGameV2& game)
{
    const std::map<int, sf::Vector2f> layout = buildWorldLayout(game);
    if (layout.empty())
        return {0.0f, 0.0f};

    float minX = layout.begin()->second.x;
    float maxX = layout.begin()->second.x;
    float minY = layout.begin()->second.y;
    float maxY = layout.begin()->second.y;

    for (const std::pair<const int, sf::Vector2f>& item : layout)
    {
        minX = std::min(minX, item.second.x);
        maxX = std::max(maxX, item.second.x);
        minY = std::min(minY, item.second.y);
        maxY = std::max(maxY, item.second.y);
    }

    return {(minX + maxX) * 0.5f, (minY + maxY) * 0.5f};
}

sf::Vector2f worldToScreen(sf::Vector2f world, sf::FloatRect viewport, const CameraState& camera)
{
    return viewport.position + (viewport.size * 0.5f) + ((world - camera.center) * camera.zoom);
}

sf::Vector2f screenToWorld(sf::Vector2f screen, sf::FloatRect viewport, const CameraState& camera)
{
    return ((screen - viewport.position - (viewport.size * 0.5f)) / camera.zoom) + camera.center;
}

std::map<int, VisualNode> buildLayout(const ggv2::GraphGameV2& game,
                                      sf::FloatRect viewport,
                                      const CameraState& camera)
{
    std::map<int, VisualNode> layout;
    const std::map<int, sf::Vector2f> worldLayout = buildWorldLayout(game);

    for (const std::pair<const int, sf::Vector2f>& item : worldLayout)
    {
        VisualNode node;
        node.position = worldToScreen(item.second, viewport, camera);
        node.radius = 36.0f * camera.zoom;
        layout[item.first] = node;
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

const sf::Texture* roadTextureForEdge(const GameAssets& assets,
                                      const ggv2::Edge& edge,
                                      bool blockedByNode)
{
    if (blockedByNode)
        return assets.get("road_medium_danger");
    if (edge.danger <= 1)
        return assets.get("road_safe");
    if (edge.danger <= 3)
        return assets.get("road_low_danger");
    return assets.get("road_medium_danger");
}

void drawConsole(sf::RenderWindow& window,
                 const sf::Font& font,
                 const GameUiState& ui,
                 sf::Vector2u size)
{
    const float top = static_cast<float>(size.y) - 176.0f;
    drawPanel(window,
              sf::FloatRect({12.0f, top}, {static_cast<float>(size.x) - 24.0f, 164.0f}),
              sf::Color(12, 17, 27, 244),
              sf::Color(78, 95, 126, 210));

    drawSectionTitle(window, font, "CONSOLA", {28.0f, top + 14.0f});

    sf::RectangleShape inputBox({static_cast<float>(size.x) - 52.0f, 34.0f});
    inputBox.setPosition({26.0f, static_cast<float>(size.y) - 50.0f});
    inputBox.setFillColor(sf::Color(18, 25, 39, 245));
    inputBox.setOutlineThickness(1.0f);
    inputBox.setOutlineColor(sf::Color(97, 118, 154, 220));
    window.draw(inputBox);

    const size_t maxVisible = 5;
    const size_t begin = ui.consoleLog.size() > maxVisible ? ui.consoleLog.size() - maxVisible : 0;
    float y = top + 42.0f;
    for (size_t i = begin; i < ui.consoleLog.size(); ++i)
    {
        sf::Text logLine = makeText(font, ui.consoleLog[i], 15, sf::Color(203, 211, 225), {28.0f, y});
        window.draw(logLine);
        y += 17.0f;
    }

    sf::Text prompt = makeText(font,
                               "Consola > " + ui.commandBuffer + "_",
                               17,
                               sf::Color(240, 242, 247),
                               {34.0f, static_cast<float>(size.y) - 42.0f});
    window.draw(prompt);
}

void drawKeyChip(sf::RenderWindow& window,
                 const sf::Font& font,
                 const std::string& key,
                 const std::string& label,
                 sf::Vector2f position)
{
    sf::RectangleShape chip({34.0f, 22.0f});
    chip.setPosition(position);
    chip.setFillColor(sf::Color(34, 44, 63, 245));
    chip.setOutlineThickness(1.0f);
    chip.setOutlineColor(sf::Color(105, 125, 160, 210));
    window.draw(chip);

    sf::Text keyText = makeText(font, key, 13, sf::Color(255, 223, 96), {position.x + 9.0f, position.y + 3.0f});
    keyText.setStyle(sf::Text::Style::Bold);
    window.draw(keyText);

    sf::Text labelText = makeText(font, label, 14, sf::Color(210, 218, 232), {position.x + 43.0f, position.y + 2.0f});
    window.draw(labelText);
}

void drawRightHud(sf::RenderWindow& window,
                  const sf::Font& font,
                  const ggv2::GraphGameV2& game,
                  const GameUiState& ui,
                  sf::Vector2u size)
{
    const float panelX = static_cast<float>(size.x) - 320.0f;
    drawPanel(window,
              sf::FloatRect({panelX, 0.0f}, {320.0f, static_cast<float>(size.y) - 180.0f}),
              sf::Color(14, 20, 32, 245),
              sf::Color(45, 58, 80, 220));

    sf::Text title = makeText(font, "PROGRA-VILLA", 22, sf::Color(243, 246, 252), {panelX + 24.0f, 26.0f});
    title.setStyle(sf::Text::Style::Bold);
    window.draw(title);

    sf::Text subtitle = makeText(font, "Mapa de rutas seguras", 13, sf::Color(150, 164, 188), {panelX + 24.0f, 54.0f});
    window.draw(subtitle);

    drawPanel(window, sf::FloatRect({panelX + 18.0f, 84.0f}, {284.0f, 128.0f}), sf::Color(22, 30, 45, 236));
    drawSectionTitle(window, font, "ESTADO", {panelX + 34.0f, 100.0f});

    std::ostringstream state;
    state << "Origen: " << (ui.startIndex >= 0 ? game.nodes()[ui.startIndex].id : "-") << "\n";
    state << "Destino: " << (ui.targetIndex >= 0 ? game.nodes()[ui.targetIndex].id : "-") << "\n";
    state << "Algoritmo: " << (ui.mode.empty() ? "-" : ui.mode) << "\n";
    state << "Nodos: " << game.nodes().size() << "   Componentes: " << game.connectedComponents(true);
    sf::Text stateText = makeText(font, state.str(), 15, sf::Color(218, 225, 238), {panelX + 34.0f, 128.0f});
    window.draw(stateText);

    drawPanel(window, sf::FloatRect({panelX + 18.0f, 228.0f}, {284.0f, 154.0f}), sf::Color(22, 30, 45, 236));
    drawSectionTitle(window, font, "ATAJOS", {panelX + 34.0f, 244.0f});
    drawKeyChip(window, font, "B", "BFS: menos calles", {panelX + 34.0f, 274.0f});
    drawKeyChip(window, font, "D", "DFS: explorar seguro", {panelX + 34.0f, 304.0f});
    drawKeyChip(window, font, "J", "Dijkstra: min peligro", {panelX + 34.0f, 334.0f});

    drawPanel(window, sf::FloatRect({panelX + 18.0f, 398.0f}, {284.0f, 166.0f}), sf::Color(22, 30, 45, 236));
    drawSectionTitle(window, font, ui.result.found ? "RUTA ACTUAL" : "SIN RUTA", {panelX + 34.0f, 414.0f});

    std::ostringstream route;
    if (ui.result.found)
    {
        route << "Peligro: " << pathDanger(game, ui.result.path) << " zombies\n";
        route << "Calles: " << (static_cast<int>(ui.result.path.size()) - 1) << " cuadras\n\n";
        route << game.formatPath(ui.result.path);
    }
    else if (!ui.mode.empty())
    {
        route << ui.result.message;
    }
    else
    {
        route << "Selecciona origen y destino.";
    }

    sf::Text routeText = makeText(font, route.str(), 16, sf::Color(218, 225, 238), {panelX + 34.0f, 444.0f});
    window.draw(routeText);

    sf::Text hint = makeText(font,
                             "Click izq: origen\nClick der: destino\nFlechas: mover camara\n+/- o rueda: zoom\nN: bloquear origen\nR: recargar mapa",
                             14,
                             sf::Color(158, 172, 196),
                             {panelX + 34.0f, static_cast<float>(size.y) - 312.0f});
    window.draw(hint);
}

void drawMapHeader(sf::RenderWindow& window,
                   const sf::Font& font)
{
    drawPanel(window,
              sf::FloatRect({18.0f, 18.0f}, {318.0f, 48.0f}),
              sf::Color(11, 16, 24, 118),
              sf::Color(255, 255, 255, 40));

    sf::Text title = makeText(font, "Progra-Villa", 24, sf::Color(245, 248, 252), {32.0f, 26.0f});
    title.setStyle(sf::Text::Style::Bold);
    window.draw(title);

    sf::Text subtitle = makeText(font, "rutas y zonas de peligro", 13, sf::Color(205, 213, 226), {178.0f, 36.0f});
    window.draw(subtitle);
}

void drawFooterBar(sf::RenderWindow& window,
                   const sf::Font& font,
                   sf::Vector2u size)
{
    const float y = static_cast<float>(size.y) - 204.0f;
    drawPanel(window,
              sf::FloatRect({18.0f, y}, {static_cast<float>(size.x) - 356.0f, 28.0f}),
              sf::Color(11, 16, 24, 120),
              sf::Color(255, 255, 255, 35));

    sf::Text footer = makeText(font,
                               "Mouse: izq origen  |  der destino  |  Flechas mover camara  |  +/- zoom  |  B BFS  |  D DFS  |  J Dijkstra",
                               14,
                               sf::Color(218, 225, 238),
                               {30.0f, y + 6.0f});
    window.draw(footer);
}

void drawSpriteCentered(sf::RenderWindow& window,
                        const sf::Texture& texture,
                        sf::Vector2f center,
                        float targetHeight,
                        sf::Color tint = sf::Color::White)
{
    sf::Sprite sprite(texture);
    const sf::Vector2u size = texture.getSize();
    const float scale = targetHeight / static_cast<float>(std::max(1u, size.y));
    sprite.setOrigin({static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f});
    sprite.setPosition(center);
    sprite.setScale({scale, scale});
    sprite.setColor(tint);
    window.draw(sprite);
}

void drawRoadSprite(sf::RenderWindow& window,
                    const sf::Texture& texture,
                    sf::Vector2f from,
                    sf::Vector2f to,
                    float targetHeight,
                    sf::Color tint = sf::Color::White)
{
    const sf::Vector2f delta = to - from;
    const float length = std::sqrt((delta.x * delta.x) + (delta.y * delta.y));
    if (length <= 1.0f)
        return;

    const float angle = std::atan2(delta.y, delta.x) * 180.0f / 3.14159265f;
    const sf::Vector2u size = texture.getSize();

    sf::Sprite sprite(texture);
    sprite.setOrigin({0.0f, static_cast<float>(size.y) * 0.5f});
    sprite.setPosition(from);
    sprite.setRotation(sf::degrees(angle));
    sprite.setScale({length / static_cast<float>(std::max(1u, size.x)),
                     targetHeight / static_cast<float>(std::max(1u, size.y))});
    sprite.setColor(tint);
    window.draw(sprite);
}

void drawBackground(sf::RenderWindow& window,
                    const GameAssets& assets,
                    sf::FloatRect bounds,
                    const CameraState& camera)
{
    const sf::Texture* texture = assets.get("background");
    if (texture == nullptr)
        return;

    const float textureScale = 1.15f / std::max(kMinZoom, camera.zoom);
    const sf::Vector2u textureSize = texture->getSize();
    const int rectLeft = static_cast<int>((camera.center.x * 0.35f) - (bounds.size.x * 0.5f * textureScale));
    const int rectTop = static_cast<int>((camera.center.y * 0.35f) - (bounds.size.y * 0.5f * textureScale));
    const int rectWidth = static_cast<int>(bounds.size.x * textureScale);
    const int rectHeight = static_cast<int>(bounds.size.y * textureScale);

    sf::RectangleShape background(bounds.size);
    background.setPosition(bounds.position);
    background.setTexture(texture);
    background.setTextureRect(sf::IntRect({rectLeft % static_cast<int>(std::max(1u, textureSize.x)),
                                           rectTop % static_cast<int>(std::max(1u, textureSize.y))},
                                          {rectWidth, rectHeight}));
    background.setFillColor(sf::Color(255, 255, 255, 165));
    window.draw(background);
}

void drawGraph(sf::RenderWindow& window,
               const sf::Font& font,
               const ggv2::GraphGameV2& game,
               const GameUiState& ui,
               const std::map<int, VisualNode>& layout,
               const GameAssets& assets)
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
            const bool blockedByNode = nodes[u].redZone || nodes[edge.to].redZone;
            const sf::Vector2f from = layout.at(static_cast<int>(u)).position;
            const sf::Vector2f to = layout.at(edge.to).position;
            const sf::Texture* roadTexture = roadTextureForEdge(assets, edge, blockedByNode);

            if (roadTexture != nullptr)
            {
                drawRoadSprite(window,
                               *roadTexture,
                               from,
                               to,
                               highlighted ? 42.0f : 34.0f,
                               blockedByNode ? sf::Color(255, 255, 255, 170) : sf::Color(255, 255, 255, 230));
            }
            else
            {
                sf::Vertex line[] = {
                    {from, highlighted ? sf::Color(255, 220, 90) : sf::Color(84, 96, 118)},
                    {to, highlighted ? sf::Color(255, 220, 90) : sf::Color(84, 96, 118)}
                };
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }

            if (highlighted)
            {
                const sf::Texture* routeTexture = assets.get("road_selected");
                if (routeTexture != nullptr)
                    drawRoadSprite(window, *routeTexture, from, to, 28.0f, sf::Color(255, 255, 255, 245));

                sf::Vertex routeLine[] = {
                    {from, sf::Color(255, 222, 70, 230)},
                    {to, sf::Color(255, 222, 70, 230)}
                };
                window.draw(routeLine, 2, sf::PrimitiveType::Lines);
            }

            if (blockedByNode)
            {
                const sf::Texture* blockedTexture = assets.get("road_blocked");
                if (blockedTexture != nullptr)
                {
                    const sf::Vector2f midBlocked = (from + to) * 0.5f;
                    drawSpriteCentered(window, *blockedTexture, midBlocked, 46.0f, sf::Color(255, 255, 255, 220));
                }
            }

            const sf::Vector2f mid = (from + to) * 0.5f;
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
        const sf::Texture* nodeTexture = assets.get(textureKeyForKind(nodes[i].kind, nodes[i].redZone));
        const bool selectedNode = containsNode(ui.result.path, static_cast<int>(i));

        if (selectedNode)
        {
            sf::CircleShape halo(40.0f);
            halo.setOrigin({40.0f, 40.0f});
            halo.setPosition(layout.at(static_cast<int>(i)).position);
            halo.setFillColor(sf::Color(255, 222, 70, 45));
            halo.setOutlineThickness(3.0f);
            halo.setOutlineColor(sf::Color(255, 222, 70, 210));
            window.draw(halo);
        }

        if (nodeTexture != nullptr)
        {
            drawSpriteCentered(window,
                               *nodeTexture,
                               layout.at(static_cast<int>(i)).position,
                               nodes[i].redZone ? 64.0f : 72.0f);
        }
        else
        {
            sf::CircleShape circle(layout.at(static_cast<int>(i)).radius);
            circle.setOrigin({layout.at(static_cast<int>(i)).radius, layout.at(static_cast<int>(i)).radius});
            circle.setPosition(layout.at(static_cast<int>(i)).position);
            circle.setFillColor(style.fill);
            circle.setOutlineThickness(selectedNode ? 5.0f : 2.0f);
            circle.setOutlineColor(selectedNode ? sf::Color(255, 220, 90) : style.outline);
            window.draw(circle);

            sf::Text icon = makeText(font,
                                     style.shortLabel,
                                     18,
                                     sf::Color::White,
                                     {layout.at(static_cast<int>(i)).position.x - 6.0f,
                                      layout.at(static_cast<int>(i)).position.y - 13.0f});
            window.draw(icon);
        }

        if (ui.startIndex == static_cast<int>(i))
        {
            const sf::Texture* marker = assets.get("marker_start");
            if (marker != nullptr)
                drawSpriteCentered(window,
                                   *marker,
                                   layout.at(static_cast<int>(i)).position + sf::Vector2f{-28.0f, -48.0f},
                                   46.0f);
        }

        if (ui.targetIndex == static_cast<int>(i))
        {
            const sf::Texture* marker = assets.get("marker_end");
            if (marker != nullptr)
                drawSpriteCentered(window,
                                   *marker,
                                   layout.at(static_cast<int>(i)).position + sf::Vector2f{28.0f, -48.0f},
                                   46.0f);
        }

        const std::string name = nodes[i].id + " | " + nodes[i].kind;
        sf::Text label = makeText(font,
                                  name,
                                  14,
                                  sf::Color(236, 239, 246),
                                  {layout.at(static_cast<int>(i)).position.x + 34.0f,
                                   layout.at(static_cast<int>(i)).position.y + 22.0f});
        window.draw(label);
    }
}

} // namespace

int main(int argc, char* argv[])
{
    const std::string mapFile = resolveMapPath(argc, argv);

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
    const GameAssets assets = loadAssets(argc > 0 ? argv[0] : nullptr);

    sf::Font font = loadBestEffortFont();
    const bool hasFont = font.getInfo().family != "";
    if (!hasFont)
    {
        std::cerr << "No se pudo cargar una fuente del sistema.\n";
        return 1;
    }

    GameUiState ui;
    CameraState camera;
    camera.center = graphWorldCenter(game);
    ui.startIndex = game.nodeIndex("R");
    ui.targetIndex = game.nodeIndex("H");
    ui.mode = "Dijkstra";
    recalcRoute(game, ui);
    pushLog(ui, "GAME_V1 cargado sobre GrafosGameV2.");
    pushLog(ui, "Escribe 'help' en la consola para ver comandos.");

    while (window.isOpen())
    {
        const sf::FloatRect mapBounds = mapViewport(window.getSize());
        const std::map<int, VisualNode> layout = buildLayout(game, mapBounds, camera);

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
                const float panStep = 72.0f / std::max(kMinZoom, camera.zoom);
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
                else if (key->code == sf::Keyboard::Key::Left)
                {
                    camera.center.x -= panStep;
                }
                else if (key->code == sf::Keyboard::Key::Right)
                {
                    camera.center.x += panStep;
                }
                else if (key->code == sf::Keyboard::Key::Up)
                {
                    camera.center.y -= panStep;
                }
                else if (key->code == sf::Keyboard::Key::Down)
                {
                    camera.center.y += panStep;
                }
                else if (key->code == sf::Keyboard::Key::Equal || key->code == sf::Keyboard::Key::Add)
                {
                    camera.zoom = std::min(kMaxZoom, camera.zoom * 1.12f);
                }
                else if (key->code == sf::Keyboard::Key::Hyphen || key->code == sf::Keyboard::Key::Subtract)
                {
                    camera.zoom = std::max(kMinZoom, camera.zoom / 1.12f);
                }
                else if (key->code == sf::Keyboard::Key::Home)
                {
                    camera.center = graphWorldCenter(game);
                    camera.zoom = 1.0f;
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
                    camera.center = graphWorldCenter(game);
                    camera.zoom = 1.0f;
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

            if (const sf::Event::MouseWheelScrolled* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (mapBounds.contains(window.mapPixelToCoords(wheel->position)))
                {
                    const float factor = wheel->delta > 0 ? 1.12f : 1.0f / 1.12f;
                    camera.zoom = std::clamp(camera.zoom * factor, kMinZoom, kMaxZoom);
                }
            }

            if (const sf::Event::MouseButtonPressed* mousePress =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                const sf::Vector2f mouse = window.mapPixelToCoords(mousePress->position);
                if (!mapBounds.contains(mouse))
                    continue;
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
                else if (mousePress->button == sf::Mouse::Button::Left)
                {
                    camera.dragging = true;
                    camera.lastMouse = mouse;
                }
            }

            if (const sf::Event::MouseButtonReleased* mouseRelease =
                    event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (mouseRelease->button == sf::Mouse::Button::Left)
                    camera.dragging = false;
            }

            if (const sf::Event::MouseMoved* mouseMove = event->getIf<sf::Event::MouseMoved>())
            {
                if (camera.dragging)
                {
                    const sf::Vector2f mouse = window.mapPixelToCoords(mouseMove->position);
                    const sf::Vector2f delta = mouse - camera.lastMouse;
                    camera.center -= delta / std::max(kMinZoom, camera.zoom);
                    camera.lastMouse = mouse;
                    if (!mapBounds.contains(mouse))
                        camera.dragging = false;
                }
            }
        }

        window.clear(sf::Color(11, 15, 23));

        sf::RectangleShape mapArea({static_cast<float>(window.getSize().x) - 320.0f,
                                    static_cast<float>(window.getSize().y) - 180.0f});
        mapArea.setPosition({0.0f, 0.0f});
        mapArea.setFillColor(sf::Color(14, 19, 28));
        window.draw(mapArea);

        drawBackground(window,
                       assets,
                       sf::FloatRect({0.0f, 0.0f},
                                     {static_cast<float>(window.getSize().x) - 320.0f,
                                      static_cast<float>(window.getSize().y) - 180.0f}),
                       camera);
        drawGraph(window, font, game, ui, layout, assets);
        drawMapHeader(window, font);
        drawFooterBar(window, font, window.getSize());
        drawRightHud(window, font, game, ui, window.getSize());
        drawConsole(window, font, ui, window.getSize());

        window.display();
    }

    return 0;
}
