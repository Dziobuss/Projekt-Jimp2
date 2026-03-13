#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_NODES 100
#define INF 1e9
#define EPSILON 1e-4
#define MAX_ITER 100

typedef struct {
    double x, y;
} Point;

double dist[MAX_NODES][MAX_NODES];
double l[MAX_NODES][MAX_NODES]; // Idealne długości
double k[MAX_NODES][MAX_NODES]; // Stałe sprężystości
Point pos[MAX_NODES];
int active_nodes[MAX_NODES];
int n_nodes = 0;

// Funkcja pomocnicza do mapowania ID wierzchołków
int get_idx(int id, int *nodes, int *count) {
    for (int i = 0; i < *count; i++) if (nodes[i] == id) return i;
    nodes[(*count)] = id;
    return (*count)++;
}

void compute_kamada_kawai(int n, double L_side) {
    // 1. Floyd-Warshall dla najkrótszych ścieżek
    for (int k_idx = 0; k_idx < n; k_idx++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k_idx] + dist[k_idx][j] < dist[i][j])
                    dist[i][j] = dist[i][k_idx] + dist[k_idx][j];

    // 2. Obliczanie macierzy L_ij i K_ij
    double max_d = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (dist[i][j] > max_d) max_d = dist[i][j];

    double L = L_side / max_d;
    double K = 1.0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            l[i][j] = L * dist[i][j];
            k[i][j] = K / (dist[i][j] * dist[i][j]);
        }
    }

    // 3. Inicjalizacja pozycji (na okręgu)
    for (int i = 0; i < n; i++) {
        pos[i].x = L_side * cos(2 * M_PI * i / n);
        pos[i].y = L_side * sin(2 * M_PI * i / n);
    }

    // 4. Główna pętla optymalizacji (Newton-Raphson)
    for (int iter = 0; iter < MAX_ITER; iter++) {
        double max_delta = 0;
        int target_node = -1;

        for (int i = 0; i < n; i++) {
            double dE_dx = 0, dE_dy = 0;
            for (int j = 0; j < n; j++) {
                if (i == j) continue;
                double dx = pos[i].x - pos[j].x;
                double dy = pos[i].y - pos[j].y;
                double d = sqrt(dx * dx + dy * dy);
                dE_dx += k[i][j] * (dx - l[i][j] * dx / d);
                dE_dy += k[i][j] * (dy - l[i][j] * dy / d);
            }
            double delta = sqrt(dE_dx * dE_dx + dE_dy * dE_dy);
            if (delta > max_delta) {
                max_delta = delta;
                target_node = i;
            }
        }

        if (max_delta < EPSILON) break;

        // Przesunięcie wybranego węzła (uproszczony krok)
        int i = target_node;
        double dE2_dx2 = 0, dE2_dy2 = 0, dE2_dxdy = 0;
        double dE_dx = 0, dE_dy = 0;

        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            double dx = pos[i].x - pos[j].x;
            double dy = pos[i].y - pos[j].y;
            double d2 = dx * dx + dy * dy;
            double d = sqrt(d2);
            double d3 = d * d2;

            dE_dx += k[i][j] * (dx - l[i][j] * dx / d);
            dE_dy += k[i][j] * (dy - l[i][j] * dy / d);
            dE2_dx2 += k[i][j] * (1 - l[i][j] * dy * dy / d3);
            dE2_dy2 += k[i][j] * (1 - l[i][j] * dx * dx / d3);
            dE2_dxdy += k[i][j] * (l[i][j] * dx * dy / d3);
        }

        double det = dE2_dx2 * dE2_dy2 - dE2_dxdy * dE2_dxdy;
        pos[i].x -= (dE_dx * dE2_dy2 - dE_dy * dE2_dxdy) / det;
        pos[i].y -= (-dE_dx * dE2_dxdy + dE_dy * dE2_dx2) / det;
    }
}

int main() {
    char name[10];
    int u_id, v_id;
    double weight;
    int nodes_map[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) 
        for (int j = 0; j < MAX_NODES; j++) 
            dist[i][j] = (i == j) ? 0 : INF;

    // Odczyt danych
    while (scanf("%s %d %d %lf", name, &u_id, &v_id, &weight) == 4) {
        int u = get_idx(u_id, nodes_map, &n_nodes);
        int v = get_idx(v_id, nodes_map, &n_nodes);
        dist[u][v] = dist[v][u] = weight;
    }

    compute_kamada_kawai(n_nodes, 1.0); // 1.0 to skala obszaru

    // Wynik
    for (int i = 0; i < n_nodes; i++) {
        printf("%d %.4f %.4f\n", nodes_map[i], pos[i].x, pos[i].y);
    }

    return 0;
}