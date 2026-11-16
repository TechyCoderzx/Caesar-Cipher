#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <cctype>

using namespace std;

// ============================================================
// ABSTRACT BASE CLASS
// ============================================================

class Cipher {
public:
    // Pure virtual functions make this an abstract class.
    virtual string encrypt(const string& message) const = 0;
    virtual string decrypt(const string& message) const = 0;

    // Virtual destructor is important when using base-class pointers/references.
    virtual ~Cipher() {}
};


// ============================================================
// DERIVED CLASS: CAESAR CIPHER
// ============================================================

class CaesarCipher : public Cipher {
private:
    // Encapsulation: the shift key cannot be accessed directly from outside.
    int shiftKey;

    // Converts any integer key into the range 0 to 25.
    static int normalizeKey(int key) {
        key = key % 26;

        if (key < 0) {
            key += 26;
        }

        return key;
    }

    // Shared logic used by both encryption and decryption.
    static string shiftText(const string& message, int shift) {
        shift = normalizeKey(shift);

        string result = message;

        for (size_t i = 0; i < result.length(); i++) {
            char ch = result[i];

            // Shift uppercase English letters.
            if (ch >= 'A' && ch <= 'Z') {
                result[i] = static_cast<char>(
                    'A' + (ch - 'A' + shift) % 26
                );
            }
            // Shift lowercase English letters.
            else if (ch >= 'a' && ch <= 'z') {
                result[i] = static_cast<char>(
                    'a' + (ch - 'a' + shift) % 26
                );
            }
            // Spaces, digits, and punctuation remain unchanged.
        }

        return result;
    }

public:
    // Constructor: sets the key when an object is created.
    CaesarCipher(int key) {
        shiftKey = normalizeKey(key);
    }

    // Setter: safely changes the shift key.
    void setKey(int key) {
        shiftKey = normalizeKey(key);
    }

    // Getter: returns the current shift key.
    int getKey() const {
        return shiftKey;
    }

    // Function overriding: implementation of the base-class function.
    string encrypt(const string& message) const override {
        return shiftText(message, shiftKey);
    }

    // Decryption reverses the encryption shift.
    string decrypt(const string& message) const override {
        return shiftText(message, -shiftKey);
    }
};


// ============================================================
// BRUTE-FORCE ANALYZER
// ============================================================

class CryptoAnalyzer {
private:
    // Gives a simple score to a possible English plaintext.
    // This is a basic heuristic, not a perfect language detector.
    int calculateEnglishScore(const string& text) const {
        const string commonLetters = "etaoinshrdlu";

        const string commonWords[] = {
            "the", "and", "you", "that", "have", "for", "not",
            "with", "this", "but", "from", "they", "say", "her",
            "she", "will", "one", "all", "would", "there", "their",
            "what", "about", "which", "when", "make", "can", "like",
            "time", "just", "know", "take", "people", "into", "your",
            "good", "some", "could", "them", "see", "other", "than",
            "then", "now", "look", "only", "come", "over", "think",
            "also", "back", "after", "use", "how", "our", "work",
            "first", "well", "even", "new", "want", "because", "these",
            "give", "day", "most", "hello", "world", "secret", "message",
            "caesar", "cipher"
        };

        const int commonWordCount =
            static_cast<int>(sizeof(commonWords) / sizeof(commonWords[0]));

        int score = 0;
        string currentWord;

        for (size_t i = 0; i <= text.length(); i++) {
            // A space at the end makes the last word get processed.
            char ch = (i < text.length()) ? text[i] : ' ';
            unsigned char safeChar = static_cast<unsigned char>(ch);

            if (isalpha(safeChar)) {
                char lowerChar = static_cast<char>(tolower(safeChar));
                currentWord += lowerChar;

                if (commonLetters.find(lowerChar) != string::npos) {
                    score += 1;
                }
            } else if (!currentWord.empty()) {
                for (int j = 0; j < commonWordCount; j++) {
                    if (currentWord == commonWords[j]) {
                        score += 8;
                        break;
                    }
                }

                currentWord.clear();
            }
        }

        return score;
    }

public:
    // Tests every possible Caesar key (0 through 25).
    void runBruteForce(const string& encryptedMessage) const {
        int bestKey = 0;
        int bestScore = -1;
        string bestCandidate;

        cout << "\n============== BRUTE-FORCE ANALYSIS ==============\n";
        cout << "Trying all 26 possible shift keys...\n\n";

        for (int key = 0; key < 26; key++) {
            // Create a Caesar Cipher object for the current test key.
            CaesarCipher trialCipher(key);

            // Polymorphism: use the derived object through the base class.
            const Cipher& cipher = trialCipher;

            string candidate = cipher.decrypt(encryptedMessage);

            cout << "Key " << key << " -> " << candidate << "\n";

            int currentScore = calculateEnglishScore(candidate);

            if (currentScore > bestScore) {
                bestScore = currentScore;
                bestKey = key;
                bestCandidate = candidate;
            }
        }

        cout << "\n---------------- BEST GUESS ----------------------\n";
        cout << "Most likely shift key : " << bestKey << "\n";
        cout << "Possible original text: " << bestCandidate << "\n";
        cout << "--------------------------------------------------\n";
        cout << "Note: The best guess is based on common English letters\n";
        cout << "and words. Check the 26 results if the message is short,\n";
        cout << "unusual, or written in a language other than English.\n";
        cout << "==================================================\n\n";
    }
};


// ============================================================
// TERMINAL DISPLAY AND INPUT FUNCTIONS
// ============================================================

void displayHeader() {
    cout << "\n";
    cout << "==================================================\n";
    cout << "                  CAESAR CIPHER                   \n";
    cout << "          ENCRYPTION & DECRYPTION SYSTEM          \n";
    cout << "==================================================\n\n";
}

// Small terminal animation. It does not affect the cipher algorithm.
void loadingAnimation(const string& message) {
    cout << message;

    for (int i = 0; i < 3; i++) {
        cout << "." << flush;
        this_thread::sleep_for(chrono::milliseconds(300));
    }

    cout << " Done!\n\n";
}

void displayMenu() {
    cout << "------------------- MAIN MENU --------------------\n";
    cout << "[1] Encrypt a message\n";
    cout << "[2] Decrypt a message\n";
    cout << "[3] Brute-force analysis\n";
    cout << "[4] About the project\n";
    cout << "[5] Exit\n";
    cout << "--------------------------------------------------\n";
}

// Reads an integer and handles invalid input without crashing.
int readInteger(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input. Please enter a whole number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


// ============================================================
// ENCRYPTION AND DECRYPTION MENU OPERATIONS
// ============================================================

void performCipherOperation(bool encryption) {
    string message;

    cout << "\n==================================================\n";

    if (encryption) {
        cout << "                 MESSAGE ENCRYPTION\n";
    } else {
        cout << "                 MESSAGE DECRYPTION\n";
    }

    cout << "==================================================\n\n";
    cout << "Enter your message: ";
    getline(cin, message);

    if (message.empty()) {
        cout << "The message cannot be empty.\n\n";
        return;
    }

    int key = readInteger("Enter the shift key: ");

    CaesarCipher caesar(key);

    // Runtime polymorphism: a Cipher reference refers to CaesarCipher.
    const Cipher& cipher = caesar;

    cout << "Effective shift key: " << caesar.getKey() << "\n";

    if (encryption) {
        loadingAnimation("Encrypting message");

        string encryptedMessage = cipher.encrypt(message);

        cout << "Original message  : " << message << "\n";
        cout << "Encrypted message : " << encryptedMessage << "\n\n";
    } else {
        loadingAnimation("Decrypting message");

        string decryptedMessage = cipher.decrypt(message);

        cout << "Encrypted message : " << message << "\n";
        cout << "Decrypted message : " << decryptedMessage << "\n\n";
    }
}


// ============================================================
// BRUTE-FORCE MENU OPERATION
// ============================================================

void performBruteForceOperation() {
    string encryptedMessage;

    cout << "\n==================================================\n";
    cout << "                 BRUTE-FORCE MODE\n";
    cout << "==================================================\n\n";
    cout << "Enter the encrypted message: ";
    getline(cin, encryptedMessage);

    if (encryptedMessage.empty()) {
        cout << "The message cannot be empty.\n\n";
        return;
    }

    loadingAnimation("Starting cryptanalysis");

    CryptoAnalyzer analyzer;
    analyzer.runBruteForce(encryptedMessage);
}


// ============================================================
// ABOUT SECTION
// ============================================================

void displayAbout() {
    cout << "\n==================================================\n";
    cout << "                  ABOUT THE PROJECT\n";
    cout << "==================================================\n\n";

    cout << "The Caesar Cipher is a substitution cipher.\n";
    cout << "It encrypts a message by shifting each English\n";
    cout << "letter by a fixed number of positions.\n\n";

    cout << "PROJECT FEATURES\n";
    cout << "1. Message encryption\n";
    cout << "2. Message decryption\n";
    cout << "3. Brute-force testing of all 26 keys\n\n";

    cout << "OOP CONCEPTS USED\n";
    cout << "1. Encapsulation\n";
    cout << "2. Abstraction\n";
    cout << "3. Inheritance\n";
    cout << "4. Runtime polymorphism\n\n";

    cout << "The brute-force feature displays every possible\n";
    cout << "decryption and gives an English-language best guess.\n";

    cout << "==================================================\n\n";
}


// ============================================================
// MAIN FUNCTION
// ============================================================

int main() {
    displayHeader();
    loadingAnimation("Initializing Caesar Cipher system");

    while (true) {
        displayMenu();

        int choice = readInteger("Enter your choice: ");

        switch (choice) {
            case 1:
                performCipherOperation(true);
                break;

            case 2:
                performCipherOperation(false);
                break;

            case 3:
                performBruteForceOperation();
                break;

            case 4:
                displayAbout();
                break;

            case 5:
                loadingAnimation("Closing application");
                cout << "Thank you for using Caesar Cipher.\n";
                cout << "Application closed successfully.\n";
                return 0;

            default:
                cout << "Invalid choice. Please select an option from 1 to 5.\n\n";
        }
    }

    return 0;
}
