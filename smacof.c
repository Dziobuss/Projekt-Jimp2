#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define MAX_NODES 500
#define ITERATIONS 200 // Zwiększono dla lepszej stabilności
#define EPSILON 1e-9

typedef struct {
    double x, y;
} Point;

// Bezpieczna odległość euklidesowa
double safe_dist(Point a, Point b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    double d = sqrt(dx*dx + dy*dy);
    return (d < EPSILON) ? EPSILON : d; // Nigdy nie zwracaj 0
}

void smacof(int n, double d_target[MAX_NODES][MAX_NODES], Point coords[MAX_NODES], int exists[MAX_NODES]) {
    double w[MAX_NODES][MAX_NODES];
    
    // 1. Oblicz wagi i przygotuj macierz celu
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j || !exists[i] || !exists[j] || d_target[i][j] >= DBL_MAX/4) {
                w[i][j] = 0;
            } else {
                // Standardowa waga SMACOF: 1 / d^2
                w[i][j] = 1.0 / (d_target[i][j] * d_target[i][j]);
            }
        }
    }

    Point next_coords[MAX_NODES];

    // 2. Pętla optymalizacji (Guttman Transform)
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < n; i++) {
            if (!exists[i]) continue;

            double sum_w = 0;
            double sum_x = 0;
            double sum_y = 0;

            for (int j = 0; j < n; j++) {
                if (i == j || !exists[j] || w[i][j] == 0) continue;

                double d_curr = safe_dist(coords[i], coords[j]);
                sum_w += w[i][j];
                
                // Formuła Guttmana
                sum_x += w[i][j] * (coords[j].x + d_target[i][j] * (coords[i].x - coords[j].x) / d_curr);
                sum_y += w[i][j] * (coords[j].y + d_target[i][j] * (coords[i].y - coords[j].y) / d_curr);
            }

            if (sum_w > 0) {
                next_coords[i].x = sum_x / sum_w;
                next_coords[i].y = sum_y / sum_w;
            } else {
                next_coords[i] = coords[i];
            }
        }

        // Aktualizacja pozycji
        for (int i = 0; i < n; i++) {
            if (exists[i]) coords[i] = next_coords[i];
        }
    }
}

int main() {
    double adj[MAX_NODES][MAX_NODES];
    int exists[MAX_NODES] = {0};
    Point coords[MAX_NODES];
    int max_id = 0;

    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            adj[i][j] = (i == j) ? 0 : DBL_MAX / 2;
        }
    }

    char name[50];
    int u, v;
    double weight;

    // Czytanie danych
    while (scanf("%s %d %d %lf", name, &u, &v, &weight) == 4) {
        if (u >= MAX_NODES || v >= MAX_NODES) continue;
        if (weight <= 0) weight = 0.1; // Waga nie może być <= 0
        adj[u][v] = adj[v][u] = weight;
        exists[u] = exists[v] = 1;
        if (u > max_id) max_id = u;
        if (v > max_id) max_id = v;
    }

    int n = max_id + 1;

    // Floyd-Warshall (Najkrótsze ścieżki)
    for (int k = 0; k < n; k++) {
        if (!exists[k]) continue;
        for (int i = 0; i < n; i++) {
            if (!exists[i]) continue;
            for (int j = 0; j < n; j++) {
                if (exists[j] && adj[i][k] + adj[k][j] < adj[i][j])
                    adj[i][j] = adj[i][k] + adj[k][j];
            }
        }
    }

    // Obsługa grafu niespójnego: zamień nieskończoność na dużą wartość
    // (Inaczej punkty odlecą w kosmos)
    double max_dist = 0;
    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            if(exists[i] && exists[j] && adj[i][j] < DBL_MAX/4 && adj[i][j] > max_dist)
                max_dist = adj[i][j];

    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            if(exists[i] && exists[j] && adj[i][j] >= DBL_MAX/4)
                adj[i][j] = max_dist * 2.0; 

    // Inicjalizacja: Punkty na okręgu (lepiej niż totalnie losowo)
    for (int i = 0; i < n; i++) {
        if (exists[i]) {
            coords[i].x = cos(i * 2.0 * M_PI / n) * n;
            coords[i].y = sin(i * 2.0 * M_PI / n) * n;
        }
    }

    smacof(n, adj, coords, exists);

    // Wynik
    for (int i = 0; i < n; i++) {
        if (exists[i]) {
            printf("%d %.4f %.4f\n", i, coords[i].x, coords[i].y);
        }
    }

    return 0;
}