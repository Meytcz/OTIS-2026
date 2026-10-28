#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <memory>
#include "models.h"

// Прогоняет одну модель с одним входом n шагов, печатает таблицу и пишет в CSV
void simulate(Model& model, const Input& input, int n, std::ofstream& csv) {
    model.reset();
    std::cout << "\n=== " << model.name() << " | input: " << input.name() << " ===\n";
    std::cout << std::setw(6) << "tau" << std::setw(12) << "u" << std::setw(14) << "y" << "\n";
    std::cout << std::string(32, '-') << "\n";

    for (int tau = 0; tau < n; ++tau) {
        double u = input.value(tau);
        double y = model.next(u);

        std::cout << std::setw(6) << tau
                  << std::setw(12) << std::fixed << std::setprecision(4) << u
                  << std::setw(14) << y << "\n";
        csv << model.name() << "," << input.name() << "," << tau << "," << u << "," << y << "\n";
    }
}

int main() {
    int n;
    std::cout << "Enter number of simulation steps n: ";
    if (!(std::cin >> n) || n <= 0) n = 20;   // если ввели ерунду — 20 шагов

    double a1, b1, b2;
    std::cout << "Model 1.10 coefficients a1 b1 b2 (e.g. 0.7 0.5 0.3): ";
    if (!(std::cin >> a1 >> b1 >> b2)) { a1 = 0.7; b1 = 0.5; b2 = 0.3; }

    LinearModel linear(a1, b1, b2);
    // Advanced Level: проверка устойчивости
    if (!linear.isStable()) {
        std::cout << "\nWARNING: root of characteristic equation z = " << linear.root()
                  << ", |z| >= 1 -> system is UNSTABLE (process will diverge)!\n";
    } else {
        std::cout << "Root z = " << linear.root() << ", |z| < 1 -> system is stable.\n";
    }

    NonlinearModel nonlinear(0.8, 0.5);
    DiffModel diff(0.5, 0.1, 1.0, 0.1);

    std::vector<Model*> models = { &linear, &nonlinear, &diff };
    std::vector<std::unique_ptr<Input>> inputs;
    inputs.push_back(std::make_unique<StepInput>(1.0));
    inputs.push_back(std::make_unique<ImpulseInput>());
    inputs.push_back(std::make_unique<HarmonicInput>());

    std::ofstream csv("results.csv");
    csv << "model,input,tau,u,y\n";
    for (Model* m : models)
        for (auto& in : inputs)
            simulate(*m, *in, n, csv);

    std::cout << "\nResults saved to results.csv\n";
    return 0;
}
