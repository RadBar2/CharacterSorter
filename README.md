# Overview
Transitive Character Ranking Calculator based on a Directed Acyclic Graph (DAG) ADT. This structure handles transitive relations (if A > B and B > C, then A > C).

# UML Diagram
```mermaid
classDiagram
    %% Main class
    class CharacterRankDAG {
        - Impl* pImpl
        + CharacterRankDAG()
        + CharacterRankDAG(const CharacterRankDAG&)
        + operator=(const CharacterRankDAG&)
        + ~CharacterRankDAG()
        + createCharacter(string)
        + readCharacter(string) const
        + updateCharacter(string, string)
        + deleteCharacter(string)
        + addRelation(string, string)
        + removeRelation(string, string)
        + operator+=(string)
        + operator-=(string)
        + operator*=(pair<string,string>)
        + operator%=(pair<string,string>)
        + operator[](string) const : bool
        + operator!()
        + operator==(CharacterRankDAG) const : bool
        + operator!=(CharacterRankDAG) const : bool
        + operator<(CharacterRankDAG) const : bool
        + operator<=(CharacterRankDAG) const : bool
        + operator>(CharacterRankDAG) const : bool
        + operator>=(CharacterRankDAG) const : bool
        + toString() const : string
    }

    %% Impl class
    class Impl {
        - nodes : set<string>
        - edges : map<string, set<string>>
        - directEdges : map<string, set<string>>
        - hasPath(a,b) : bool
        - enforceAcyclic(a,b)
        - recomputeTransitiveClosure()
        - dfs(origin, current, visited, newEdges)
    }

    %% Exception class
    class RankingException {
        + RankingException(string msg)
    }

    %% Inheritance
    RankingException --|> std::runtime_error

    %% Composition / Pimpl relationship
    CharacterRankDAG --> Impl : uses / pImpl
```

# Make options
- make -f makefile.txt            builds default target (module + demo)
- make -f makefile.txt run-demo   runs demo
- make -f makefile.txt run-test   runs test
- make -f makefile.txt clean      cleans all
- make -f makefile.txt rebuild    rebuilds from scratch



