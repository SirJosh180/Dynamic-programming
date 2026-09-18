// approx_match.cpp
//
// Approximate (fuzzy) substring search in a text file, allowing up to k
// character insertions, deletions, and substitutions (typographical
// errors) between the target pattern and a substring of each line.
//
// DP recurrence (see write-up for full derivation):
//   D[0][j] = 0                                   (a match may start fresh anywhere)
//   D[i][0] = i                                    (i deletions to match empty text)
//   D[i][j] = D[i-1][j-1]                          if P[i] == T[j]
//           = 1 + min(D[i-1][j-1],   // substitution
//                      D[i-1][j],    // deletion of P[i]
//                      D[i][j-1])    // insertion of T[j]
//                                                   otherwise
// The line matches within k errors iff D[m][j] <= k for some j.
// Only the previous column is needed, so space is O(m) and time is
// O(m*n) per line (i.e. O(m*N) over a file of total size N).
//
// This version runs as an interactive loop: give it the file name once
// (as a command-line argument, or it will ask), then it repeatedly asks
// for a pattern and a k, prints the matching lines, and asks again --
// forever, until you type "quit".
//
// Build:
//   g++ -O2 -std=c++17 -o approx_match approx_match.cpp
//
// Run:
//   ./approx_match sample.txt
//   (or just ./approx_match, and it will ask you for the file name too)

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Returns true iff `line` contains a substring within edit distance k of
// `pattern`. Implements the DP above using two rolling columns (O(m)
// space) with an early exit as soon as a match is found.
bool approxMatchInLine(const std::string& pattern, const std::string& line, int k) {
    int m = static_cast<int>(pattern.size());
    int n = static_cast<int>(line.size());
    if (m == 0) return true;

    std::vector<int> prev(m + 1), curr(m + 1);
    for (int i = 0; i <= m; ++i) prev[i] = i;   // D[i][0] = i

    for (int j = 1; j <= n; ++j) {
        curr[0] = 0;                            // D[0][j] = 0 (free start)
        for (int i = 1; i <= m; ++i) {
            int substCost = (pattern[i - 1] == line[j - 1]) ? 0 : 1;
            curr[i] = std::min({
                prev[i - 1] + substCost,  // match / substitution
                prev[i] + 1,              // deletion of P[i]
                curr[i - 1] + 1           // insertion of T[j]
            });
        }
        prev.swap(curr);
        if (prev[m] <= k) return true;          // early exit: match found
    }
    return false;
}

struct MatchResult {
    int lineNumber;
    std::string text;
};

// Scans `filename` line by line, running approxMatchInLine on each.
// Returns false only on file-open failure; matches are appended to `out`.
bool searchFile(const std::string& pattern, const std::string& filename, int k,
                 std::vector<MatchResult>& out) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << filename << "'\n";
        return false;
    }
    std::string line;
    int lineno = 0;
    while (std::getline(file, line)) {
        ++lineno;
        if (approxMatchInLine(pattern, line, k)) {
            out.push_back({lineno, line});
        }
    }
    return true;
}

void printResults(const std::string& pattern, int k, const std::vector<MatchResult>& results) {
    if (results.empty()) {
        std::cout << "No lines matched \"" << pattern << "\" within k=" << k << " errors.\n";
        return;
    }
    std::cout << results.size() << " matching line(s) for \"" << pattern
               << "\" (k=" << k << "):\n";
    for (const auto& r : results) {
        std::cout << "  Line " << r.lineNumber << ": " << r.text << "\n";
    }
}

// Reads a single non-negative integer from a line of input.
// Returns true on success, false on "quit"/"exit" or on bad input.
bool readNonNegativeInt(const std::string& prompt, int& value, bool& wantsQuit) {
    wantsQuit = false;
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) { wantsQuit = true; return false; }
    if (s == "quit" || s == "exit") { wantsQuit = true; return false; }
    try {
        size_t pos;
        value = std::stoi(s, &pos);
        if (value < 0 || pos != s.size()) throw std::invalid_argument("bad int");
    } catch (...) {
        return false;
    }
    return true;
}

// The main interactive loop: keep the file fixed, repeatedly ask for a
// new pattern and a new k, print results, and loop until the user quits.
void runInteractive(const std::string& filename) {
    std::cout << "Approximate string matching in '" << filename << "'.\n";
    std::cout << "Type 'quit' at any prompt to exit.\n\n";

    while (true) {
        std::cout << "Enter target pattern: ";
        std::string pattern;
        if (!std::getline(std::cin, pattern)) break;
        if (pattern == "quit" || pattern == "exit") break;
        if (pattern.empty()) {
            std::cout << "Please enter a non-empty pattern.\n\n";
            continue;
        }

        int k;
        bool wantsQuit;
        if (!readNonNegativeInt("Enter max errors k (0, 1, 2, ...): ", k, wantsQuit)) {
            if (wantsQuit) break;
            std::cout << "k must be a non-negative integer.\n\n";
            continue;
        }

        std::vector<MatchResult> results;
        if (!searchFile(pattern, filename, k, results)) {
            // File itself failed to open -- no point looping further.
            return;
        }
        std::cout << "\n";
        printResults(pattern, k, results);
        std::cout << "\n";
        // Loop back automatically and ask for the next pattern/k.
    }

    std::cout << "Goodbye!\n";
}

int main(int argc, char* argv[]) {
    std::string filename;

    if (argc == 2) {
        filename = argv[1];
    } else if (argc == 1) {
        // No filename given on the command line -- ask for it once.
        std::cout << "Enter text file name: ";
        if (!std::getline(std::cin, filename) || filename.empty()) {
            std::cerr << "No file name given. Exiting.\n";
            return 1;
        }
    } else {
        std::cerr << "Usage:\n"
                  << "  " << argv[0] << " <filename>   (then enter pattern/k repeatedly)\n"
                  << "  " << argv[0] << "               (will also ask for the filename)\n";
        return 1;
    }

    // Quick check that the file can be opened before starting the loop.
    {
        std::ifstream test(filename);
        if (!test.is_open()) {
            std::cerr << "Error: could not open file '" << filename << "'\n";
            return 1;
        }
    }

    runInteractive(filename);
    return 0;
}