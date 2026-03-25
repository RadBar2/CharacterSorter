#include "CharacterRankDAG.h"
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <queue>
#include <vector>

namespace Ranking {

    // --- 1. Internal Fields (The Impl Class) ---
    class CharacterRankDAG::Impl {
    public:
        std::set<std::string> nodes;
        std::map<std::string, std::set<std::string>> edges;
        std::map<std::string, std::set<std::string>> directEdges;

        bool hasPath(const std::string& start, const std::string& end) const {
            if (edges.find(start) == edges.end()) return false;
            if (edges.at(start).count(end)) return true;

            for (const auto& neighbor : edges.at(start)) {
                if (hasPath(neighbor, end)) return true;
            }
            return false;
        }

        void enforceAcyclic(const std::string& superior, const std::string& subordinate) {
            if (superior == subordinate) {
                throw RankingException("Self-loops not allowed in DAG.");
            }
            if (hasPath(subordinate, superior)) {
                throw RankingException("Cycle detected: Adding this relation violates DAG structure.");
            }
        }

        void recomputeTransitiveClosure() {
            std::map<std::string, std::set<std::string>> newEdges;
            for (const auto& start : nodes) {
                std::set<std::string> visited;
                dfs(start, start, visited, newEdges);
            }
            edges = std::move(newEdges);
        }

        void dfs(const std::string& origin, const std::string& current,
                 std::set<std::string>& visited,
                 std::map<std::string, std::set<std::string>>& newEdges) const {
            if (!edges.count(current)) return;
            for (const auto& neighbor : edges.at(current)) {
                if (!visited.count(neighbor)) {
                    visited.insert(neighbor);
                    newEdges[origin].insert(neighbor);
                    dfs(origin, neighbor, visited, newEdges);
                }
            }
        }
    };

    // --- 2. Constructors & Destructor ---
    CharacterRankDAG::CharacterRankDAG() : pImpl(std::make_unique<Impl>()) {}
    
    CharacterRankDAG::CharacterRankDAG(const CharacterRankDAG& other) 
        : pImpl(std::make_unique<Impl>(*other.pImpl)) {}
    
    CharacterRankDAG& CharacterRankDAG::operator=(const CharacterRankDAG& other) {
        if (this != &other) pImpl = std::make_unique<Impl>(*other.pImpl);
        return *this;
    }
    
    CharacterRankDAG::~CharacterRankDAG() = default;

    // --- 3. Operator Overloads ---
    CharacterRankDAG& CharacterRankDAG::operator+=(const std::string& name) { 
        createCharacter(name); 
        return *this; 
    }
    CharacterRankDAG& CharacterRankDAG::operator-=(const std::string& name) {
        deleteCharacter(name); 
        return *this;
    }
    bool CharacterRankDAG::operator[](const std::string& name) const {
        return readCharacter(name);
    }
    void CharacterRankDAG::operator!() {
        pImpl->nodes.clear();
        pImpl->edges.clear();
        pImpl->directEdges.clear();
    }

    // Comparison Operators
    bool CharacterRankDAG::operator==(const CharacterRankDAG& other) const {
        return pImpl->nodes == other.pImpl->nodes && pImpl->edges == other.pImpl->edges;
    }
    bool CharacterRankDAG::operator!=(const CharacterRankDAG& other) const {
        return !(*this == other);
    }
    bool CharacterRankDAG::operator<(const CharacterRankDAG& other) const {
        if (pImpl->nodes != other.pImpl->nodes) return pImpl->nodes < other.pImpl->nodes;
        return pImpl->edges < other.pImpl->edges;
    }
    bool CharacterRankDAG::operator<=(const CharacterRankDAG& other) const {
        return *this < other || *this == other;
    }
    bool CharacterRankDAG::operator>(const CharacterRankDAG& other) const {
        return !(*this <= other);
    }
    bool CharacterRankDAG::operator>=(const CharacterRankDAG& other) const {
        return !(*this < other);
    }

    // --- 4. Named Methods (CRUD & Logic) ---
    void CharacterRankDAG::createCharacter(const std::string& name) {
        if (pImpl->nodes.count(name)) throw RankingException("Character already exists.");
        pImpl->nodes.insert(name);
    }

    bool CharacterRankDAG::readCharacter(const std::string& name) const {
        return pImpl->nodes.count(name) > 0;
    }

    void CharacterRankDAG::updateCharacter(const std::string& oldName, const std::string& newName) {
        if (!pImpl->nodes.count(oldName)) throw RankingException("Character not found.");
        if (pImpl->nodes.count(newName)) throw RankingException("New name already exists.");

        // Update logic
        pImpl->nodes.erase(oldName);
        pImpl->nodes.insert(newName);

        auto it = pImpl->directEdges.find(oldName);
        if (it != pImpl->directEdges.end()) {
            pImpl->directEdges[newName] = std::move(it->second);
            pImpl->directEdges.erase(it);
        }

        for (auto& pair : pImpl->directEdges) {
            if (pair.second.count(oldName)) {
                pair.second.erase(oldName);
                pair.second.insert(newName);
            }
        }
        pImpl->recomputeTransitiveClosure();
    }

    void CharacterRankDAG::deleteCharacter(const std::string& name) {
        if (!pImpl->nodes.count(name)) throw RankingException("Character not found.");
        
        pImpl->nodes.erase(name);
        pImpl->directEdges.erase(name);
        for (auto& pair : pImpl->directEdges) {
            pair.second.erase(name);
        }
        pImpl->recomputeTransitiveClosure();
    }

    void CharacterRankDAG::addRelation(const std::string& superior, const std::string& subordinate) {
        if (!pImpl->nodes.count(superior) || !pImpl->nodes.count(subordinate)) {
            throw RankingException("One or both characters do not exist.");
        }
        
        pImpl->enforceAcyclic(superior, subordinate);
        pImpl->directEdges[superior].insert(subordinate);
        pImpl->recomputeTransitiveClosure();
    }

    void CharacterRankDAG::removeRelation(const std::string& superior, const std::string& subordinate) {
        if (!pImpl->directEdges.count(superior) || !pImpl->directEdges[superior].count(subordinate)) {
            throw RankingException("Relation does not exist.");
        }
        pImpl->directEdges[superior].erase(subordinate);
        pImpl->recomputeTransitiveClosure();
    }

    std::vector<std::string> CharacterRankDAG::getRanking() const {
        std::map<std::string, int> inDegree;
        for (const auto& node : pImpl->nodes) inDegree[node] = 0;
        for (const auto& pair : pImpl->directEdges) {
            for (const auto& neighbor : pair.second) inDegree[neighbor]++;
        }

        std::queue<std::string> q;
        for (const auto& pair : inDegree) if (pair.second == 0) q.push(pair.first);

        std::vector<std::string> result;
        while (!q.empty()) {
            std::string current = q.front(); q.pop();
            result.push_back(current);
            if (pImpl->directEdges.count(current)) {
                for (const auto& neighbor : pImpl->directEdges.at(current)) {
                    if (--inDegree[neighbor] == 0) q.push(neighbor);
                }
            }
        }

        if (result.size() != pImpl->nodes.size()) throw RankingException("Cycle detected.");
        return result;
    }

    std::string CharacterRankDAG::toString() const {
        std::stringstream ss;
        auto ranking = getRanking();
        ss << "Ranking:\n";
        for (const auto& name : ranking) ss << name << "\n";
        return ss.str();
    }
}