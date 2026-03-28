from random import randint

nodes_count = int(input("Ile nodeow: "))
max_connections = int(input("Ile max connectow: "))
nodes_list = [chr(ord('A') + i) for i in range(nodes_count)]

wagi_input = input("Czy dawac rozne wagi (t/n): ")
wagi = (wagi_input == 't')

with open("dane.txt", mode="w", encoding="utf-8", newline='') as f:
    for i in nodes_list:
        # Losujemy liczbe polaczen dla kazdego wezla
        num_conn = randint(1, max_connections)
        
        for j in range(num_conn):
            connect = i
            while connect == i:
                connect = nodes_list[randint(0, len(nodes_list) - 1)]
            
            # Obliczamy wartosci
            id1 = ord(i) - ord('A') + 1
            id2 = ord(connect) - ord('A') + 1
            waga_val = 1 if not wagi else randint(1, 9999) / 1000
            
            # Budujemy jedna linie tekstu: "AB 1 2 0.543\n"
            linia = f"{i}{connect} {id1} {id2} {waga_val}\n"
            
            # Zapisujemy do pliku
            f.write(linia)

print("Dane zostaly zapisane do pliku dane.txt")