#include <iostream>
#include "CharacterRankDAG.h"

using namespace Ranking;

int main() {
    std::cout << "CharacterRankDAG Demo\n\n";

    CharacterRankDAG dag;

    // Create (+= / createCharacter)
    std::cout << "[CREATE]\n";
    dag += "A";
    dag += "B";
    dag += "C";
    dag.createCharacter("D");

    std::cout << "Created characters A, B, C, D\n\n";

    
    // Read ([] / readCharacter)
    std::cout << "[READ]\n";
    std::cout << "Does A exist? " << (dag["A"] ? "Yes" : "No") << "\n";
    std::cout << "Does X exist? " << (dag["X"] ? "Yes" : "No") << "\n\n";

    // Add Relations
    std::cout << "[ADD RELATIONS]\n";
    dag.addRelation("A", "B");
    dag.addRelation("B", "C");
    dag.addRelation("A", "D");

    std::cout << "Added relations: A>B, B>C, A>D\n\n";

    // Update (*= / updateCharacter)
    std::cout << "[UPDATE]\n";
    dag *= {"A", "Hero"};
    std::cout << "Renamed A -> Hero\n\n";

    // Remove Relation (%=)
    std::cout << "[REMOVE RELATION]\n";
    dag %= {"Hero", "B"};
    std::cout << "Removed relation Hero > B\n\n";

    // Delete (-= / deleteCharacter)
    std::cout << "[DELETE]\n";
    dag -= "D";
    std::cout << "Deleted character D\n\n";

    // Print State
    std::cout << "[TO STRING]\n";
    std::cout << dag.toString() << "\n\n";

    // Comparison Operators
    std::cout << "[COMPARISON]\n";
    CharacterRankDAG dag2;

    dag2 += "Hero";
    dag2 += "B";
    dag2 += "C";

    if (dag == dag2) {
        std::cout << "dag == dag2\n";
    } else {
        std::cout << "dag != dag2\n";
    }

    if (dag < dag2) {
        std::cout << "dag < dag2\n";
    } else {
        std::cout << "dag >= dag2\n";
    }
    std::cout << "\n";

    // Clear (!)
    std::cout << "[CLEAR]\n";
    !dag;
    std::cout << "Cleared DAG\n";
    std::cout << dag.toString() << "\n\n";

    // Exception tests
    std::cout << "[EXCEPTIONS]\n";

    try {
        dag += "X";
        dag += "X"; // duplicate
    } catch (const RankingException& e) {
        std::cout << "Caught exception (duplicate): " << e.what() << "\n";
    }

    try {
        dag.addRelation("X", "Y"); // Y doesn't exist
    } catch (const RankingException& e) {
        std::cout << "Caught exception (invalid relation): " << e.what() << "\n";
    }

    try {
        dag.addRelation("X", "X"); // self-loop
    } catch (const RankingException& e) {
        std::cout << "Caught exception (self-loop): " << e.what() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";

    return 0;
}

