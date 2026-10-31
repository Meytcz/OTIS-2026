#pragma once
#include <cmath>
#include <string>



// Базовый абстрактный класс входного сигнала
class Input {
public:
    virtual ~Input() = default;
    virtual double value(int tau) const = 0;   // значение u в момент tau
    virtual std::string name() const = 0;
};

// Ступенчатое: u(tau) = const
class StepInput : public Input {
    double c;
public:
    explicit StepInput(double c = 1.0) : c(c) {}
    double value(int) const override { return c; }
    std::string name() const override { return "step"; }
};

// Импульсное: u(0) = 1, u(tau > 0) = 0
class ImpulseInput : public Input {
public:
    double value(int tau) const override { return tau == 0 ? 1.0 : 0.0; }
    std::string name() const override { return "impulse"; }
};

// Гармоническое: u(tau) = sin(tau)
class HarmonicInput : public Input {
public:
    double value(int tau) const override { return std::sin(static_cast<double>(tau)); }
    std::string name() const override { return "harmonic"; }
};



// Базовый абстрактный класс модели
class Model {
protected:
    double y;        // текущее значение y(tau)
    double y0;       // начальное значение
public:
    explicit Model(double y0 = 0.0) : y(y0), y0(y0) {}
    virtual ~Model() = default;
    // Рассчитать следующий шаг y(tau+1) по входу u(tau)
    virtual double next(double u) = 0;
    virtual std::string name() const = 0;
    virtual void reset() { y = y0; }
    double current() const { return y; }
};

// Model 1.10 (Distributed Control Window Model):
// y(tau+1) = a1*y(tau) + b1*u(tau) + b2*u(tau-2)
class LinearModel : public Model {
    double a1, b1, b2;
    double u1 = 0.0, u2 = 0.0;   // u(tau-1), u(tau-2)
public:
    LinearModel(double a1, double b1, double b2, double y0 = 0.0)
        : Model(y0), a1(a1), b1(b1), b2(b2) {}
    double next(double u) override {
        y = a1 * y + b1 * u + b2 * u2;
        u2 = u1;     // сдвигаем историю входа
        u1 = u;
        return y;
    }
    void reset() override { Model::reset(); u1 = u2 = 0.0; }
    std::string name() const override { return "Model 1.10 (linear)"; }

    // Анализ устойчивости: характеристическое уравнение z - a1 = 0, корень z = a1.
    // Система устойчива, если |z| < 1.
    bool isStable() const { return std::fabs(a1) < 1.0; }
    double root() const { return a1; }
};

// Model 2.4 (Logarithmic Multi-Variable Interaction):
// y(tau+1) = a*y(tau)*cos(y(tau)) + b*ln(1 + |u(tau)|)
class NonlinearModel : public Model {
    double a, b;
public:
    NonlinearModel(double a, double b, double y0 = 0.0) : Model(y0), a(a), b(b) {}
    double next(double u) override {
        y = a * y * std::cos(y) + b * std::log(1.0 + std::fabs(u));
        return y;
    }
    std::string name() const override { return "Model 2.4 (nonlinear)"; }
};

// Model 3.8 (Combined Linear-Quadratic Decay), метод Эйлера:
// dy/dt = -a1*y - a2*y^2 + b*u   =>   y(tau+1) = y + dt * (-a1*y - a2*y^2 + b*u)
class DiffModel : public Model {
    double a1, a2, b, dt;
public:
    DiffModel(double a1, double a2, double b, double dt, double y0 = 0.0)
        : Model(y0), a1(a1), a2(a2), b(b), dt(dt) {}
    double next(double u) override {
        y = y + dt * (-a1 * y - a2 * y * y + b * u);
        return y;
    }
    std::string name() const override { return "Model 3.8 (Euler)"; }
};
