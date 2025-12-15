// (поиск расстояний, взвешенный граф)
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <locale>
#include <limits>
#include <iomanip>
#include <queue>

using namespace std;

void clearScreen();
int isInteger(const string& message);
void bfsd(int** G, int numG, int** GD, int s);
void printMatrix(int** Matrix, int numG);
void analyzeDistances(int numG, int** GD, int* ecc);

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Rus");
    clearScreen();
    srand(time(NULL));

    int** G = nullptr;
    int** GD = nullptr;
    int numG = 0;
    int* ecc = nullptr;

    bool useCommandLine = false;
    bool isWeighted = false;
    bool isOriented = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-weighted") == 0 && i + 1 < argc) {
            isWeighted = (atoi(argv[i + 1]) != 0);
            useCommandLine = true;
            i++;
        } else if (strcmp(argv[i], "-oriented") == 0 && i + 1 < argc) {
            isOriented = (atoi(argv[i + 1]) != 0);
            useCommandLine = true;
            i++;
        }
    }

    if (useCommandLine) {
        cout << "Режим: командная строка\n";
        cout << "Взвешенный: " << (isWeighted ? "да" : "нет") << "\n";
        cout << "Ориентированный: " << (isOriented ? "да" : "нет") << "\n\n";
    }

    numG = isInteger("\nВведите количество вершин графа: ");
    while (numG <= 0) {
        cout << "Ошибка! Количество вершин должно быть положительным\n";
        numG = isInteger("Введите количество вершин графа: ");
    }

    ecc = (int*)malloc(numG * sizeof(int));
    G = (int**)malloc(sizeof(int*) * numG);
    GD = (int**)malloc(sizeof(int*) * numG);

    for (int i = 0; i < numG; i++) {
        G[i] = (int*)malloc(numG * sizeof(int));
        GD[i] = (int*)malloc(numG * sizeof(int));
    }

    if (!useCommandLine) {
        cout << "\nТип графа:\n";
        cout << "1 — Ориентированный\n";
        cout << "2 — Неориентированный\n";
        int orientChoice = isInteger("Выберите тип (1 или 2): ");
        while (orientChoice != 1 && orientChoice != 2) {
            cout << "Ошибка! Введите 1 или 2\n";
            orientChoice = isInteger("Выберите тип (1 или 2): ");
        }
        isOriented = (orientChoice == 1);

        cout << "\nВзвешенный граф?\n";
        cout << "1 — Да\n";
        cout << "0 — Нет\n";
        int weightChoice = isInteger("Выберите (1 или 0): ");
        while (weightChoice != 0 && weightChoice != 1) {
            cout << "Ошибка! Введите 0 или 1\n";
            weightChoice = isInteger("Выберите (1 или 0): ");
        }
        isWeighted = (weightChoice == 1);
    }

    for (int i = 0; i < numG; i++) {
        for (int j = 0; j < numG; j++) {
            if (i == j) {
                G[i][j] = 0;
            } else {
                int hasEdge = rand() % 2;
                if (hasEdge) {
                    if (isWeighted) {
                        G[i][j] = rand() % 10 + 1;
                    } else {
                        G[i][j] = 1;
                    }
                } else {
                    G[i][j] = 0;
                }

                if (!isOriented && i < j) {
                    G[j][i] = G[i][j];
                }
            }
        }
    }

    cout << "\nМатрица смежности: \n";
    printMatrix(G, numG);

    cout << "\nМатрица расстояний: \n";
    for (int i = 0; i < numG; i++) {
        bfsd(G, numG, GD, i);
    }
    printMatrix(GD, numG);

    cout << "\nВектор эксцентриситета: \n";
    for (int i = 0; i < numG; i++) {
        ecc[i] = 0;
        for (int j = 0; j < numG; j++) {
            if (GD[i][j] != -1 && GD[i][j] > ecc[i]) {
                ecc[i] = GD[i][j];
            }
        }
        cout << std::setw(3) << ecc[i] << "\n";
    }

    analyzeDistances(numG, GD, ecc);

    for (int i = 0; i < numG; i++) {
        free(G[i]);
        free(GD[i]);
    }
    free(G);
    free(GD);
    free(ecc);

    return 0;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int isInteger(const string& message) {
    int value;
    while (true) {
        cout << message;
        if (!(cin >> value)) {
            cout << "Ошибка: введено не число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (cin.peek() != '\n') {
            cout << "Ошибка: введено не целое число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        return value;
    }
}

void bfsd(int** G, int numG, int** GD, int s) {
    queue<int> q;
    int v;
    int* distance = (int*)malloc(numG * sizeof(int));

    for (int i = 0; i < numG; i++) {
        distance[i] = -1;
    }

    distance[s] = 0;
    q.push(s);

    while(!q.empty()) {
        v = q.front();
        q.pop();

        for (int i = 0; i < numG; i++) {
            if (G[v][i] > 0 && distance[i] == -1) {
                q.push(i);
                distance[i] = distance[v] + G[v][i];
            }
        }
    }
    for (int i = 0; i < numG; i++) {
        GD[s][i] = distance[i];
    }

    free(distance);
}

void printMatrix(int** Matrix, int numG){
    for (int i = 0; i < numG; i++) {
        for (int j = 0; j < numG; j++) {
            cout << std::setw(4) << Matrix[i][j];
        }
        cout << "\n";
    }
}

void analyzeDistances(int numG, int** GD, int* ecc) {
    int radius = numeric_limits<int>::max();
    int diameter = numeric_limits<int>::min();

    for (int i = 0; i < numG; i++) {
        if (ecc[i] < radius) radius = ecc[i];
        if (ecc[i] > diameter) diameter = ecc[i];
    }

    cout << "\n--- АНАЛИЗ ГРАФА ---\n";
    cout << "Радиус графа: " << radius << endl;
    cout << "Диаметр графа: " << diameter << endl;

    cout << "\nЦентральные вершины: ";
    for (int i = 0; i < numG; i++) {
        if (ecc[i] == radius) cout << i << " ";
    }
    cout << endl;

    cout << "Периферийные вершины: ";
    for (int i = 0; i < numG; i++) {
        if (ecc[i] == diameter) cout << i << " ";
    }
    cout << endl;
}