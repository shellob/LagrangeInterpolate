#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <ctime>
#include <cstdlib>


using namespace std;

double lagrange_interpolation(const vector<double>& Y, int i, int j, int k, double t) {
    double Li = (t - j) * (t - k) / ((i - j) * (i - k));
    double Lj = (t - i) * (t - k) / ((j - i) * (j - k));
    double Lk = (t - i) * (t - j) / ((k - i) * (k - j));
    return Li * Y[i] + Lj * Y[j] + Lk * Y[k];
}

vector<double> densify_table(const vector<double>& Y, int& IER) {
    int N = Y.size();
    if (N <= 3) {
        IER = 1;
        throw runtime_error("Уплотнённый вектор не может быть построен из-за малости N (N <= 3)");
    }

    IER = 0;
    vector<double> YY;
    YY.reserve(2 * N - 1);

    for (int i = 0; i < N - 1; ++i) {
        YY.emplace_back(Y[i]);
        YY.emplace_back(lagrange_interpolation(Y, i, i + 1, min(i + 2, N - 1), i + 0.5));
    }
    YY.emplace_back(Y.back());

    return YY;
}

double true_function(double x) {
    return x * x; // Пример истинной функции (квадратичная функция)
}

vector<double> generate_random_data(int N) {
    vector<double> data(N);

    for (int i = 0; i < N; ++i) {
        data[i] = i * i + static_cast<double>(rand()) / RAND_MAX * 0.2 - 0.1;
    }
    sort(data.begin(), data.end());

    for (int i = 0; i < N; ++i) {
        data[i] = true_function(data[i]);
    }

    return data;
}

vector<double> compute_reverse_values(const vector<double>& Y, const vector<double>& YY) {
    int N = Y.size();
    vector<double> reverse_Y(N);

    for (int i = 0; i < N - 1; ++i) {
        reverse_Y[i] = lagrange_interpolation(YY, 2 * i, 2 * i + 1, 2 * i + 2, 2 * i);
    }
    reverse_Y[N - 1] = YY[2 * N - 2];

    return reverse_Y;
}

double max_error(const vector<double>& Y, const vector<double>& reverse_Y) {
    int N = Y.size();
    double max_err = 0;

    for (int i = 0; i < N; ++i) {
        double err = abs(Y[i] - reverse_Y[i]);
        max_err = max(max_err, err);
    }

    return max_err;
}

void print_table(const vector<double>& Y, const vector<double>& YY, const vector<double>& reverse_Y) {
    int N = Y.size();
    int M = YY.size();

    cout << setw(15) << "Исходная таблица" << setw(15) << "Уплотненная таблица" << setw(15) << "Обратные значения" << setw(15) << "Погрешность" << endl;
    cout << "----------------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < M; ++i) {
        if (i % 2 == 0) {
            cout << setw(15) << Y[i / 2];
        }
        else {
            cout << setw(15) << "-";
        }

        cout << setw(15) << YY[i];

        if (i % 2 == 0) {
            cout << setw(15) << reverse_Y[i / 2];
            double error = abs(Y[i / 2] - reverse_Y[i / 2]);
            cout << setw(15) << error << endl;
        }
        else {
            cout << setw(15) << "-" << setw(15) << "-" << endl;
        }
    }
}

void test(const vector<double>& Y) {
    int IER;

    try {
        vector<double> YY = densify_table(Y, IER);
        vector<double> reverse_Y = compute_reverse_values(Y, YY);
        double max_err = max_error(Y, reverse_Y);
        print_table(Y, YY, reverse_Y);
        cout << "Наибольшее отклонение: " << max_err << endl;
    }
    catch (const runtime_error& e) {
        cout << "Ошибка: " << e.what() << endl;
    }
}

int main() {
    setlocale(LC_ALL, "rus");
    srand(static_cast<unsigned>(time(0)));
    // Заданные значения
    vector<double> Y = { 1, 8, 27, 64, 125 };
    cout << "Тест с заданными значениями:" << endl;
    test(Y);

    cout << endl;

    // Случайные значения
    int N = 10;
    vector<double> random_Y = generate_random_data(N);
    cout << "Тест со случайными значениями:" << endl;
    test(random_Y);

    return 0;
}