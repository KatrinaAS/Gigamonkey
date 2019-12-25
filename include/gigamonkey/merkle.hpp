// Copyright (c) 2019 Daniel Krawisz
// Distributed under the Open BSV software license, see the accompanying file LICENSE.

#ifndef GIGAMONKEY_MERKLE
#define GIGAMONKEY_MERKLE

#include "hash.hpp"
#include "txid.hpp"

namespace gigamonkey::merkle {
    
    using digest = bitcoin::txid;
        
    digest concatinated(const digest&, const digest&);
        
    enum direction : byte {
        none = 0,
        left = 1, 
        right = 2
    };
    
    struct tree;
    
    class branch;
    
    class node {
        friend struct tree;
        branch* Parent;
    public:
        direction Direction;
        const digest& Left;
        const digest& Right;
        digest Digest;
        
        node(const digest& l, const digest& r) : Parent{nullptr}, Direction{none}, Left{l}, Right{r}, Digest{concatinated(l, r)} {}
        node(const digest& d) : node{d, d} {}
    };
    
    struct step {
        direction Direction;
        node Node;
    };
    
    class branch {
        friend struct tree;
        branch* Left;
        branch* Right;
        branch(node n) : Left{nullptr}, Right{nullptr}, Node{n} {}
        branch(branch* l, branch* r);
        branch(branch* b) : branch{b, b} {}
    public:
        node Node;
        ~branch() {
            delete Left;
            delete Right;
        }
    };
    
    // For a full node 
    struct tree {
        
        branch* Tree;
        map<digest&, branch*> Leaves; 
        list<digest> Elements;
        
        list<step> path(const digest&);
        
        tree() : Tree{nullptr}, Leaves{} {}
        tree(list<digest>);
        ~tree() {
            delete Tree;
        }
        
        digest root() {
            return Tree == nullptr ? digest{} : Tree->Node.Digest;
        }
    };
        
    inline digest root(list<digest> l) {
        return tree{l}.root();
    } 
    
    writer write(writer w, list<step>);
    bytes write(list<step>);
    
    // for an spv node. 
    struct path {
        digest Txid;
        list<step> Path;
        static path read(reader);
        
        static path read(const bytes& b) {
            return read(reader{b});
        }
        
        bool valid() const {
            return Path.size() > 0;
        }
        
    private:
        list<digest> Intermediate;
        path(list<step> p, list<digest> i) : Path{p}, Intermediate{i} {}
    };
    
}

#endif
