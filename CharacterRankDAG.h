#ifndef CHARACTER_RANK_DAG_H
#define CHARACTER_RANK_DAG_H

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace Ranking {

    class RankingException : public std::runtime_error {
    public:
        explicit RankingException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class CharacterRankDAG {
    private:
        // --- 1. Fields & Internal Types ---
        class Impl; 
        std::unique_ptr<Impl> pImpl; 

    public:
        // --- 2. Constructors & Destructor ---
        CharacterRankDAG();
        CharacterRankDAG(const CharacterRankDAG& other);
        CharacterRankDAG& operator=(const CharacterRankDAG& other);
        ~CharacterRankDAG();

        // --- 3. Operator Overloads ---
        CharacterRankDAG& operator+=(const std::string& name);                          
        CharacterRankDAG& operator-=(const std::string& name);                          
        CharacterRankDAG& operator=(std::pair<std::string, std::string> rel);           
        CharacterRankDAG& operator*=(const std::pair<std::string, std::string>& names); 
        CharacterRankDAG& operator%=(std::pair<std::string, std::string> rel);          
        bool operator[](const std::string& name) const;                                 
        void operator!();                                                               

        // Comparison Operators
        bool operator==(const CharacterRankDAG& other) const;
        bool operator!=(const CharacterRankDAG& other) const;
        bool operator<(const CharacterRankDAG& other) const;
        bool operator<=(const CharacterRankDAG& other) const;
        bool operator>(const CharacterRankDAG& other) const;
        bool operator>=(const CharacterRankDAG& other) const;

        // --- 4. Other Methods ---
        // CRUD Operations
        void createCharacter(const std::string& name);
        bool readCharacter(const std::string& name) const;
        void updateCharacter(const std::string& oldName, const std::string& newName);
        void deleteCharacter(const std::string& name);

        // Relation Management
        void addRelation(const std::string& superior, const std::string& subordinate);
        void removeRelation(const std::string& superior, const std::string& subordinate);

        // Logic & Conversion
        std::vector<std::string> getRanking() const;
        std::string toString() const;
    };
}

#endif