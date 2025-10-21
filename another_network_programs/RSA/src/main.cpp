#include <iostream>
#include <math.h>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <string>
#include <cstdint>
#include <fstream>
#include <algorithm>
#include <limits>

long long extendedEuclid(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long gcd = extendedEuclid(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

// Algoritmo extendido de Euclides, recuperado de: 
// https://es.wikipedia.org/wiki/Algoritmo_de_Euclides
long long modularReverse(long long k, long long z) {
    long long x, y;
    long long gcd = extendedEuclid(k, z, x, y);
    if (gcd != 1) {
        return -1;  // No existe inverso modular si el GCD no es 1
    }
    return (x % z + z) % z;
}

// Criba de Eratostenes; Pseudocodigo recuperado de: 
// https://es.wikipedia.org/wiki/Criba_de_Erat%C3%B3stenes
void primeNumberGenerator(long long limit, long long& p, long long& q) {
    std::srand(std::time(0));
    std::vector<long long> numMarked(limit, false);
    numMarked[0] = true;
    numMarked[1] = true;

    long long n = std::sqrt(limit);
    for (long long i = 2; i <= n; ++i) {
        if (!numMarked[i]) {
            for (long long j = i * i; j < limit; j += i) {
                numMarked[j] = true;
            }
        }
    }

    std::vector<long long> primeNumber;
    long long countPrimeNumbers = 0;
    for (long long i = 1000000; i < limit; ++i) {
        if (!numMarked[i]) {
            primeNumber.push_back(i);
            ++countPrimeNumbers;
        }
    }
    primeNumber.resize(countPrimeNumbers);
    long long firstValue = std::rand() % (primeNumber.size() - 2) + 1;
    p = primeNumber[firstValue];
    q = primeNumber[std::rand() % (firstValue - 1)];
}

// Algoritmo de Euclides, para MCD, recuperado de:
//  https://es.wikipedia.org/wiki/Algoritmo_de_Euclides
long long MCD(long long a, long long b) {
    while (b != 0) {
        long long c = b;
        b = a % b;
        a = c;
    }
    return a;
}


std::string converToBinary(long long base10) {
    std::string result = "";
    while (base10 > 0) {
        result += std::to_string(base10 % 2);
        base10 = base10 / 2;
    }

    return result;
}


// Adaptación de suma modular, Autor Chris Dodd 2018, recuperado de: https://stackoverflow.com/q/52063315
uint64_t modularAdd(uint64_t a, uint64_t b, uint64_t m) {
    if (a >= m) { a %= m; }
    if (b >= m) { b %= m; }

    a += b;
    if (a >= m || a < b) { a -= m; };
    return a;
}

// Adaptación de Multiplicacion modular, Autor KAKAK 2018, recuperado de: https://stackoverflow.com/q/52063315
uint64_t modularMultiplication(uint64_t a, uint64_t b, uint64_t m) {

    if (a == 0 || b < m / a) {  return ((uint64_t)a * b) % m; }
    uint64_t sum = 0;
    while (b > 0) {
        if (b & 1) { sum = modularAdd(sum, a, m); }
            
        a = (2 * a) % m;
        b >>= 1;
    }
    return sum;
}


// Adaptación de Exponenciación modular, Autor KAKAK 2018, recuperado de: https://stackoverflow.com/q/52063315
uint64_t modularExponentation(uint64_t a, uint64_t b, uint64_t m) {

    uint64_t result, exp;
    result = 1;
    exp = a % m;
    while (b > 0) {
        if (b & 1)
            result = modularMultiplication(result, exp, m);
        exp = modularMultiplication(exp, exp, m);
        b >>= 1;
    }
    return result;
}

int main() {
    std::srand(std::time(0));

    std::cout << "Texto a cifrar: " << std::endl;
    std::string message = "";
    
    std::ifstream textToEncrypt("src/msg.txt");
    if (!textToEncrypt.is_open()) {
        std::cerr << "ERROR: could not open file" << std::endl;
        return EXIT_FAILURE;
    }
    std::string line;

    while (std::getline(textToEncrypt, line)) {
        message += line + '\n';
    }

    std::cout << message << std::endl;

    textToEncrypt.close();
    
    //Generar llaves
    long long p, q;
    primeNumberGenerator(10000000, p, q);
    long long n = p * q;
    long long z = (p - 1) * (q - 1);

    int k = 65537;
    while (k < z) {
        if (MCD(z, k) == 1) {
            break;
        }
        ++k;
    }

    std::cout << "Números primos calculados: P = " << p << ", Q = " << q << std::endl;
    std::cout << "z = " << z << std::endl;
    std::cout << "La llave publica es: k = " << k << " y n = " << n << std::endl;

    long long j = modularReverse(k, z);


    std::cout << "La llave privada es: j = " << j << std::endl;

    std::cout << "Mensaje codificado: \n";
    // Se encripta el mensaje;
    std::vector<uint64_t> encryptText;
    for (int i = 0; i < message.size(); ++i) {
        long long M = static_cast<uint64_t>(message[i]);
        uint64_t c = modularExponentation(M, k, n);
        std::cout << static_cast<char>(c);
        encryptText.push_back(c);
    }
    std::cout << "\n";

    std::cout << "Mensaje descodificado: \n";
    // Se desencripta;
    std::string decryptedText = "";
    for (int i = 0; i < encryptText.size(); ++i) {
        char c = static_cast<char>(modularExponentation(encryptText[i], j, n));
        decryptedText += c;
    }
    std::cout << decryptedText << std::endl;


    return EXIT_SUCCESS;
}