#include <iostream>
#include <fstream>
#include <sstream>
#include "CharacterRankDAG.h"

using namespace Ranking;

// Helper that prints and THROWS on failure to trigger the main catch block
void assertTest(const std::string& testName, bool pass) {
    if (pass) {
        std::cout << "[ PASS ] " << testName << "\n";
    } else {
        std::cout << "[ FAIL ] " << testName << "\n";
        // Throwing here ensures we jump straight to the catch in main
        throw RankingException("Test Logic Failure: " + testName);
    }
}

// ========================
// TESTS
// ========================

bool testCreateAndRead() {
    CharacterRankDAG dag;
    dag += "A";
    return dag["A"] && !dag["X"];
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

}

bool testCycleDetection() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B"; dag += "C";
    try {
        dag.addRelation("A", "B");
        dag.addRelation("B", "C");
        dag.addRelation("C", "A"); // Should throw
        return false; // If it gets here, cycle detection failed
    } catch (const RankingException&) {
        return true; // Correctly caught the cycle
    }
}

void testUpdateCharacter() {
    CharacterRankDAG dag;
    dag += "A";

    dag *= {"A", "Hero"};

}

void testDeleteCharacter() {
    CharacterRankDAG dag;
    dag += "A";

    dag -= "A";

}

void testRemoveRelation() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B";

    dag.addRelation("A", "B");
    dag %= {"A", "B"};
}

void testClearOperator() {
    CharacterRankDAG dag;
    dag += "A"; dag += "B";

    !dag;

    
}

void testComparisonOperators() {
    CharacterRankDAG dag1;
    CharacterRankDAG dag2;

    dag1 += "A";
    dag2 += "A";

    bool equal = (dag1 == dag2);
    bool notEqual = !(dag1 != dag2);

}

void testToString() {
    CharacterRankDAG dag;
    dag += "A";

    std::string output = dag.toString();

    
}

// ========================
// MAIN
// ========================

int main() {
    // 1. Setup Logging
    std::ofstream logFile("log.txt");
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log.txt\n";
        return 1;
    }

    // Redirect cout to logFile
    std::streambuf* coutBuf = std::cout.rdbuf();
    std::cout.rdbuf(logFile.rdbuf());

    std::cout << "CharacterRankDAG Test Report\n";
    std::cout << "============================\n\n";

    // 2. Run Tests Individually
    // If one fails, the others will still run.
    /*
    runTest("Create and Read", testCreateAndRead);
    runTest("Duplicate Insert", testDuplicateInsert);
    runTest("Add Relation", testAddRelation);
    runTest("Invalid Relation", testInvalidRelation);
    runTest("Cycle Detection", testCycleDetection);
    runTest("Update Character", testUpdateCharacter);
    runTest("Delete Character", testDeleteCharacter);
    runTest("Remove Relation", testRemoveRelation);
    runTest("Clear Operator", testClearOperator);
    runTest("Comparison Operators", testComparisonOperators);
    runTest("ToString Output", testToString); */

    std::cout << "\n============================\n";
    std::cout << "Testing Session Finished.\n";

    // 3. Cleanup and Restore
    logFile.flush();
    logFile.close();
    std::cout.rdbuf(coutBuf); // Restore console

    // 4. Output log to console for immediate viewing
    std::ifstream inFile("log.txt");
    if (inFile) {
        std::cout << inFile.rdbuf();
    }

    return 0;
}
