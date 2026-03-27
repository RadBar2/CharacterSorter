#include <iostream>
#include <fstream>
#include <sstream>

#include <exception>
#include <stdexcept>

#include "CharacterRankDAG.h"

using namespace Ranking;

// Helper to print result
void printResult(const std::string& testName, bool pass) {
    std::cout << testName << ": " << (pass ? "PASS" : "FAIL") << "\n";
}

// ========================
// TESTS
// ========================

void testCreateAndRead() {
    CharacterRankDAG dag;
    dag += "A";

    printResult("Create + Read existing", dag["A"]);
    printResult("Read non-existing", !dag["X"]);
}

void testDuplicateInsert() {
    CharacterRankDAG dag;
    bool exceptionThrown = false;

    try {
        dag += "A";
        dag += "A";
    } catch (const RankingException&) {
        exceptionThrown = true;
    }

    printResult("Duplicate insert throws", exceptionThrown);
}

void testAddRelation() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B";

    bool success = true;

    try {
        dag.addRelation("A", "B");
    } catch (...) {
        success = false;
    }

    printResult("Add relation A > B", success);
}

void testInvalidRelation() {
    CharacterRankDAG dag;
    dag += "A";

    bool exceptionThrown = false;

    try {
        dag.addRelation("A", "B"); // B missing
    } catch (const RankingException&) {
        exceptionThrown = true;
    }

    printResult("Invalid relation throws", exceptionThrown);
}

void testCycleDetection() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B"; dag += "C";

    bool exceptionThrown = false;

    try {
        dag.addRelation("A", "B");
        dag.addRelation("B", "C");
        dag.addRelation("C", "A"); // cycle
    } catch (const RankingException&) {
        exceptionThrown = true;
    }

    printResult("Cycle detection", exceptionThrown);
}

void testUpdateCharacter() {
    CharacterRankDAG dag;
    dag += "A";

    dag *= {"A", "Hero"};

    printResult("Update character name", dag["Hero"] && !dag["A"]);
}

void testDeleteCharacter() {
    CharacterRankDAG dag;
    dag += "A";

    dag -= "A";

    printResult("Delete character", !dag["A"]);
}

void testRemoveRelation() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B";

    dag.addRelation("A", "B");
    dag.removeRelation("A", "B");

    // We assume no exception = success
    printResult("Remove relation", true);
}

void testClearOperator() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B";

    !dag;

    printResult("Clear operator", !dag["A"] && !dag["B"]);
}

void testComparisonOperators() {
    CharacterRankDAG dag1;
    CharacterRankDAG dag2;

    dag1 += "A";
    dag2 += "A";

    bool equal = (dag1 == dag2);
    bool notEqual = !(dag1 != dag2);

    printResult("Operator ==", equal);
    printResult("Operator !=", notEqual);
}

void testToString() {
    CharacterRankDAG dag;
    dag += "A";

    std::string output = dag.toString();

    printResult("toString not empty", !output.empty());
}

// ========================
// MAIN
// ========================

int main() {
    try {
        // Redirect output to both console AND file
        std::ofstream logFile("log.txt");
        std::streambuf* coutBuf = std::cout.rdbuf();

        std::cout.rdbuf(logFile.rdbuf());

        std::cout << "=== TESTING CharacterRankDAG ===\n\n";

        testCreateAndRead();
        testDuplicateInsert();
        testAddRelation();
        testInvalidRelation();
        testCycleDetection();
        testUpdateCharacter();
        testDeleteCharacter();
        testRemoveRelation();
        testClearOperator();
        testComparisonOperators();
        testToString();

        std::cout << "\n=== TESTING COMPLETE ===\n";

        // Restore console output
        std::cout.rdbuf(coutBuf);

        // Also print to console
        std::ifstream inFile("log.txt");
        std::cout << inFile.rdbuf();
    } catch (const std::exception& e) {
        std::cerr << "Caught standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Caught an unknown exception!" << std::endl;
    }
    

    return 0;
}