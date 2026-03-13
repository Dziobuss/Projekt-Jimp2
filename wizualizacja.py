import matplotlib.pyplot as plt

nodes_dict = {}
connections = []
file_path = "dane.txt"

try:
    # Otwieramy plik do odczytu
    with open(file_path, "r", encoding="utf-8", errors='ignore') as f:
        input_lines = f.readlines()
    
    # Parsowanie danych linia po linii
    for line in input_lines:
        # strip() usuwa biale znaki i ewentualne \r (problematyczne ^M)
        parts = line.strip().split()
        if len(parts) == 0: 
            continue
        
        # Jesli linia ma 3 elementy i pierwszy to liczba -> to jest punkt (id, x, y)
        if len(parts) == 3 and parts[0].isdigit():
            try:
                n_id = int(parts[0])
                nx = float(parts[1])
                ny = float(parts[2])
                nodes_dict[n_id] = (nx, ny)
            except ValueError: 
                pass
            
        # Jesli linia ma 4 elementy (np. AB 1 2 1) -> to jest polaczenie
        elif len(parts) == 4:
            try:
                n1 = int(parts[1])
                n2 = int(parts[2])
                connections.append((n1, n2))
            except ValueError: 
                pass

    if not nodes_dict:
        print(f"Blad: Nie znaleziono punktow w pliku {file_path}. Sprawdz format danych.")
    else:
        # Tworzenie wykresu
        plt.figure(figsize=(10, 8))

        # Rysowanie polaczen
        for n1, n2 in connections:
            if n1 in nodes_dict and n2 in nodes_dict:
                p1 = nodes_dict[n1]
                p2 = nodes_dict[n2]
                plt.plot([p1[0], p2[0]], [p1[1], p2[1]], 'b-', alpha=0.7)

        # Rysowanie punktow i etykiet
        for n_id, (nx, ny) in nodes_dict.items():
            plt.scatter(nx, ny, color='red', s=100, zorder=5)
            plt.annotate(str(n_id), (nx, ny), textcoords="offset points", xytext=(0,10), ha='center')

        plt.grid(True, linestyle='--', alpha=0.5)
        plt.axhline(0, color='black', lw=0.5)
        plt.axvline(0, color='black', lw=0.5)
        plt.title(f"Wizualizacja Grafu z pliku: {file_path}")
        plt.show()

except FileNotFoundError:
    print(f"Blad: Nie znaleziono pliku '{file_path}' w tym samym folderze co skrypt.")