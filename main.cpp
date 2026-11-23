#include <iostream>
#include <string>
using namespace std;

class CaesarCipher {
    int key;

public:
    CaesarCipher(int k) {
        key = (k % 26 + 26) % 26;
    }

    string encrypt(string msg) {
        for (int i = 0; i < msg.length(); i++) {
            if (msg[i] >= 'A' && msg[i] <= 'Z')
                msg[i] = 'A' + (msg[i] - 'A' + key) % 26;
            else if (msg[i] >= 'a' && msg[i] <= 'z')
                msg[i] = 'a' + (msg[i] - 'a' + key) % 26;
        }
        return msg;
    }

    string decrypt(string msg) {
        CaesarCipher temp(26 - key);
        return temp.encrypt(msg);
    }
};

void bruteForce(string msg) {
    int bestKey = 0, bestScore = -1;

    cout << "\n--- BRUTE FORCE RESULTS ---\n";

    for (int k = 0; k < 26; k++) {
        CaesarCipher c(k);
        string result = c.decrypt(msg);
        int score = 0;

        for (int i = 0; i < result.length(); i++) {
            char ch = tolower(result[i]);
            if (ch == 'e' || ch == 't' || ch == 'a' ||
                ch == 'o' || ch == 'i' || ch == 'n')
                score++;
        }

        cout << "Key " << k << ": " << result << endl;

        if (score > bestScore) {
            bestScore = score;
            bestKey = k;
        }
    }

    CaesarCipher best(bestKey);

    cout << "\nBest Guess: " << best.decrypt(msg);
    cout << "\nKey: " << bestKey << endl;
}

int main() {
    int choice, key;
    string msg;

    do {
        cout << "\n===== CAESAR CIPHER =====\n";
        cout << "1. Encrypt\n";
        cout << "2. Decrypt\n";
        cout << "3. Brute Force\n";
        cout << "4. About\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
            case 2: {
                cout << "Enter message: ";
                getline(cin, msg);

                cout << "Enter shift key: ";
                cin >> key;
                cin.ignore();

                CaesarCipher c(key);

                if (choice == 1)
                    cout << "Encrypted: " << c.encrypt(msg);
                else
                    cout << "Decrypted: " << c.decrypt(msg);

                cout << endl;
                break;
            }

            case 3:
                cout << "Enter encrypted message: ";
                getline(cin, msg);
                bruteForce(msg);
                break;

            case 4:
                cout << "\nCaesar Cipher is a substitution cipher.\n";
                cout << "It shifts letters by a fixed number.\n";
                cout << "Features: Encryption, Decryption and Brute Force.\n";
                break;

            case 5:
                cout << "Thank you for using Caesar Cipher!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 5);

    return 0;
}