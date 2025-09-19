# Algorithms and Principles of Computer Science: Final Test

## Introduction
This is the repository for the Final Test (Prova Finale) of Algorithms and Principles of Computer Science (Algoritmi e Principi dell'Informatica) in the academic year 2022/2023 at Polytechnic of Milan.

Subject: 052509 - Prova Finale (Progetto di Algoritmi e Strutture Dati)

Professor: Barenghi Alessandro

Academic Year: 2022/2023

## Description of the Project
The Project is called *CercaPercorso* (Find Route) and it simulates an electric car rental system distributed along a highway with service stations, each located at a unique distance (in km) from the start.
Each station maintains a fleet of up to 512 electric cars, where each car has a fixed autonomy (km range). Given a start and end station, the program must plan a trip with the minimum number of stops. If multiple routes have the same number of stops, the path that prioritizes stations closer to the highway start is chosen.

Supported Commands:
- aggiungi-stazione: Add a station with its cars.
- demolisci-stazione: Remove an existing station.
- aggiungi-auto: Add a car to a station.
- rottama-auto: Remove a car with given autonomy from a station.
- pianifica-percorso: Compute the optimal path between two stations under the problem constraints

The program reads these commands from a text file and prints the required responses (aggiunta, demolita, rottamata, nessun percorso, or the sequence of station distances).

For further details, please review the [full description](Specifications/Description_2022_2023.pdf).

For the open tests used to test locally the provided solution, please refer to the folder [Specifications](Specifications), and to the files Open_Tests_1,2,3,4.

## Algorithm
The program models an **autostrada (highway)** as a set of **stations**, each uniquely identified by its distance from the start. Every station maintains a **sorted list of car autonomies** (maximum range per car).  

The goal of the algorithm is to compute a **trip plan** between two given stations that:  
1. Minimizes the number of stops.  
2. Breaks ties by preferring stations closer to the highway start.  

### Data Structures
- **Hash table of stations**:  
  Stations are stored in buckets based on their distance (`distance % HASH_TABLE_SIZE`). Each bucket is a **sorted linked list** by distance.  
- **Cars per station**:  
  Each station holds up to **512 car autonomies**, stored in a **descending sorted array**.  
- **Path representation**:  
  When planning a trip, all stations between start and end are collected into a **sorted array (`path`)**, which is later traversed using BFS.  

### Trip Planning
Two variants of **Breadth-First Search (BFS)** are used depending on the direction of travel:

- **Forward BFS**:  
  Used when traveling from a smaller distance to a larger one (`start < end`).  
  - Start from the initial station.  
  - Explore reachable stations within the maximum autonomy.  
  - Stop when destination is reached, reconstruct path using predecessors.  

- **Backward BFS**:  
  Used when traveling from a larger distance to a smaller one (`start > end`).  
  - Works symmetrically by moving backward from the destination.  
  - Uses a **two-level queue** to ensure the path with minimum stops and correct lexicographic ordering is selected.  

Both BFS implementations guarantee:  
- **Minimal number of stops** (shortest path in terms of hops).  
- **Tie-breaking**: If multiple paths have equal length, the one with earlier (smaller distance) stations is selected.  

### Command Handling
- **aggiungi-stazione**: Insert a new station at a given distance with its cars (ignored if duplicate).  
- **demolisci-stazione**: Remove a station by distance.  
- **aggiungi-auto**: Insert a car autonomy into a station’s fleet (kept sorted).  
- **rottama-auto**: Remove one car with a given autonomy if it exists.  
- **pianifica-percorso**: Invoke BFS to compute the path.  

### Complexity
- **Station management**:  
  - Insert/remove station: `O(1)` amortized (hash + linked list).  
  - Insert/remove car: `O(512)` worst case (array shift, but capped at 512).  
- **Trip planning**:  
  - BFS traverses each station at most once → `O(N)` where `N` is number of stations between start and end.  
  - Sorting (`quickSort`) is used when stations are spread across hash buckets.

# Final Considerations
Final Mark: 30/30

## Authors
This Project was developed by:
- Francesco Di Giore [@Digioref](https://github.com/Digioref)
