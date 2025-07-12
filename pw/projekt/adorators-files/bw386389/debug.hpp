#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iomanip>
const int LOG_LIMIT = 6000;

std::ostream& operator<<(std::ostream &os, const Graph::Edge &e) {
    os << "to " << e.to << " weighing " << e.weight;
    return os;
}

extern Graph g;

namespace Debug {
class LogStream : protected std::ofstream {
private:
    std::string name;
    int log_limit;
public:
    LogStream(std::string name, int log_limit) : name(name), log_limit(log_limit) {
        open(name);
    }

    std::ofstream& use() {
        log_limit--;
        if(log_limit < 0)
            throw std::ios_base::failure("Log limit exceeded.");
        return *this;
    }

    ~LogStream() {
        close();
    }
} log("log.out", LOG_LIMIT);


void print(std::ostream &os, const std::priority_queue<Graph::Edge> &v, size_t depth=0, size_t i=0) {
    os << "(";
    if (v.size() < g.B[i])
        os << "EMPTY";
    else if (!v.empty())
        os << v.top();
    os << ")";
}

void printd(std::ostream& os, size_t depth){
    if(depth != 0)
        os << std::setfill(' ') << std::setw(2*depth - 1) << " ";
}

template<typename T>
typename std::enable_if<std::is_trivial<T>::value || std::is_same<T, Graph::Edge>::value, void>::type print(std::ostream &os, const T &v, size_t depth=0, size_t i=0) {
    printd(os, depth);
    os << v;
}

template<typename T>
typename std::enable_if<!std::is_trivial<T>::value && !std::is_same<T, Graph::Edge>::value, void>::type print(std::ostream &os, const T &v, size_t depth=0, size_t i=0) {
    size_t idx = 0;
    os << "[";

    for(auto &t : v) {
        if(idx == 0)
            os << std::endl;
        if(depth == 0)
            os << idx;
        printd(os, depth);
        print(os, t, depth + 1, idx);
        os << std::endl;
        idx++;
    }
    os << "]" << std::endl;
}

void printb(std::ostream& os, Graph g) {
    os << "B: ";
    Debug::print(os, g.B);
}

}

std::ostream& operator<<(std::ostream &os, Graph g) {
    os << "Edges: ";
    Debug::print(os, g.edges);

    os << "BValues: ";
    Debug::print(os, g.B);

    os << "S: ";
    Debug::print(os, g.S);

    os << "T: ";
    Debug::print(os, g.T);
    return os;
}


#endif // DEBUG_HPP
