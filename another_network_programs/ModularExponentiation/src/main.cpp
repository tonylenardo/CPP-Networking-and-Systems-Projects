#include <iostream>
#include <math.h>
#include <vector>
#include <bitset>

std::string converToBinary(long long base10);

int main() {
  std::cout << "Ingrese el numero b" << std::endl;
  long long b = 0;
  std::cin >> b;
  std::cout << "Ingrese la potencia n" << std::endl;
  long long n = 0;
  std::cin >> n;
  std::cout << "Ingrese el modulo m" << std::endl;
  long long m = 0;
  std::cin >> m;
  
  std::vector<long long> exponents;
  std::string binary = converToBinary(n);

  for (long long i = 0; i < binary.size(); ++i) {
    if (binary[i] != '0') {
      exponents.push_back(1 << i);
    } else {
      exponents.push_back(-1);
    }
  }

  std::vector <long long> modulus;
  
  long long exp = b;
  long long mod = 0;
  for (long long i = 0; i < exponents.size(); ++i) {
    mod = exp % m;
    exp = powl(mod, 2);
    if (exponents[i] != -1) {
      modulus.push_back(mod);
    }
  }
  
  for (long long i = 0; i < modulus.size() -1; ++i) {
    long long result = (modulus[i] * modulus[i + 1]) % m;
    modulus[i+1] = result;
  }

  std::cout << b << "^" << n << " mod " << m << " = " << modulus[modulus.size() - 1] << std::endl;

  return EXIT_SUCCESS;
}

std::string converToBinary(long long base10) {
  std::string result = "";
  while (base10 > 0) {
    result += std::to_string(base10 % 2);
    base10 = base10 / 2;
  }
  return result;
}


