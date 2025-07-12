#ifndef ADORATE_42_CPP

#include "blimit.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include <unordered_map>
#include <map>

#define INFO
#ifdef INFO
#define COM(s) std::cerr << s << std::endl
#elif
#define COM(...) {}
#endif

using index_t = unsigned long;
using weight_t = int;
using bvalue_t = unsigned int;

template<typename T>
using V = std::vector<T>;

const size_t not_found = -1;
const index_t null_index = -1;

V<index_t> q, r;

class Graph {
public:
    struct Edge {
    public:
        index_t to;
        weight_t weight;
        bool operator<(const Edge &other) const {
            if (weight != other.weight)
                return weight > other.weight;
            return to > other.to;
        }
    };

    using VVE = V<V<Edge>>;
    using VSI = V<std::set<index_t>>;
    using VB = V<bvalue_t>;
    using PE = std::priority_queue<Edge>;
    using VPE = V<PE>;
    using VIND = V<index_t>;

    VVE edges;
    VSI T;
    VPE S;
    VB B;
    VIND backmapping;
    std::deque<std::mutex> M;

    void read(std::string filename);

    const Edge* last(const index_t u) const {
        if(S[u].size() == B[u])
            return &S[u].top();
        return NULL;
    }

    void fit(size_t num_of_nodes) {
        edges.resize(num_of_nodes);
        B.resize(num_of_nodes);
        backmapping.resize(num_of_nodes);
        T.resize(num_of_nodes);
        S.resize(num_of_nodes);
        M.resize(num_of_nodes);

        M.shrink_to_fit();
        S.shrink_to_fit();
        T.shrink_to_fit();
        backmapping.shrink_to_fit();
        B.shrink_to_fit();
        edges.shrink_to_fit();
    }

    weight_t sum() {
        weight_t w = 0;
        for(auto &pq: S) {
            while(!pq.empty()) {
                w += pq.top().weight;
                pq.pop();
            }
        }
        clear();
        return w/2;
    }

    const V<Edge>& operator[](size_t idx) const {
        return edges[idx];
    }

    void remove(const index_t &y, const index_t &x) {
        T[y].erase(T[y].find(x));
    }

    bool t_contains(const index_t &u, const index_t &other) const {
        return T[u].find(other) != T[u].end();
    }

    void init(int b_method) {
        size_t idx = 0;
        for(auto &b : B) {
            b = bvalue(b_method, backmapping[idx]);
            idx++;
        }
    }

private:
    void clear() {
        for(auto &t : T)
            t.clear();

        for(auto &s : S)
            while(!s.empty()) {
                s.pop();
            }
    }

} g;


struct UndirectedEdge {
    index_t from, to;
    weight_t weight;
};

class FileStream : public std::ifstream {
public:
    FileStream(std::string input_filename) {
        open(input_filename);
    }
    ~FileStream() {
        close();
    }
};

//O(ElogV)
void Graph::read(std::string filename) {
    FileStream file (filename);
    std::string input;
    index_t x, y;
    weight_t w;

    std::vector<UndirectedEdge> vue;
    std::set<index_t> sind;
    std::unordered_map<index_t, index_t> mapping;

    while (getline(file, input)) {
        std::stringstream ss(input);
        if (std::isdigit(input[0])) {
            ss >> x >> y >> w; // O(E)

            vue.push_back({x, y, w}); // O(E)
            sind.insert(x); // O(ElogV)
            sind.insert(y);
        }
    }

    fit(sind.size());

    size_t counter = 0;
    for(index_t ind : sind) {
        mapping[ind] = counter;
        backmapping[counter] = ind;
        counter++;
    }

    while(!vue.empty()) {
        UndirectedEdge &b = vue.back();
        index_t &from = mapping[b.from];
        index_t &to = mapping[b.to];

        edges[from].push_back({to, b.weight});
        edges[to].push_back({from, b.weight});
        vue.pop_back();
    }

    // Memory reduction
    for(auto &e : edges) {
        e.shrink_to_fit();
        std::sort(e.begin(),e.end()); // O(ElogV)
    }
}

// O(1)
bool wcmp(const Graph::Edge *sv, const Graph::Edge &e, const index_t &u) {
    if(sv == NULL)
        return true;
    if(sv->weight != e.weight)
        return e.weight > sv->weight;
    return u > sv->to;
}

const size_t argmax(const index_t u) {
    size_t idx = 0;
    for (const Graph::Edge &e : g[u]) {
        const index_t &v = e.to;
        const Graph::Edge *sv = g.last(v);

        if (!g.t_contains(u, v) && wcmp(sv, e, u) && g.B[v] > 0) // drugi warunek
            return idx;

        idx++;
    }
    return not_found;
}

void q_iter(const index_t u) {
    while (g.T[u].size() < g.B[u]) {
        size_t idx = argmax(u);

        if(idx == null_index)
            break;
        const index_t x = g[u][idx].to;
        const weight_t w = g[u][idx].weight;

        //g.M[x].lock();
        if(argmax(u) == idx) {
            const Graph::Edge *ey = g.last(x);
            index_t y = null_index;

            if(ey != NULL) {
                assert(ey->weight <= w);
                y = ey->to;
                g.S[x].pop();
            }

            g.S[x].push({u, w});
            g.T[u].insert(x);

            if (y != null_index) {
                g.remove(y, x);
                r.push_back(y);
            }
        }
        //g.M[x].unlock();
    }
}

int b_adorate(int b_method, int thread_count) {
    g.init(b_method);

    size_t idx = g.edges.size();
    while(idx > 0) {
        if(g.B[idx-1] > 0)
            q.push_back(idx-1);
        idx--;
    }

    while (!q.empty()) {
        for (index_t u : q)
           q_iter(u);

        swap(q, r);
        //COM(q.size());
        r.clear();
    }

    weight_t w = g.sum();
    return w;

}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "usage: "<<argv[0]<<" thread-count inputfile b-limit"<< std::endl;
        return 1;
    }

    int thread_count = std::stoi(argv[1]);
    int b_limit = std::stoi(argv[3]);
    std::string input_filename{argv[2]};

    g.read(input_filename);
    COM("Loaded successfully");

    for (int b_method = 0; b_method < b_limit + 1; b_method++) {
        std::cout << b_adorate(b_method, thread_count) << std::endl;
    }
}

#endif // ADORATE_42_CPP
