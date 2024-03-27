#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

void random_matrix(vector<vector<int>> &matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10; // Generates random numbers between 0 and 9
        }
    }
}

void multiplication_mat(const vector<vector<int>> &A, const vector<vector<int>> &B, vector<vector<int>> &C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            C[i][j] = 0;
            for (int k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int size = 0;
    cout << "Enter size of the matrix - ";
    cin >> size;

    cout << "Performing calculation of matrix sized - " << size << "X" << size << endl;
    vector<vector<int>> A(size, vector<int>(size));
    vector<vector<int>> B(size, vector<int>(size));
    vector<vector<int>> C(size, vector<int>(size));

    srand(time(NULL));

    random_matrix(A, size);
    random_matrix(B, size);

    auto start_time = chrono::high_resolution_clock::now();
    multiplication_mat(A, B, C, size);
    auto end_time = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "The execution time of multiplication: " << duration.count() << " microseconds" << endl;

    return 0;
}
