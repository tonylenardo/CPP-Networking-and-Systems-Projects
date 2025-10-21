#include <iostream>
#include <inttypes.h>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <numeric>
#define NUMBER_OF_OCTETS 4


std::string getMask(std::string currentMask, int prefix, int exponent) {

    std::string mask = currentMask;
    int numberBitsTurnedOn = prefix;

    for (int i = 0; i < mask.size(); ++i) {
        if (mask[i] == '0') {
            mask[i] = '1';
            ++numberBitsTurnedOn;
        }
        if (numberBitsTurnedOn == (32 - exponent)) { break; }
    }
    return mask;
}


void setMask(std::string mask, std::vector<std::string>& vectorMask) {

    int index = 0;
    std::string octet = "";

    for (int i = 0; i < mask.size(); ++i) {

        if (mask[i] == '.') {
            vectorMask[index] = octet;
            octet = "";
            ++index;
        }
        else {
            octet += mask[i];
        }
    }
    vectorMask[3] = octet;
}

std::string getCurrentMask(std::string baseNetwork, int prefix) {
    std::string currentMask;
    for (int i = 0; i < 32; ++i) {

        if (i % 8 == 0 && i != 0)
        {
            currentMask += ".";
        }
        if (i < prefix)
        {
            currentMask += "1";
        }
        else
        {
            currentMask += "0";
        }
    }
    return currentMask;
}

int getPrefix(std::string network) {
    int result = 8;
    size_t pos = network.find('.');
    int range = std::atoi(network.substr(0, pos).c_str());

    if (range < 128) { result = 8; }
    else if (range < 191) { result = 16; }
    else { result = 24; }

    return result;
}

void setBaseNetwork(std::string baseNetwork, std::vector<int>& vector) {

    std::string octecValue = "";
    int numberOctet = 0;

    for (int i = 0; i < baseNetwork.size(); ++i) {
        if (baseNetwork[i] == '.' || baseNetwork[i] == '/') {
            vector[numberOctet] = std::atoi(octecValue.c_str());
            octecValue = "";
            ++numberOctet;
        }
        else {
            octecValue += baseNetwork[i];
        }
    }
}

void handleRequest(std::vector<std::tuple<int, std::string>>& requests) {
    std::string request;
    std::string character;
    int numberRequest = 0;
    std::cin.ignore();
    while (std::getline(std::cin, request)) {
        if (request.size() == 0) { break; }
        character = request.substr(0, 1);
        int pos = request.find(', ');
        numberRequest = std::atoi(request.substr(pos, request.size()).c_str());
        requests.emplace_back(numberRequest, character);
    }
}

int getExponent(std::vector<std::tuple<int, std::string>> requests, int numberRequest) {
    int exponent = round(log2(std::get<0>(requests[numberRequest])));
    if ((std::pow(2, exponent) - 2) < std::get<0>(requests[numberRequest])) {
        exponent += 1;
    }
    return exponent;
}

std::string getFirstAddress(std::string network, int order) {
    std::vector<int>vectorNetwork;
    vectorNetwork.resize(NUMBER_OF_OCTETS);

    std::string firstAddres = "";
    std::string octet = "";
    int numberOctet = 0;

    for (int i = 0; i < network.size(); ++i) {
        if (network[i] == '.') {
            vectorNetwork[numberOctet] = std::atoi(octet.c_str());
            octet = "";
            ++numberOctet;
        }
        else {
            octet += network[i];
        }
    }
    vectorNetwork[numberOctet] = std::atoi(octet.c_str());

    if (vectorNetwork[3] < 256) {
        vectorNetwork[3] = vectorNetwork[3] + 1;
    }
    else {
        vectorNetwork[3] = 0;
        for (int i = 2; i >= 0; --i) {

            if (vectorNetwork[i] < 256) {
                vectorNetwork[i] = vectorNetwork[i] + 1;
                break;
            }
            else {
                vectorNetwork[i] = 0;
            }
        }
    }

    for (int i = 0; i < vectorNetwork.size(); ++i) {
        firstAddres += std::to_string(vectorNetwork[i]);
        if (i + 1 < vectorNetwork.size()) {
            firstAddres += ".";
        }
    }

    return firstAddres;
}

std::string getLastAddress(std::string broadcastNetwork) {
    std::vector<int>vectorNetwork;
    vectorNetwork.resize(NUMBER_OF_OCTETS);

    std::string lastAddress = "";
    std::string octet = "";
    int numberOctet = 0;

    for (int i = 0; i < broadcastNetwork.size(); ++i) {
        if (broadcastNetwork[i] == '.') {
            vectorNetwork[numberOctet] = std::atoi(octet.c_str());
            octet = "";
            ++numberOctet;
        }
        else {
            octet += broadcastNetwork[i];
        }
    }
    vectorNetwork[numberOctet] = std::atoi(octet.c_str());


    if (vectorNetwork[3] == 0) {
        vectorNetwork[3] = 254;
        for (int i = 2; i >= 0; --i) {
            if (vectorNetwork[i] != 0) {
                vectorNetwork[i] = vectorNetwork[i] - 1;
                break;
            }
            else {
                vectorNetwork[i] = 255;
            }
        }
    }
    else {
        vectorNetwork[3] = vectorNetwork[3] - 1;
    }

    for (int i = 0; i < vectorNetwork.size(); ++i) {
        lastAddress += std::to_string(vectorNetwork[i]);
        if (i + 1 < vectorNetwork.size()) {
            lastAddress += ".";
        }
    }
    return lastAddress;
}

std::string getBroadcastAddress(std::string nextNetwork, int order) {
    std::vector<int>vectorNetwork;
    vectorNetwork.resize(NUMBER_OF_OCTETS);

    std::string broadcastAddres = "";
    std::string octet = "";
    int numberOctet = 0;

    for (int i = 0; i < nextNetwork.size(); ++i) {
        if (nextNetwork[i] == '.') {
            vectorNetwork[numberOctet] = std::atoi(octet.c_str());
            octet = "";
            ++numberOctet;
        }
        else {
            octet += nextNetwork[i];
        }
    }
    vectorNetwork[numberOctet] = std::atoi(octet.c_str());


    if (vectorNetwork[3] == 0) {
        vectorNetwork[3] = 255;
        for (int i = 2; i >= 0; --i) {
            if (order == 1){
                if (vectorNetwork[i] != 255) {
                    vectorNetwork[i] = vectorNetwork[i] + 1;
                    break;
                }
                else {
                    vectorNetwork[i] = 0;
                }
            } else {
                if (vectorNetwork[i] != 0) {
                    vectorNetwork[i] = vectorNetwork[i] - 1;
                    break;
                }
                else {
                    vectorNetwork[i] = 255;
                }
            }
        }
    }
    else {
        vectorNetwork[3] = vectorNetwork[3] - 1;
    }

    for (int i = 0; i < vectorNetwork.size(); ++i) {
        broadcastAddres += std::to_string(vectorNetwork[i]);
        if (i + 1 < vectorNetwork.size()) {
            broadcastAddres += ".";
        }
    }
    return broadcastAddres;
}

std::vector<int> stringToVector(std::string red) {
    std::vector<int> network;
    std::string temp = "";
    for (int i = 0; i < red.size(); ++i) {
        if (red[i] == '.') {
            network.push_back(std::atoi(temp.c_str()));
            temp = "";
        }
        else {
            temp += red[i];
        }

    }
    network.push_back(std::atoi(temp.c_str()));
    return network;
}

std::string nextNetwork(std::string red, int N, int order) {
    std::string result = "";

    std::vector<int> network = stringToVector(red);

    if (order == 1) {
        if (((network[3] + 1) - N) < 0) {
            network[2] = network[2] + 1;
            if (network[3] == 0) {
                network[3] = 256;
            }

            if (network[3] - N < 0) {
                int sub = std::abs((network[3] - N));
                network[3] = 256 - sub;
            }
            else {
                network[3] = network[3] - N;
            }
        }
        else {
            if (network[3] == 0) {
                network[3] = 256;
            }
            network[3] = network[3] - N;
        }
    }
    else {
        if ((network[3] - N) < 0) {
            network[2] = network[2] - 1;

            if (network[3] - N < 0) {
                int sub = std::abs((network[3] - N));
                network[3] = 256 - sub;
            }
            else {
                network[3] = network[3] - N;
            }
        }
        else {
            if (network[3] - N < 0) {
                int sub = std::abs((network[3] - N));
                network[3] = 256 - sub;
            }
            else {
                network[3] = network[3] - N;
            }
        }
    }

    for (int i = 0; i < network.size(); ++i) {
        result += std::to_string(network[i]);
        if (i < 3) {
            result += ".";
        }
    }
    return result;

}

int sumRequest(std::vector<std::tuple<int, std::string>> requests) {
    int result = 0;
    for (auto a : requests) {
        result += std::get<0>(a);
    }
    return result;
}

int mask(std::vector<std::string> &vectorMask, std::vector<std::tuple<int, std::string>> requests, int numberRequest, int prefix, std::string currentMask) {
    int exponent = getExponent(requests, numberRequest);

    std::string mask = getMask(currentMask, prefix, exponent);
    setMask(mask, vectorMask);

    int maskNumber = 0;
    for (int i = 0; i < mask.size(); ++i) {
        if (mask[i] == '1') {
            ++maskNumber;
        }
    }
    return maskNumber;
}

int getNumberJumps(std::vector<std::string> mask) {
    int jump = 0;
    int lastModificateOctet = 0;
    for (int i = 0; i < mask.size(); ++i) {
        if (mask[i].find('0') != std::string::npos) {
            lastModificateOctet = i;
            break;
        }
    }

    jump = 256 - std::stoi(mask[lastModificateOctet], nullptr, 2);

    return jump;
}


int main() {
    std::cout << "Ingrese la red base - ej. 192.128.24.0\n";
    std::string baseNetwork = "172.16.64.0";
    std::cin >> baseNetwork;

    std::vector <int> vectorBaseNetwork;
    vectorBaseNetwork.resize(NUMBER_OF_OCTETS);

    setBaseNetwork(baseNetwork, vectorBaseNetwork);

    std::cout << "Orden de acomodo:\n"
        << "1.  menor a mayor\n"
        << "2.  mayor a menor\n";
    int order = 2;
    std::cin >> order;

    std::cout << "Ingrese el conjunto de solicitudes - ej. A, 16\n"
        << "\033[33m(para finalizar ingrese una linea en blanco)\033[0m" << std::endl;

    std::vector<std::tuple<int, std::string>> requests;
    handleRequest(requests);


    // Se ordena primeramente alfabeticamente
    std::sort(requests.begin(), requests.end(), [](const auto& a, const auto& b) { return std::get<1>(a) < std::get<1>(b); });
    // Luego se ordena númericamente
    std::sort(requests.begin(), requests.end(), [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });

    printf("%-10s %-20s %-20s %-10s %-20s %-20s\n", "Conjunto", "Direcciones asignables"
        , "", "Máscara", "Dirección Red", "Dirección Broadcast");
    printf("%-10s %-20s %-20s %-10s %-20s %-20s\n", "", "Primera", "Última"
        , "", "", "");

    std::string firstAddress = "192.168.16.1";
    std::string lastAddress = "192.168.16.x";
    std::string networkAddress = baseNetwork;
    std::string broadcastAddress = "192.168.16.y";

    std::string red = baseNetwork;

    std::vector<int>temp = stringToVector(red);
    if (order == 2) {
        int sum = sumRequest(requests);

        while (sum > 256) {
            sum -= 256;
            if (temp[2] < 255) {
                temp[2] = temp[2] + 1;
            }
            else {
                temp[2] = 0;
                temp[1] = temp[1] + 1;
            }
        }
    }
    if (temp[3] == 0) {
        temp[3] = 256;
    }
    red = "";
    for (int i = 0; i < temp.size(); ++i) {
        red += std::to_string(temp[i]);
        if (i < 3) {
            red += ".";
        }
    }

    int prefix = getPrefix(baseNetwork);

    std::string currentMask = getCurrentMask(baseNetwork, prefix);
    std::vector<std::string> vectorMask;
    vectorMask.resize(NUMBER_OF_OCTETS);


    for (int numberRequest = 0; numberRequest < requests.size(); ++numberRequest) {

        int maskNumber = mask(vectorMask, requests, numberRequest, prefix, currentMask);
        int jumps = getNumberJumps(vectorMask);

        networkAddress = nextNetwork(red, jumps, order);
        firstAddress = getFirstAddress(networkAddress, order);
        broadcastAddress = getBroadcastAddress(red, order);
        lastAddress = getLastAddress(broadcastAddress);
        red = networkAddress;


        printf("%-10s: %-20s %-20s", std::get<1>(requests[numberRequest]).c_str()
            , firstAddress.c_str(), lastAddress.c_str());
        printf(" %-10d %-20s %-20s\n", maskNumber, red.c_str()
            , broadcastAddress.c_str());
    }

    return EXIT_SUCCESS;
}

