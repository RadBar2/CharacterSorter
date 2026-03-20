#ifndef CHARACTER_RANK_DAG_H
#define CHARACTER_RANK_DAG_H

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace ranking {

    // Custom Exception
    class RankingException : public std::runtime_error {
    public:
        explicit RankingException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class CharacterRankDAG {
    public:
        CharacterRankDAG();
        CharacterRankDAG(const CharacterRankDAG& other);
        CharacterRankDAG& operator=(const CharacterRankDAG& other);
        ~CharacterRankDAG();

        // CRUD Operations
        void createCharacter(const std::string& name);
        bool readCharacter(const std::string& name) const;
        void updateCharacter(const std::string& oldName, const std::string& newName);
        void deleteCharacter(const std::string& name);

        // Relation Management
        void addRelation(const std::string& superior, const std::string& subordinate);
        void removeRelation(const std::string& superior, const std::string& subordinate);

        // Operator Overloads
        CharacterRankDAG& operator+=(const std::string& name);                          // Create
        CharacterRankDAG& operator-=(const std::string& name);                          // Delete
        CharacterRankDAG& operator=(std::pair<std::string, std::string> rel);           // Add Relation
        CharacterRankDAG& operator*=(const std::pair<std::string, std::string>& names); // Update Character
        CharacterRankDAG& operator%=(std::pair<std::string, std::string> rel);          // Remove Relation
        bool operator[](const std::string& name) const;                                 // Search
        void operator!();                                                               // Clear

        // Comparison Operators
        bool operator==(const CharacterRankDAG& other) const;
        bool operator!=(const CharacterRankDAG& other) const;
        bool operator<(const CharacterRankDAG& other) const;
        bool operator<=(const CharacterRankDAG& other) const;
        bool operator>(const CharacterRankDAG& other) const;
        bool operator>=(const CharacterRankDAG& other) const;

        std::string toString() const;

        // Methods
        std::vector<std::string> getRanking() const;

    private:
        class Impl; 
        std::unique_ptr<Impl> pImpl; // The Pimpl pointer
    };
}

#endif