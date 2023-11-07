#pragma once

#include <math.h>
#define PI 3.1415926535

class Complex
{
private:
    double real;
    double imag;

public:
    Complex(double real = 0.0, double imag = 0.0)
    {
        this->real = real;
        this->imag = imag;
    }

    Complex(const Complex &other)
    {
        this->real = other.real;
        this->imag = other.imag;
    }

    ~Complex() {}

    Complex operator+(const Complex &other)
    {
        return Complex(this->real + other.real, this->imag + other.imag);
    }

    Complex operator-(const Complex &other)
    {
        return Complex(this->real - other.real, this->imag - other.imag);
    }

    Complex operator*(const double &scalar)
    {
        return Complex(scalar * this->real, scalar * this->imag);
    }

    // (a+bi)(c+di) = (ac - bd) + (bc + ad)i
    Complex operator*(const Complex &other)
    {
        return Complex(this->real * other.real - this->imag * other.imag, this->real * other.imag + this->imag * other.real);
    }

    Complex operator/(const double &scalar)
    {
        return Complex(scalar / this->real, scalar / this->imag);
    }

    // (a+bi)/(c+di) = ((ac + bd)/(c^2 + d^2)) + ((bc - ad)/(c^2 + d^2))i
    Complex operator/(const Complex &other)
    {
        return Complex((this->real * other.real + this->imag * other.imag) / (other.real * other.real + other.imag * other.imag), (this->imag * other.real - this->real * other.imag) / (other.real * other.real + other.imag * other.imag));
    }

    Complex fromPolar(double radius, double theta)
    {
        return Complex(radius * cos(theta), radius * sin(theta));
    }

    double argument(const Complex &complex)
    {
        if (complex.real == 0.0)
            return complex.imag > 0 ? PI / 2 : 3 * PI / 2;
        return atan(complex.imag / complex.real);
    }

    double argument()
    {
        return argument(*this);
    }

    double modulus(const Complex &complex)
    {
        return sqrt(complex.real * complex.real + complex.imag * complex.imag);
    }

    double modulus()
    {
        return modulus(*this);
    }
};