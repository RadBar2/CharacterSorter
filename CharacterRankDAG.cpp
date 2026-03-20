#include "CharacterRankDAG.h"
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <queue>
#include <vector>

namespace ranking {

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
            // perform all operations first
            bool selfLoop = (superior == subordinate);
            bool wouldCycle = hasPath(subordinate, superior);

            if (selfLoop) {
                throw RankingException("Self-loops not allowed in DAG.");
            }
            if (wouldCycle) {
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

        void dfs(const std::string& origin,
                 const std::string& current,
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

    // Constructors / Destructors
    CharacterRankDAG::CharacterRankDAG() : pImpl(std::make_unique<Impl>()) {}
    CharacterRankDAG::CharacterRankDAG(const CharacterRankDAG& other)
        : pImpl(std::make_unique<Impl>(*other.pImpl)) {}
    CharacterRankDAG& CharacterRankDAG::operator=(const CharacterRankDAG& other) {
        if (this != &other) pImpl = std::make_unique<Impl>(*other.pImpl);
        return *this;
    }
    CharacterRankDAG::~CharacterRankDAG() = default;

    // CRUD operations (OOP-style exception at the end)
    void CharacterRankDAG::createCharacter(const std::string& name) {
        // Backup current state
        auto nodesBackup = pImpl->nodes;

        // Perform operation
        pImpl->nodes.insert(name);

        // Exception check after operation
        if (nodesBackup.count(name) > 0) {
            // Rollback on failure
            pImpl->nodes = std::move(nodesBackup);
            throw RankingException("Character already exists.");
        }
    }

    bool CharacterRankDAG::readCharacter(const std::string& name) const {
        return pImpl->nodes.count(name) > 0;
    }

    void CharacterRankDAG::updateCharacter(const std::string& oldName, const std::string& newName) {
        // Backup current state
        auto nodesBackup = pImpl->nodes;
        auto edgesBackup = pImpl->edges;
        auto directEdgesBackup = pImpl->directEdges;

        // Perform operation
        pImpl->nodes.erase(oldName);
        pImpl->nodes.insert(newName);

        if (pImpl->edges.count(oldName)) {
            pImpl->edges[newName] = pImpl->edges[oldName];
            pImpl->edges.erase(oldName);
        }

        for (auto& pair : pImpl->edges) {
            if (pair.second.count(oldName)) {
                pair.second.erase(oldName);
                pair.second.insert(newName);
            }
        }

        // Exception checks after operation
        bool oldExists = nodesBackup.count(oldName) > 0;
        bool newExists = nodesBackup.count(newName) > 0;

        if (!oldExists || newExists) {
            // Rollback to previous state
            pImpl->nodes = std::move(nodesBackup);
            pImpl->edges = std::move(edgesBackup);
            pImpl->directEdges = std::move(directEdgesBackup);

            if (!oldExists) {
                throw RankingException("Character not found.");
            }
            if (newExists) {
                throw RankingException("New name already exists.");
            }
        }
    }

    void CharacterRankDAG::deleteCharacter(const std::string& name) {
        // Backup current state
        auto nodesBackup = pImpl->nodes;
        auto edgesBackup = pImpl->edges;
        auto directEdgesBackup = pImpl->directEdges;

        // Perform operation
        pImpl->nodes.erase(name);
        pImpl->directEdges.erase(name);
        for (auto& pair : pImpl->directEdges) {
            pair.second.erase(name);
        }

        pImpl->edges = pImpl->directEdges;
        pImpl->recomputeTransitiveClosure();

        // Exception check after operation
        bool existedBefore = nodesBackup.count(name) > 0;
        if (!existedBefore) {
            // Rollback to previous state
            pImpl->nodes = std::move(nodesBackup);
            pImpl->edges = std::move(edgesBackup);
            pImpl->directEdges = std::move(directEdgesBackup);

            throw RankingException("Character not found.");
        }
    }

    void CharacterRankDAG::addRelation(const std::string& superior, const std::string& subordinate) {
        // Backup current state
        auto nodesBackup = pImpl->nodes;
        auto edgesBackup = pImpl->edges;
        auto directEdgesBackup = pImpl->directEdges;

        // Perform operation
        pImpl->directEdges[superior].insert(subordinate);
        pImpl->edges = pImpl->directEdges;
        pImpl->recomputeTransitiveClosure();

        try {
            // Exception checks after operation
            if (!pImpl->nodes.count(superior) || !pImpl->nodes.count(subordinate)) {
                throw RankingException("One or both characters do not exist.");
            }

            pImpl->enforceAcyclic(superior, subordinate);

        } catch (const RankingException& e) {
            // Rollback on failure
            pImpl->nodes = std::move(nodesBackup);
            pImpl->edges = std::move(edgesBackup);
            pImpl->directEdges = std::move(directEdgesBackup);
            throw;
        }
    }

    void CharacterRankDAG::removeRelation(const std::string& superior, const std::string& subordinate) {
        // Backup current state
        auto edgesBackup = pImpl->edges;
        auto directEdgesBackup = pImpl->directEdges;

        // Perform operation
        pImpl->directEdges[superior].erase(subordinate);
        pImpl->edges = pImpl->directEdges;
        pImpl->recomputeTransitiveClosure();

        // Exception check after operation
        if (edgesBackup.count(superior) == 0 || edgesBackup[superior].count(subordinate) == 0) {
            // Rollback on failure
            pImpl->edges = std::move(edgesBackup);
            pImpl->directEdges = std::move(directEdgesBackup);
            throw RankingException("Relation does not exist.");
        }
    }

    // Operator overloads
    CharacterRankDAG& CharacterRankDAG::operator+=(const std::string& name) { 
        createCharacter(name); 
        return *this; 
    }
    CharacterRankDAG& CharacterRankDAG::operator-=(const std::string& name) {
        deleteCharacter(name); 
        return *this;
    }
    CharacterRankDAG& CharacterRankDAG::operator*=(const std::pair<std::string, std::string>& names) {
        updateCharacter(names.first, names.second);
        return *this;
    }
    CharacterRankDAG& CharacterRankDAG::operator%=(std::pair<std::string, std::string> rel) {
        removeRelation(rel.first, rel.second);
        return *this;
    }
    bool CharacterRankDAG::operator[](const std::string& name) const {
        return pImpl->nodes.count(name) > 0;
    }
    void CharacterRankDAG::operator!() {
        pImpl->nodes.clear();
        pImpl->edges.clear();
    }

    // Comparison
    bool CharacterRankDAG::operator<(const CharacterRankDAG& other) const {
        // Compare by nodes first
        if (pImpl->nodes != other.pImpl->nodes) {
            return pImpl->nodes < other.pImpl->nodes;
        }

        // Then compare edges (lexicographical)
        return pImpl->edges < other.pImpl->edges;
}

    bool CharacterRankDAG::operator==(const CharacterRankDAG& other) const {
        return pImpl->nodes == other.pImpl->nodes && pImpl->edges == other.pImpl->edges;
    }

    bool CharacterRankDAG::operator!=(const CharacterRankDAG& other) const {
        return !(*this == other);
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

    // Topological Sort
    std::vector<std::string> CharacterRankDAG::getRanking() const {
        std::map<std::string, int> inDegree;
        for (const auto& node : pImpl->nodes) {
            inDegree[node] = 0;
        }
        for (const auto& pair : pImpl->edges) {
            for (const auto& neighbor : pair.second) {
                inDegree[neighbor]++;
            }
        }

        std::queue<std::string> q;
        for (const auto& pair : inDegree) {
            if (pair.second == 0) {
                q.push(pair.first);
            }
        }

        std::vector<std::string> result;
        while (!q.empty()) {
            std::string current = q.front(); q.pop();
            result.push_back(current);
            if (pImpl->edges.count(current)) {
                for (const auto& neighbor : pImpl->edges.at(current)) {
                    if (--inDegree[neighbor] == 0) {
                        q.push(neighbor);
                    }
                }
            }
        }

        if (result.size() != pImpl->nodes.size()) {
            throw RankingException("Cycle detected during topological sort.");
        }

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