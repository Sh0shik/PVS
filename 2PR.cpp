// Практическая работа 2. Распараллеливание циклов в OpenMP.
// Маслаков Савелий, КИ24-07Б. Вариант 19-1-3-28-19-28.
//
// A:  a[0] = 0;     a[i] = 10*x + a[i - 1].
// B:  b[0] = x;     b[i] = sin(x/i) + cos(i/N) + x/(N/10 - i).
// C:  c[0] = 1;     c[i] = b[i]*N - b[N - i].
// D:  d[0] = x;     d[i] = a[i - 1] + i - N.
// B1: b[0] = b[0];  b[i] = a[i]/i + c[i]/N.
// D1: d[0] = x;     d[i] = cos(a[i]/i) + c[i]/i.

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <omp.h>

using namespace std;

void write_arrays(ofstream& f, const char* title,
                  double* a, double* b, double* c, double* d, int N)
{
    f << title << '\n';
    f << setw(6) << "i" << setw(18) << "a[i]" << setw(18) << "b[i]"
      << setw(18) << "c[i]" << setw(18) << "d[i]" << '\n';
    for (int i = 0; i <= N; ++i)
        f << setw(6) << i << setw(18) << a[i] << setw(18) << b[i]
          << setw(18) << c[i] << setw(18) << d[i] << '\n';
    f << '\n';
}

// N — последний индекс. t — времена A, B, C, D, B1, D1 и Total.
void compute(int N, int L, double x, bool parallel, ofstream& file, double t[])
{
    double* a = new double[N + 1];
    double* b = new double[N + 1];
    double* c = new double[N + 1];
    double* d = new double[N + 1];

    if (file.is_open())
        file << fixed << setprecision(6)
             << "N = " << N << ", L = " << L << ", X = " << x << "\n\n";

    if (parallel) {
        // Параллельный расчет. A выполняется обычным циклом.
        // A: текущий элемент зависит от предыдущего.
        double start = omp_get_wtime();
        a[0] = 0;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = 10 * x + a[i - 1];
            a[i] = tmp;
        }
        t[0] = omp_get_wtime() - start;

        // B: рассчитываем исходные значения.
        start = omp_get_wtime();
        b[0] = x;
        #pragma omp parallel for schedule(runtime)
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j) {
                double den = N / 10.0 - i;
                if (den == 0)
                    den = 1;  // Как в ПР1.
                tmp = sin(x / i) + cos((double)i / N) + x / den;
            }
            b[i] = tmp;
        }
        t[1] = omp_get_wtime() - start;

        // C: читает уже вычисленный B.
        start = omp_get_wtime();
        c[0] = 1;
        #pragma omp parallel for schedule(runtime)
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = b[i] * N - b[N - i];
            c[i] = tmp;
        }
        t[2] = omp_get_wtime() - start;

        // D: читает уже вычисленный A.
        start = omp_get_wtime();
        d[0] = x;
        #pragma omp parallel for schedule(runtime)
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = a[i - 1] + i - N;
            d[i] = tmp;
        }
        t[3] = omp_get_wtime() - start;

        if (file.is_open())
            write_arrays(file, "BEFORE redefinition:", a, b, c, d, N);

        // B1: переопределяем B после расчета C.
        start = omp_get_wtime();
        #pragma omp parallel for schedule(runtime)
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = a[i] / i + c[i] / N;
            b[i] = tmp;
        }
        t[4] = omp_get_wtime() - start;

        // D1: переопределяем D.
        start = omp_get_wtime();
        d[0] = x;
        #pragma omp parallel for schedule(runtime)
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = cos(a[i] / i) + c[i] / i;
            d[i] = tmp;
        }
        t[5] = omp_get_wtime() - start;
    } else {
        // Последовательный расчет. Все циклы обычные.
        // A: текущий элемент зависит от предыдущего.
        double start = omp_get_wtime();
        a[0] = 0;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = 10 * x + a[i - 1];
            a[i] = tmp;
        }
        t[0] = omp_get_wtime() - start;

        // B: рассчитываем исходные значения.
        start = omp_get_wtime();
        b[0] = x;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j) {
                double den = N / 10.0 - i;
                if (den == 0)
                    den = 1;  // Как в ПР1.
                tmp = sin(x / i) + cos((double)i / N) + x / den;
            }
            b[i] = tmp;
        }
        t[1] = omp_get_wtime() - start;

        // C: читает уже вычисленный B.
        start = omp_get_wtime();
        c[0] = 1;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = b[i] * N - b[N - i];
            c[i] = tmp;
        }
        t[2] = omp_get_wtime() - start;

        // D: читает уже вычисленный A.
        start = omp_get_wtime();
        d[0] = x;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = a[i - 1] + i - N;
            d[i] = tmp;
        }
        t[3] = omp_get_wtime() - start;

        if (file.is_open())
            write_arrays(file, "BEFORE redefinition:", a, b, c, d, N);

        // B1: переопределяем B после расчета C.
        start = omp_get_wtime();
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = a[i] / i + c[i] / N;
            b[i] = tmp;
        }
        t[4] = omp_get_wtime() - start;

        // D1: переопределяем D.
        start = omp_get_wtime();
        d[0] = x;
        for (int i = 1; i <= N; ++i) {
            double volatile tmp = 0;
            for (int j = 0; j < L; ++j)
                tmp = cos(a[i] / i) + c[i] / i;
            d[i] = tmp;
        }
        t[5] = omp_get_wtime() - start;
    }

    if (file.is_open())
        write_arrays(file, "AFTER redefinition:", a, b, c, d, N);

    t[6] = t[0] + t[1] + t[2] + t[3] + t[4] + t[5];

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
}

// Отдельный короткий цикл показывает исходные настройки OpenMP.
void show_default_distribution(int N)
{
    int* owner = new int[N + 1];
    int threads = 0;
    #pragma omp parallel
    {
        #pragma omp master
        threads = omp_get_num_threads();

        #pragma omp for
        for (int i = 1; i <= N; ++i)
            owner[i] = omp_get_thread_num();
    }

    cout << "Default threads: " << threads << '\n';
    for (int first = 1; first <= N;) {
        int last = first;
        while (last < N && owner[last + 1] == owner[first])
            ++last;
        cout << "Thread " << owner[first] << ": i = " << first << ".." << last << '\n';
        first = last + 1;
    }
    delete[] owner;
}

void print_time(const char* label, const double t[], double seq_total = 0)
{
    cout << left << setw(7) << label << right;
    for (int i = 0; i < 7; ++i)
        cout << setw(11) << t[i];
    if (seq_total > 0 && t[6] > 0)
        cout << setw(11) << seq_total / t[6];
    cout << endl;
}

int main()
{
    int N;
    cout << "Last index N (0 = check arrays): ";
    if (!(cin >> N) || N < 0 || N == INT_MAX)
        return 1;

    int L = 100;
    int threads = 0;
    int policy = 1;
    int chunk = 0;
    int runs = 1;
    int save = 1;

    if (N == 0) {
        int cores = omp_get_num_procs();
        int Ntest = 100 * cores + cores / 2;
        N = Ntest - 1;
        cout << "Check: " << Ntest << " elements, N = " << N << ", L = " << L << '\n';
        show_default_distribution(N);
    } else {
        cout << "L = ";
        if (!(cin >> L) || L < 1)
            return 1;
        cout << "Threads (0 = default): ";
        if (!(cin >> threads) || threads < 0)
            return 1;
        cout << "Schedule (1 = static, 2 = dynamic, 3 = guided): ";
        if (!(cin >> policy) || policy < 1 || policy > 3)
            return 1;
        if (policy == 1)
            cout << "Chunk (0 = contiguous ranges): ";
        else
            cout << "Chunk (0 = default size 1): ";
        if (!(cin >> chunk) || chunk < 0)
            return 1;
        cout << "Parallel runs (1 or 5): ";
        if (!(cin >> runs) || (runs != 1 && runs != 5))
            return 1;
        cout << "Save arrays? (1 = yes, 0 = no): ";
        if (!(cin >> save) || (save != 0 && save != 1))
            return 1;
    }

    double x;
    cout << "X = ";
    if (!(cin >> x) || !isfinite(x))
        return 1;

    if (threads > 0) {
        omp_set_dynamic(0);
        omp_set_num_threads(threads);
    }
    omp_sched_t schedules[] = {omp_sched_static, omp_sched_dynamic, omp_sched_guided};
    omp_set_schedule(schedules[policy - 1], chunk);

    ofstream seq_file, par_file;
    if (save) {
        seq_file.open("arrays_seq.txt");
        par_file.open("arrays_par.txt");
        if (!seq_file || !par_file) {
            cerr << "Could not open array output files.\n";
            return 1;
        }
    }

    double seq[7], par[7], sum[7] = {};
    compute(N, L, x, false, seq_file, seq);

    cout << "\nN = " << N << ", L = " << L << ", X = " << x << '\n';
    cout << fixed << setprecision(6) << "Time, seconds\n";
    cout << "Run              A          B          C          D         B1         D1"
            "      Total    Speedup\n";
    print_time("Seq", seq);

    for (int run = 0; run < runs; ++run) {
        compute(N, L, x, true, par_file, par);
        print_time("Par", par, seq[6]);
        for (int i = 0; i < 7; ++i)
            sum[i] += par[i];

        // Массивы достаточно сохранить после первого параллельного запуска.
        if (par_file.is_open())
            par_file.close();
    }
    if (runs > 1) {
        for (int i = 0; i < 7; ++i)
            sum[i] /= runs;
        print_time("Mean", sum, seq[6]);
    }

    if (save) {
        seq_file.close();
        if (!seq_file || !par_file) {
            cerr << "Could not write array output files.\n";
            return 1;
        }
        cout << "Saved arrays_seq.txt and arrays_par.txt\n";
    }
}
