#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <iomanip>


void desencriptar(const std::vector<std::string>& message_encripted, const std::vector<int>& key) {
    int size_key = key.size();

    std::vector<std::string> matriz;
    for (int i = 0; i < message_encripted.size(); ++i) {
        matriz.push_back(message_encripted[key[i]]);
    }

    int count = 0;
    for (int i = 0; i < matriz[count].size(); ++i) {
        for (int j = 0; j < matriz.size(); ++j) {
            std::cout << matriz[j][i];
        }
        if (count >= matriz.size()) { break; }
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "Mensaje a Encriptar" << std::endl;
    std::string message = "Me encanta tomar café, sobre todo cuando es temprano. Es como una pausa antes de empezar"
        "el día, un momento solo para mí.No necesito que sea algo especial, con que esté caliente"
        "y fuerte es suficiente.A veces lo disfruto con un poco de pan o simplemente lo tomo solo,"
        "disfrutando el olor y el sabor.No sé si me despierta realmente o si es solo costumbre,"
        "pero siempre siento que el día empieza mejor con una taza de café.";

    std::cout << message << std::endl;

    std::string clave = "hiperblanduzco";

    std::vector <std::string> key;
    key.resize(clave.size());

    for (int i = 0; i < clave.size(); ++i) {
        key[i] = clave[i];
    }

    std::vector <std::string> orderKey(key);
    std::sort(orderKey.begin(), orderKey.end());

    std::vector<std::tuple<std::string, int>> tuplas;
    for (int i = 0; i < key.size(); ++i) {
        int pos = 0;
        for (int j = 0; j < orderKey.size(); ++j) {
            if (key[i] == orderKey[j]) {
                pos = j;
                break;
            }
        }
        tuplas.push_back(std::make_tuple(key[i], pos));
    }

    int count = 0;
    std::string text = "";
    std::vector<std::string> vectorMessage;


    for (int i = 0; i < message.size(); ++i) {

        ++count;
        text += std::tolower(message[i]);
        if (count == clave.size()) {
            count = 0;
            vectorMessage.push_back(text);
            text = "";
        }
    }
    if (text.size() < clave.size()) {
        vectorMessage.push_back(text);
    }

    std::vector <std::string> messageEncripted;
    std::string temp = "";
    int index = 0;
    int k = 0;
    for (int j = 0; tuplas.size() * tuplas.size(); ++j) {
        if (std::get<1>(tuplas[k]) == index) {
            for (int i = 0; i < vectorMessage.size(); ++i) {
                if (vectorMessage[i].size() < k) {
                    break;
                }
                else {
                    temp += vectorMessage[i][k];
                }
            }
            messageEncripted.push_back(temp);
            ++index;
            k = 0;
            temp = "";
        }
        else {
            ++k;
        }
        if (index == tuplas.size()) {
            break;
        }
    }
    //messageEncripted.resize(message.size());

    std::cout << "Mensaje Encriptado con la clave \"" << clave << "\"" << std::endl << std::endl;


    for (auto c : messageEncripted) {
        std::cout << c;
    }

    std::cout << "\n\nMatriz de transposición" << std::endl << std::endl;

    for (int i = 0; i < clave.size(); ++i) {
        std::cout << std::get<0>(tuplas[i]) << "  ";
    }
    std::cout << std::endl;

    for (int i = 0; i < clave.size(); ++i) {
        std::cout << std::setfill('0') << std::setw(2) << std::get<1>(tuplas[i]) << " ";
    }
    std::cout << std::endl;

    std::string line = "";
    int i = 0;
    for (char c : message) {
        line += c;
        line += "  ";
        i++;
        if (i == clave.size()) {
            std::cout << line << std::endl;
            i = 0;
            line = "";
        }
    }
    std::cout << line << std::endl;

    std::cout << "\nMensaje Desencriptado" << std::endl << std::endl;

    std::vector<int> llave;
    for (int i = 0; i < tuplas.size(); ++i) {
        llave.push_back(std::get<1>(tuplas[i]));
    }

    desencriptar(messageEncripted, llave);

    return EXIT_SUCCESS;
}



