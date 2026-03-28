import matplotlib.pyplot as plt
import sys
import os

def visualize_graph(coords_file, edges_file):
    # 1. Sprawdzanie czy pliki istnieją
    if not os.path.exists(coords_file):
        print(f"BŁĄD: Plik ze współrzędnymi '{coords_file}' nie istnieje!")
        return
    if not os.path.exists(edges_file):
        print(f"BŁĄD: Plik z danymi grafu '{edges_file}' nie istnieje!")
        return

    coords = {}
    # 2. Wczytywanie współrzędnych (format: ID X Y)
    with open(coords_file, 'r') as f:
        for line in f:
            parts = line.split()
            if len(parts) >= 3:
                node_id, x, y = parts[0], float(parts[1]), float(parts[2])
                coords[node_id] = (x, y)

    plt.figure(figsize=(10, 10))
    ax = plt.gca()

    # 3. Rysowanie krawędzi (format: E_NAME U V WEIGHT)
    with open(edges_file, 'r') as f:
        for line in f:
            # Ignoruj komentarze i puste linie
            if line.startswith('#') or not line.strip():
                continue
            parts = line.split()
            if len(parts) >= 3:
                u, v = parts[1], parts[2]
                if u in coords and v in coords:
                    x_vals = [coords[u][0], coords[v][0]]
                    y_vals = [coords[u][1], coords[v][1]]
                    # Rysuj linię (szara, półprzezroczysta)
                    plt.plot(x_vals, y_vals, color='gray', linestyle='-', alpha=0.5, zorder=1)

    # 4. Rysowanie wierzchołków
    x_pts = [c[0] for c in coords.values()]
    y_pts = [c[1] for c in coords.values()]
    labels = list(coords.keys())

    plt.scatter(x_pts, y_pts, color='red', s=150, edgecolors='black', zorder=2)

    # Dodawanie etykiet ID nad wierzchołkami
    for label, x, y in zip(labels, x_pts, y_pts):
        plt.annotate(label, (x, y), textcoords="offset points", xytext=(0,12), ha='center', fontweight='bold')

    plt.title(f"Wizualizacja układu grafu\nPlik: {coords_file}", fontsize=14)
    plt.axis('equal') # Kluczowe dla zachowania kształtu koła
    plt.grid(True, linestyle=':', alpha=0.6)
    
    print(f"Wyświetlam wykres dla plików: {coords_file} oraz {edges_file}")
    plt.show()

if __name__ == "__main__":
    # Jeśli nie podano argumentów, skrypt spróbuje użyć tych nazw domyślnie
    c_file = sys.argv[1] if len(sys.argv) > 1 else "wynik.txt"
    d_file = sys.argv[2] if len(sys.argv) > 2 else "dane.txt"
    
    visualize_graph(c_file, d_file)
