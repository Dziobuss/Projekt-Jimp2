import matplotlib.pyplot as plt
import os

def visualize_graph(nodes_file="output", edges_file="dane.txt"):
    nodes_dict = {}
    connections = []

    # --- 1. WCZYTYWANIE WIERZCHOŁKÓW (output) ---
    if os.path.exists(nodes_file):
        try:
            with open(nodes_file, "r", encoding="utf-8", errors='ignore') as f:
                for line in f:
                    parts = line.strip().split()
                    # Szukamy linii: ID X Y (3 elementy)
                    if len(parts) == 3 and parts[0].lstrip('-').isdigit():
                        n_id = int(parts[0])
                        nx = float(parts[1])
                        ny = float(parts[2])
                        nodes_dict[n_id] = (nx, ny)
            print(f"Wczytano {len(nodes_dict)} wierzchołków z pliku {nodes_file}.")
        except Exception as e:
            print(f"Błąd przy wczytywaniu wierzchołków: {e}")
    else:
        print(f"Błąd: Nie znaleziono pliku z wierzchołkami '{nodes_file}'!")
        return

    # --- 2. WCZYTYWANIE POŁĄCZEŃ (dane.txt) ---
    if os.path.exists(edges_file):
        try:
            with open(edges_file, "r", encoding="utf-8", errors='ignore') as f:
                for line in f:
                    parts = line.strip().split()
                    # Obsługa formatu: TEKST ID1 ID2 COŚ (np. AB 1 2 1)
                    if len(parts) >= 3:
                        try:
                            # Zakładamy, że ID1 i ID2 są na 1. i 2. indeksie (po tekście)
                            # Jeśli Twoje dane.txt mają inny układ, skoryguj indeksy poniżej
                            n1 = int(parts[1])
                            n2 = int(parts[2])
                            connections.append((n1, n2))
                        except (ValueError, IndexError):
                            continue
            print(f"Wczytano {len(connections)} potencjalnych połączeń z pliku {edges_file}.")
        except Exception as e:
            print(f"Błąd przy wczytywaniu połączeń: {e}")
    else:
        print(f"Ostrzeżenie: Nie znaleziono pliku z połączeniami '{edges_file}'.")

    if not nodes_dict:
        print("Nie ma czego rysować – brak wierzchołków.")
        return

    # --- 3. RYSOWANIE ---
    plt.figure(figsize=(10, 8))
    
    # Rysowanie krawędzi
    drawn_edges = 0
    for n1, n2 in connections:
        if n1 in nodes_dict and n2 in nodes_dict:
            p1 = nodes_dict[n1]
            p2 = nodes_dict[n2]
            plt.plot([p1[0], p2[0]], [p1[1], p2[1]], color='blue', alpha=0.5, zorder=1)
            drawn_edges += 1
    
    # Rysowanie punktów
    x_coords = [p[0] for p in nodes_dict.values()]
    y_coords = [p[1] for p in nodes_dict.values()]
    plt.scatter(x_coords, y_coords, color='red', s=100, zorder=2)

    # Etykiety punktów
    for n_id, (nx, ny) in nodes_dict.items():
        plt.annotate(str(n_id), (nx, ny), textcoords="offset points", xytext=(0,10), ha='center')

    plt.title(f"Graf: {len(nodes_dict)} węzłów, {drawn_edges} krawędzi")
    plt.grid(True, linestyle='--', alpha=0.3)
    plt.show()

# Start
visualize_graph("output", "dane.txt")