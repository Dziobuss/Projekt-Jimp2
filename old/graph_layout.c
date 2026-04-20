#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h> // Dodane dla obsługi losowania
#include <float.h> // Dodane dla DBL_MAX używanego w SMACOF

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define EPSILON 1e-9

typedef struct { int id; double x, y; } Vertex;
typedef struct { int u, v; double weight; } Edge;

// Bezpieczna odległość euklidesowa dla SMACOF
double safe_dist(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    double d = sqrt(dx*dx + dy*dy);
    return (d < EPSILON) ? EPSILON : d; // Nigdy nie zwracaj 0
}

// --- ALGORYTM 1: SMACOF ---
void calculate_smacof(Vertex* vertices, int num_v, Edge* edges, int num_e, int iterations) {
    // Alokacja pamięci dla macierzy odległości i wag
    double **d_target = malloc(num_v * sizeof(double*));
    double **w = malloc(num_v * sizeof(double*));
    for(int i = 0; i < num_v; i++) {
        d_target[i] = malloc(num_v * sizeof(double));
        w[i] = malloc(num_v * sizeof(double));
        for(int j = 0; j < num_v; j++) {
            d_target[i][j] = (i == j) ? 0.0 : DBL_MAX / 2.0;
        }
    }

    // Wypełnianie bezpośrednich krawędzi
    for(int i = 0; i < num_e; i++) {
        int u_idx = -1, v_idx = -1;
        for(int j = 0; j < num_v; j++) {
            if(vertices[j].id == edges[i].u) u_idx = j;
            if(vertices[j].id == edges[i].v) v_idx = j;
        }
        if(u_idx != -1 && v_idx != -1) {
            // Zabezpieczenie przed ujemnymi lub zerowymi wagami
            double weight = (edges[i].weight <= 0) ? 0.1 : edges[i].weight;
            d_target[u_idx][v_idx] = weight;
            d_target[v_idx][u_idx] = weight;
        }
    }

    // Floyd-Warshall (Najkrótsze ścieżki)
    for(int k = 0; k < num_v; k++) {
        for(int i = 0; i < num_v; i++) {
            for(int j = 0; j < num_v; j++) {
                if (d_target[i][k] + d_target[k][j] < d_target[i][j]) {
                    d_target[i][j] = d_target[i][k] + d_target[k][j];
                }
            }
        }
    }

    // Obsługa grafu niespójnego: zamień nieskończoność na dużą wartość
    double max_dist = 0;
    for(int i = 0; i < num_v; i++) {
        for(int j = 0; j < num_v; j++) {
            if(d_target[i][j] < DBL_MAX / 4.0 && d_target[i][j] > max_dist) {
                max_dist = d_target[i][j];
            }
        }
    }
    for(int i = 0; i < num_v; i++) {
        for(int j = 0; j < num_v; j++) {
            if(d_target[i][j] >= DBL_MAX / 4.0) {
                d_target[i][j] = max_dist * 2.0; 
            }
        }
    }

    // Inicjalizacja: Punkty na okręgu (nadpisuje losową z maina, lepsze dla SMACOF)
    for (int i = 0; i < num_v; i++) {
        vertices[i].x = cos(i * 2.0 * M_PI / num_v) * num_v;
        vertices[i].y = sin(i * 2.0 * M_PI / num_v) * num_v;
    }

    // 1. Oblicz wagi
    for(int i = 0; i < num_v; i++) {
        for(int j = 0; j < num_v; j++) {
            if (i == j || d_target[i][j] >= DBL_MAX / 4.0) {
                w[i][j] = 0;
            } else {
                w[i][j] = 1.0 / (d_target[i][j] * d_target[i][j]);
            }
        }
    }

    double *next_x = malloc(num_v * sizeof(double));
    double *next_y = malloc(num_v * sizeof(double));

    // 2. Pętla optymalizacji (Guttman Transform)
    for (int iter = 0; iter < iterations; iter++) {
        for (int i = 0; i < num_v; i++) {
            double sum_w = 0, sum_x = 0, sum_y = 0;

            for (int j = 0; j < num_v; j++) {
                if (i == j || w[i][j] == 0) continue;

                double d_curr = safe_dist(vertices[i].x, vertices[i].y, vertices[j].x, vertices[j].y);
                sum_w += w[i][j];
                
                // Formuła Guttmana
                sum_x += w[i][j] * (vertices[j].x + d_target[i][j] * (vertices[i].x - vertices[j].x) / d_curr);
                sum_y += w[i][j] * (vertices[j].y + d_target[i][j] * (vertices[i].y - vertices[j].y) / d_curr);
            }

            if (sum_w > 0) {
                next_x[i] = sum_x / sum_w;
                next_y[i] = sum_y / sum_w;
            } else {
                next_x[i] = vertices[i].x;
                next_y[i] = vertices[i].y;
            }
        }

        // Aktualizacja pozycji
        for (int i = 0; i < num_v; i++) {
            vertices[i].x = next_x[i];
            vertices[i].y = next_y[i];
        }
    }

    // Sprzątanie pamięci algorytmu
    for(int i = 0; i < num_v; i++) {
        free(d_target[i]);
        free(w[i]);
    }
    free(d_target); free(w);
    free(next_x); free(next_y);
}

// --- ALGORYTM 2: FRUCHTERMAN-REINGOLD (Siłowy) ---
void calculate_fruchterman(Vertex* vertices, int num_v, Edge* edges, int num_e, int iterations) {
    double area = 1000000.0;
    double k = sqrt(area / num_v); 
    double t = 10.0; // "Temperatura" - maksymalny krok przesunięcia

    for (int iter = 0; iter < iterations; iter++) {
        // 1. Odpychanie
        for (int i = 0; i < num_v; i++) {
            for (int j = 0; j < num_v; j++) {
                if (i == j) continue;
                double dx = vertices[i].x - vertices[j].x;
                double dy = vertices[i].y - vertices[j].y;
                double dist = sqrt(dx * dx + dy * dy) + 0.01;
                double force = (k * k) / dist;
                vertices[i].x += (dx / dist) * force * 0.01; // Zmniejszony mnożnik
                vertices[i].y += (dy / dist) * force * 0.01;
            }
        }
        // 2. Przyciąganie
        for (int i = 0; i < num_e; i++) {
            int u_idx = -1, v_idx = -1;
            for(int j=0; j < num_v; j++) {
                if(vertices[j].id == edges[i].u) u_idx = j;
                if(vertices[j].id == edges[i].v) v_idx = j;
            }
            if (u_idx != -1 && v_idx != -1) {
                double dx = vertices[v_idx].x - vertices[u_idx].x;
                double dy = vertices[v_idx].y - vertices[u_idx].y;
                double dist = sqrt(dx * dx + dy * dy) + 0.01;
                double force = (dist * dist) / k;
                double factor = 0.005 * edges[i].weight; // Zmniejszony mnożnik
                vertices[u_idx].x += (dx / dist) * force * factor;
                vertices[u_idx].y += (dy / dist) * force * factor;
                vertices[v_idx].x -= (dx / dist) * force * factor;
                vertices[v_idx].y -= (dy / dist) * force * factor;
            }
        }
    }
}

void save_to_file(const char* fn, Vertex* v, int n) {
    FILE* f = fopen(fn, "w");
    if (!f) return;
    for (int i = 0; i < n; i++) fprintf(f, "%d %f %f\n", v[i].id, v[i].x, v[i].y);
    fclose(f);
}

void save_to_binary(const char* fn, Vertex* v, int n) {
    FILE* f = fopen(fn, "wb");
    if (!f) return;
    fwrite(&n, sizeof(int), 1, f);
    fwrite(v, sizeof(Vertex), n, f);
    fclose(f);
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Inicjalizacja generatora liczb losowych
    
    char *input = NULL, *output = NULL, *algo = "smacof", *format = "txt";
    int iter = 1000;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("Uzycie: %s -i <in> -o <out> -a <smacof|fr> [-n iter] [-f txt|bin]\n", argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) input = argv[++i];
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) output = argv[++i];
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) algo = argv[++i];
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) iter = atoi(argv[++i]);
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) format = argv[++i];
    }

    if (!input || !output) return 1;

    Edge edges[500]; int num_e = 0, node_map[500], num_v = 0;
    FILE* fin = fopen(input, "r");
    if (!fin) {
        fprintf(stderr, "Blad: Nie mozna otworzyc pliku (Kod 1)\n");
        return 1;
    }

    char e_name[20];
    while (fscanf(fin, "%s %d %d %lf", e_name, &edges[num_e].u, &edges[num_e].v, &edges[num_e].weight) == 4) {
        int u_f = 0, v_f = 0;
        for(int j=0; j<num_v; j++) {
            if(node_map[j] == edges[num_e].u) u_f = 1;
            if(node_map[j] == edges[num_e].v) v_f = 1;
        }
        if(!u_f) node_map[num_v++] = edges[num_e].u;
        if(!v_f) node_map[num_v++] = edges[num_e].v;
        num_e++;
        if (num_e >= 500) break;
    }
    fclose(fin);

    if (num_v == 0) {
        fprintf(stderr, "Blad: Bledny format danych (Kod 2)\n");
        return 2;
    }

    Vertex* vertices = malloc(num_v * sizeof(Vertex));
    if (!vertices) return 4;

    // INICJALIZACJA LOSOWA - kluczowa dla wizualizacji 2D
    for(int i=0; i<num_v; i++) {
        vertices[i].id = node_map[i];
        vertices[i].x = (double)(rand() % 1000) - 500.0;
        vertices[i].y = (double)(rand() % 1000) - 500.0;
    }

    if (strcmp(algo, "smacof") == 0) {
        calculate_smacof(vertices, num_v, edges, num_e, iter);
    } else if (strcmp(algo, "fr") == 0) {
        calculate_fruchterman(vertices, num_v, edges, num_e, iter);
    } else {
        fprintf(stderr, "Blad: Nieznany algorytm. Uzyj smacof lub fr (Kod 3)\n");
        free(vertices);
        return 3;
    }

    if (strcmp(format, "bin") == 0) save_to_binary(output, vertices, num_v);
    else save_to_file(output, vertices, num_v);

    free(vertices);
    return 0;
}