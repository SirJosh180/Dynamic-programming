# Dynamic-programming
Design and implement a Dynamic Programming algorithm for approximate string matching. The program searches a text file for substrings similar to a target pattern while allowing a maximum of k insertions, deletions, and substitutions. The activity includes the DP algorithm, C/C++ implementation, complexity analysis, and demonstration test runs.

# Approximate String Matching
 
A C++17 command-line tool for approximate (fuzzy) substring matching
using Dynamic Programming.
 
## Features
 
-   Searches a text file for approximate substring matches.
-   Supports insertion, deletion, and substitution errors.
-   Uses `k` as the maximum number of allowed errors.
-   Uses two rolling DP columns, requiring `O(m)` space.
-   Uses early exit when a match is found.
-   Allows repeated searches without restarting the program.
-   Type `quit` or `exit` to stop.
## Requirements
 
-   C++17-compatible compiler
-   `g++` or `clang++`
-   A text file to search
## Project Files
 
``` text
Dynamic/
├── approx_match.cpp
└── sample.txt
```
 
## Sample `sample.txt`
 
``` text
I like pesto sauce.
The chef prepared pest soup.
The garden has many plants.
This recipe uses pasta.
We found a small pet near the house.
The restaurant serves fresh pesto.
The word past appears in this sentence.
```
 
## Compile
 
Open Terminal and go to the project folder:
 
``` bash
cd ~/Desktop/Dynamic
```
 
Compile:
 
``` bash
g++ -O2 -std=c++17 -o approx_match approx_match.cpp
```
 
This creates the executable `approx_match`.
 
## Run
 
The recommended command is:
 
``` bash
./approx_match sample.txt
```
 
The program keeps the file fixed and asks for the pattern and `k`
interactively:
 
``` text
Approximate string matching in 'sample.txt'.
Type 'quit' at any prompt to exit.
 
Enter target pattern: pest
Enter max errors k (0, 1, 2, ...): 1
```
 
### Run Without a Filename
 
You can also run:
 
``` bash
./approx_match
```
 
The program will ask:
 
``` text
Enter text file name:
```
 
Enter:
 
``` text
sample.txt
```
 
## Meaning of `k`
 
`k` is the maximum total number of insertions, deletions, and
substitutions.
 
-   `k = 0`: exact matching only.
-   `k = 1`: one edit is allowed.
-   `k = 2`: up to two edits are allowed.
For example:
 
``` text
pesto → pest
```
 
uses one deletion.
 
``` text
pet → pest
```
 
uses one insertion.
 
``` text
past → pest
```
 
uses one substitution.
 
## Example Test Run
 
Command:
 
``` bash
./approx_match sample.txt
```
 
Input:
 
``` text
Enter target pattern: pest
Enter max errors k (0, 1, 2, ...): 1
```
 
Output for the supplied sample:
 
``` text
6 matching line(s) for "pest" (k=1):
  Line 1: I like pesto sauce.
  Line 2: The chef prepared pest soup.
  Line 4: This recipe uses pasta.
  Line 5: We found a small pet near the house.
  Line 6: The restaurant serves fresh pesto.
  Line 7: The word past appears in this sentence.
```
 
Line 4 matches because `pasta` contains the substring `past`, and `past`
differs from `pest` by one substitution.
 
For exact matching:
 
``` text
Enter target pattern: pest
Enter max errors k (0, 1, 2, ...): 0
```
 
The exact `pest` occurrence is on Line 2.
 
For a pattern that does not occur within the allowed errors:
 
``` text
Enter target pattern: computer
Enter max errors k (0, 1, 2, ...): 0
```
 
The program reports:
 
``` text
No lines matched "computer" within k=0 errors.
```
 
## Interactive Searches
 
After displaying results, the program automatically asks for another
pattern:
 
``` text
Enter target pattern:
```
 
For example:
 
``` text
pest
1
 
pasta
1
 
pet
1
```
 
The same file is searched each time.
 
To stop:
 
``` text
quit
```
 
or:
 
``` text
exit
```
 
## How the DP Works
 
Let:
 
-   `P` = target pattern
-   `T` = current text line
-   `m` = length of the pattern
-   `n` = length of the line
The DP value `D[i][j]` represents the minimum edit distance needed for
the first `i` characters of the pattern against text ending at position
`j`.
 
Initialization:
 
``` text
D[0][j] = 0
D[i][0] = i
```
 
Recurrence:
 
``` text
D[i][j] = D[i-1][j-1]                              if P[i] = T[j]
 
D[i][j] = 1 + min(
    D[i-1][j-1],   // substitution
    D[i-1][j],     // deletion
    D[i][j-1]      // insertion
)                                                     otherwise
```
 
A line contains an approximate match when:
 
``` text
D[m][j] <= k
```
 
for some position `j`.
 
The value `D[0][j] = 0` allows a possible match to start anywhere in the
line.
 
## Implementation Details
 
The core function is:
 
``` cpp
bool approxMatchInLine(
    const std::string& pattern,
    const std::string& line,
    int k)
```
 
It uses:
 
``` cpp
std::vector<int> prev(m + 1), curr(m + 1);
```
 
Instead of storing the entire DP table, only the previous and current
columns are kept.
 
The program checks:
 
``` cpp
if (prev[m] <= k) return true;
```
 
This is the early-exit condition.
 
The `searchFile` function reads the file line by line and stores
matching lines using:
 
``` cpp
struct MatchResult {
    int lineNumber;
    std::string text;
};
```
 
## Complexity
 
For a pattern of length `m` and a line of length `n`:
 
``` text
Time:  O(mn)
Space: O(m)
```
 
If the total number of characters in the file is `N`:
 
``` text
Total time: O(mN)
```
 
## Troubleshooting
 
### File Cannot Be Opened
 
If you see:
 
``` text
Error: could not open file 'sample.txt'
```
 
check the current directory:
 
``` bash
pwd
```
 
Then:
 
``` bash
ls
```
 
Make sure `sample.txt` is present.
 
If the file is somewhere else, provide its path:
 
``` bash
./approx_match /path/to/sample.txt
```
 
### Recompile After Changing the Code
 
After editing `approx_match.cpp`, compile again:
 
``` bash
g++ -O2 -std=c++17 -o approx_match approx_match.cpp
```
 
Then run:
 
``` bash
./approx_match sample.txt
```
 
## Quick Start
 
``` bash
cd ~/Desktop/Dynamic
g++ -O2 -std=c++17 -o approx_match approx_match.cpp
./approx_match sample.txt
```
 
Then enter:
 
``` text
pest
1
```
 
Type `quit` when finished.
 
## Purpose
 
This implementation demonstrates how Dynamic Programming can be used for
approximate string matching. The same basic idea can be applied to typo
detection, text search, basic plagiarism checking, and biological
sequence matching.
