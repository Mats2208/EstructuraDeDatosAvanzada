#include <iostream>
#include <vector>
#include <limits>
using namespace std;

const int INF = numeric_limits<int>::max();

// =======================
// ESTRUCTURA DE ARISTA
// =======================
struct Edge {
    char to;
    int weight;
};

// Grafo: A=0, B=1, ..., G=6
vector<Edge> graph[7];

// =======================
// CONVERSIÓN LETRA → ÍNDICE
// =======================
int idx(char c) {
    return c - 'A';
}

// =======================
// CONSTRUCCIÓN DEL GRAFO
// =======================
void construirGrafo() {

    // A
    graph[idx('A')].push_back({'B', 2});
    graph[idx('A')].push_back({'D', 1});

    // B
    graph[idx('B')].push_back({'C', 3});

    // D
    graph[idx('D')].push_back({'C', 2});
    graph[idx('D')].push_back({'E', 1});

    // C
    graph[idx('C')].push_back({'F', 3});

    // E
    graph[idx('E')].push_back({'G', 2});

    // G
    graph[idx('G')].push_back({'F', 1});

    // Extras del grafo (no afectan desde A pero están en la imagen)
    graph[idx('G')].push_back({'A', 3});
    graph[idx('G')].push_back({'B', 4});
    graph[idx('G')].push_back({'D', 2});
}

// =======================
// IMPRIMIR DISTANCIAS
// =======================
void imprimirEstado(vector<int>& dist) {
    cout << "Distancias: ";
    for (int i = 0; i < 7; i++) {
        if (dist[i] == INF) cout << char(i + 'A') << "=∞ ";
        else cout << char(i + 'A') << "=" << dist[i] << " ";
    }
    cout << endl;
}

// =======================
// DIJKSTRA MANUAL
// =======================
void dijkstra(char start) {

    int n = 7;

    vector<int> dist(n, INF);
    vector<int> pred(n, -1);
    vector<bool> visited(n, false);

    dist[idx(start)] = 0;

    cout << "Inicio desde: " << start << endl;
    imprimirEstado(dist);

    for (int i = 0; i < n; i++) {

        // 🔍 Buscar nodo no visitado con menor distancia
        int u = -1;
        int minDist = INF;

        for (int j = 0; j < n; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (u == -1) break;

        visited[u] = true;

        cout << "\n=====================\n";
        cout << "Fijando nodo: " << char(u + 'A') << endl;

        // 🔁 Relajar vecinos
        for (auto edge : graph[u]) {

            int v = idx(edge.to);
            int peso = edge.weight;

            cout << "Evaluando: " << char(u + 'A') 
                 << " -> " << edge.to 
                 << " (peso " << peso << ")";

            if (dist[u] + peso < dist[v]) {
                dist[v] = dist[u] + peso;
                pred[v] = u;

                cout << "  ✔ ACTUALIZA → nueva dist = " 
                     << dist[v] 
                     << " | pred = " << char(u + 'A');
            } else {
                cout << "  ✖ NO mejora";
            }

            cout << endl;
        }

        imprimirEstado(dist);
    }

    // =======================
    // RESULTADO FINAL
    // =======================
    cout << "\n\n=== RESULTADO FINAL ===\n";

    for (int i = 0; i < n; i++) {
        cout << char(i + 'A') << " -> dist: ";

        if (dist[i] == INF) cout << "∞";
        else cout << dist[i];

        cout << " | pred: ";
        if (pred[i] == -1) cout << "-";
        else cout << char(pred[i] + 'A');

        cout << endl;
    }

    // =======================
    // RECONSTRUIR RUTAS
    // =======================
    cout << "\n=== RUTAS DESDE " << start << " ===\n";

    for (int i = 0; i < n; i++) {

        if (dist[i] == INF) continue;

        cout << char(i + 'A') << ": ";

        vector<char> path;
        int current = i;

        while (current != -1) {
            path.push_back(char(current + 'A'));
            current = pred[current];
        }

        for (int j = path.size() - 1; j >= 0; j--) {
            cout << path[j];
            if (j > 0) cout << " -> ";
        }

        cout << endl;
    }
}

// =======================
// MAIN
// =======================
int main() {

    construirGrafo();

    dijkstra('F');

    return 0;
}