import matplotlib.pyplot as plt

# 1. Twoje stałe dane punktów (id, x, y)
nodes_data = [
    (1, 0.3018, 0.2126),
    (2, 0.0524, -0.1247),
    (3, -0.3028, -0.3444),
    (4, -0.0846, -0.6975)
]

# Przekształcenie danych w słownik dla szybkiego dostępu: {id: (x, y)}
nodes_dict = {item[0]: (item[1], item[2]) for item in nodes_data}

print("Wklej dane połączeń (np. 'AB 1 2 1'), a następnie naciśnij Enter i Ctrl+D (Linux/Mac) lub Ctrl+Z (Windows) + Enter, aby zakończyć:")

# 2. Pobieranie danych z Ctrl+V
input_lines = []
try:
    while True:
        line = input()
        if not line: break
        input_lines.append(line)
except EOFError:
    pass

# 3. Parsowanie połączeń
connections = []
for line in input_lines:
    parts = line.split()
    if len(parts) >= 3:
        try:
            # Wyciągamy node1 (indeks 1) i node2 (indeks 2)
            n1 = int(parts[1])
            n2 = int(parts[2])
            connections.append((n1, n2))
        except ValueError:
            continue

# 4. Tworzenie wykresu
plt.figure(figsize=(8, 8))

# Rysowanie punktów
x_coords = [p[1] for p in nodes_data]
y_coords = [p[2] for p in nodes_data]
plt.scatter(x_coords, y_coords, color='red', s=100, zorder=5)

# Dodawanie etykiet punktów
for node_id, (nx, ny) in nodes_dict.items():
    plt.annotate(str(node_id), (nx, ny), textcoords="offset points", xytext=(0,10), ha='center', fontweight='bold')

# 5. Rysowanie linii (połączeń)
for n1, n2 in connections:
    if n1 in nodes_dict and n2 in nodes_dict:
        start = nodes_dict[n1]
        end = nodes_dict[n2]
        plt.plot([start[0], end[0]], [start[1], end[1]], 'b-', alpha=0.6, zorder=1)

# Estetyka
plt.axhline(0, color='black', linewidth=0.5)
plt.axvline(0, color='black', linewidth=0.5)
plt.grid(True, linestyle='--', alpha=0.3)
plt.title('Wizualizacja połączeń między węzłami')
plt.xlabel('Oś X')
plt.ylabel('Oś Y')

plt.show()