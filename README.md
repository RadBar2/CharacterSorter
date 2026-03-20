# Overview
Transitive Character Ranking Calculator based on a Directed Acyclic Graph (DAG) ADT. This structure handles transitive relations (if A > B and B > C, then A > C).

## UML Diagram
```mermaid
classDiagram
    %% Main class
    class CharacterRankDAG {
        - Impl_ptr pImpl
        + CharacterRankDAG()
        + CharacterRankDAG(const CharacterRankDAG&)
        + operator=(const CharacterRankDAG&)
        + ~CharacterRankDAG()
        + createCharacter(String)
        + readCharacter(String) const
        + updateCharacter(String, String)
        + deleteCharacter(String)
        + addRelation(String, String)
        + removeRelation(String, String)
        + operator+=(String)
        + operator-=(String)
        + operator*=(PairString)
        + operator%=(PairString)
        + operator[](String) const : bool
        + operator!()
        + operator==(CharacterRankDAG) const : bool
        + operator!=(CharacterRankDAG) const : bool
        + operator<(CharacterRankDAG) const : bool
        + operator<=(CharacterRankDAG) const : bool
        + operator>(CharacterRankDAG) const : bool
        + operator>=(CharacterRankDAG) const : bool
        + toString() const : String
    }

    %% Impl class
    class Impl {
        - nodes : SetString
        - edges : MapStringSet
        - directEdges : MapStringSet
        - hasPath(a,b) : bool
        - enforceAcyclic(a,b)
        - recomputeTransitiveClosure()
        - dfs(origin, current, visited, newEdges)
    }

    %% Exception class
    class RankingException {
        + RankingException(String msg)
    }

    %% Placeholder for runtime_error
    class RuntimeError

    %% Inheritance
    RankingException --|> RuntimeError

    %% Composition / Pimpl relationship
    CharacterRankDAG --> Impl : uses / pImpl
```

## Make options
1. **Build default target (module + demo)**
   ```bash
   make -f makefile.txt
   ```
2. **Run Demo**
   ```bash
   make -f makefile.txt run-demo
   ```
3. **Run Test**
   ```bash
   make -f makefile.txt run-test
   ``` 
4. **Cleans Everything**
   ```bash
   make -f makefile.txt clean
   ``` 
5. **Rebuilds from scratch**
   ```bash
   make -f makefile.txt rebuild
   ``` 
