/**
 * ====================================================================================
 * SMART CAMPUS NAVIGATION AND EMERGENCY ROUTE MANAGEMENT SYSTEM (C++)
 * ====================================================================================
 * An academic-grade terminal implementation of core Data Structures & Algorithms.
 * * Featured Custom Implementations (Built from scratch):
 * 1. Weighted Graph via Adjacency List
 * 2. Chaining Hash Map (Collision Resolution buckets)
 * 3. Binary Min-Heap Priority Queue (Dijkstra optimizer)
 * 4. Search History Call Stack (LIFO Tracker)
 * 5. Dispatcher Buffer Queue (FIFO Scheduler)
 * * Features:
 * - Interactive Canvas ASCII Art Layout map
 * - Dynamic Road Obstruction module (Toggle blocks)
 * - Shortest Path solver (Dijkstra algorithm)
 * - Real-time step-by-step visualizer (Heap/Relaxation animator)
 * - Emergency locator hotkeys
 * ====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>

// ANSI Colors for immersive styling
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define BLUE        "\033[1;34m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[1;36m"
#define WHITE       "\033[1;37m"
#define BG_SLATE    "\033[48;5;235m"

const int INFINITY_VAL = 999999;

// ===================================================================
// 1. DATA STRUCTURE: Campus Node Representation
// ===================================================================
struct CampusNode {
    std::string id;
    std::string name;
    std::string type; // "hostel", "academic", "amenity", "medical", "sports", "security", "gate"
    int x, y;         // Positional coordinates
};

struct Edge {
    std::string to;
    int weight;
    bool blocked;
};

// ===================================================================
// 2. CUSTOM HASH MAP WITH CHAINING COLLISION RESOLUTION
// ===================================================================
struct HashNode {
    std::string key;
    CampusNode value;
    HashNode* next;
    HashNode(std::string k, CampusNode v) : key(k), value(v), next(nullptr) {}
};

class CampusHashMap {
private:
    static const int BUCKET_SIZE = 8;
    HashNode* buckets[BUCKET_SIZE];

    // Simple Hash Function matching modulo rules
    int _hash(const std::string& key) {
        int hashValue = 0;
        for (size_t i = 0; i < key.length(); i++) {
            hashValue += static_cast<int>(key[i]) * (i + 1);
        }
        return hashValue % BUCKET_SIZE;
    }

public:
    CampusHashMap() {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            buckets[i] = nullptr;
        }
    }

    ~CampusHashMap() {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
        }
    }

    void set(const std::string& key, const CampusNode& value) {
        int idx = _hash(key);
        HashNode* curr = buckets[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }
        HashNode* newNode = new HashNode(key, value);
        newNode->next = buckets[idx];
        buckets[idx] = newNode;
    }

    bool get(const std::string& key, CampusNode& outVal) {
        int idx = _hash(key);
        HashNode* curr = buckets[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                outVal = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    std::vector<CampusNode> values() {
        std::vector<CampusNode> results;
        for (int i = 0; i < BUCKET_SIZE; i++) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                results.push_back(curr->value);
                curr = curr->next;
            }
        }
        return results;
    }

    std::vector<std::string> keys() {
        std::vector<std::string> results;
        for (int i = 0; i < BUCKET_SIZE; i++) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                results.push_back(curr->key);
                curr = curr->next;
            }
        }
        return results;
    }

    void printBucketDiagnostics() {
        std::cout << BOLD << CYAN << "\n --- CAMPUS HASH TABLE DIAGNOSTICS (Modulo 8 Chaining) ---\n" << RESET;
        for (int i = 0; i < BUCKET_SIZE; i++) {
            std::cout << " Bucket [" << i << "]: ";
            HashNode* curr = buckets[i];
            if (curr == nullptr) {
                std::cout << RESET << " [Empty Bucket]\n";
            } else {
                while (curr != nullptr) {
                    std::cout << GREEN << "\"" << curr->key << "\" " << RESET 
                              << "(ID: " << curr->value.id << ") " 
                              << YELLOW << "-> " << RESET;
                    curr = curr->next;
                }
                std::cout << "NULL\n";
            }
        }
    }
};

// ===================================================================
// 3. BINARY MIN-HEAP PRIORITY QUEUE
// ===================================================================
struct HeapItem {
    std::string nodeId;
    int distance;
};

class MinHeap {
private:
    std::vector<HeapItem> heap;

    int getParentIndex(int index) { return (index - 1) / 2; }
    int getLeftChildIndex(int index) { return 2 * index + 1; }
    int getRightChildIndex(int index) { return 2 * index + 2; }

    void bubbleUp(int index) {
        while (index > 0 && heap[index].distance < heap[getParentIndex(index)].distance) {
            std::swap(heap[index], heap[getParentIndex(index)]);
            index = getParentIndex(index);
        }
    }

    void bubbleDown(int index) {
        int smallest = index;
        int left = getLeftChildIndex(index);
        int right = getRightChildIndex(index);
        int length = heap.size();

        if (left < length && heap[left].distance < heap[smallest].distance) {
            smallest = left;
        }
        if (right < length && heap[right].distance < heap[smallest].distance) {
            smallest = right;
        }

        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            bubbleDown(smallest);
        }
    }

public:
    void insert(const std::string& nodeId, int distance) {
        heap.push_back({nodeId, distance});
        bubbleUp(heap.size() - 1);
    }

    HeapItem extractMin() {
        if (heap.empty()) return {"", -1};
        HeapItem minItem = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            bubbleDown(0);
        }
        return minItem;
    }

    bool isEmpty() {
        return heap.empty();
    }

    int size() {
        return heap.size();
    }

    void printHeapState() {
        if (heap.empty()) {
            std::cout << "[Empty Heap]";
            return;
        }
        for (size_t i = 0; i < heap.size(); i++) {
            std::cout << "[" << heap[i].nodeId << ": " << heap[i].distance << "m] ";
        }
    }
};

// ===================================================================
// 4. WEIGHTED CAMPUS GRAPH REPRESENTATION
// ===================================================================
class CampusGraph {
public:
    CampusHashMap nodes;
    std::map<std::string, std::vector<Edge>> adjacencyList;

    void addNode(const std::string& id, const std::string& name, const std::string& type, int x, int y) {
        nodes.set(id, {id, name, type, x, y});
        if (adjacencyList.find(id) == adjacencyList.end()) {
            adjacencyList[id] = std::vector<Edge>();
        }
    }

    void addEdge(const std::string& fromId, const std::string& toId, int weight) {
        adjacencyList[fromId].push_back({toId, weight, false});
        adjacencyList[toId].push_back({fromId, weight, false});
    }

    void setBlockRoadState(const std::string& fromId, const std::string& toId, bool isBlocked) {
        if (adjacencyList.find(fromId) != adjacencyList.end()) {
            for (auto& edge : adjacencyList[fromId]) {
                if (edge.to == toId) edge.blocked = isBlocked;
            }
        }
        if (adjacencyList.find(toId) != adjacencyList.end()) {
            for (auto& edge : adjacencyList[toId]) {
                if (edge.to == fromId) edge.blocked = isBlocked;
            }
        }
    }

    std::vector<Edge> getNeighbors(const std::string& nodeId) {
        std::vector<Edge> validNeighbors;
        if (adjacencyList.find(nodeId) != adjacencyList.end()) {
            for (const auto& edge : adjacencyList[nodeId]) {
                if (!edge.blocked) {
                    validNeighbors.push_back(edge);
                }
            }
        }
        return validNeighbors;
    }

    void printAdjacencyList() {
        std::cout << BOLD << CYAN << "\n --- CAMPUS ADJACENCY LIST (WEIGHTS) ---\n" << RESET;
        for (const auto& pair : adjacencyList) {
            std::string id = pair.first;
            CampusNode node;
            nodes.get(id, node);
            std::cout << " " << BOLD << node.name << RESET << " (ID: " << id << ") -> ";
            
            bool first = true;
            for (const auto& edge : pair.second) {
                if (!first) std::cout << ", ";
                CampusNode dest;
                nodes.get(edge.to, dest);
                if (edge.blocked) {
                    std::cout << RED << "[BLOCKED] " << dest.name << " (" << edge.weight << "m)" << RESET;
                } else {
                    std::cout << GREEN << dest.name << " (" << edge.weight << "m)" << RESET;
                }
                first = false;
            }
            std::cout << "\n";
        }
    }
};

// ===================================================================
// 5. SEARCH HISTORY STACK (LIFO)
// ===================================================================
struct HistoryFrame {
    std::string timestamp;
    std::string source;
    std::string dest;
    std::vector<std::string> path;
    int distance;
};

class NavigationStack {
private:
    std::vector<HistoryFrame> frames;

public:
    void push(const HistoryFrame& frame) {
        frames.push_back(frame);
    }

    bool pop(HistoryFrame& outFrame) {
        if (frames.empty()) return false;
        outFrame = frames.back();
        frames.pop_back();
        return true;
    }

    bool isEmpty() {
        return frames.empty();
    }

    void printStackHistory() {
        std::cout << BOLD << CYAN << "\n --- TRACEBACK STACK HISTORY (LIFO) ---\n" << RESET;
        if (frames.empty()) {
            std::cout << " [Stack trace empty. Calculate routes to populate history frames]\n";
            return;
        }

        for (int i = static_cast<int>(frames.size()) - 1; i >= 0; i--) {
            const auto& frame = frames[i];
            std::cout << " " << (i == static_cast<int>(frames.size()) - 1 ? BOLD YELLOW " [TOP FRAME]" : " [FRAME]") << RESET << "\n";
            std::cout << "   Time     : " << frame.timestamp << "\n";
            std::cout << "   Route    : " << BOLD << frame.source << RESET << " -> " << BOLD << frame.dest << RESET << "\n";
            std::cout << "   Distance : " << frame.distance << " meters\n";
            std::cout << "   Path     : ";
            for (size_t p = 0; p < frame.path.size(); p++) {
                if (p > 0) std::cout << " -> ";
                std::cout << frame.path[p];
            }
            std::cout << "\n\n";
        }
    }
};

// ===================================================================
// 6. TRANSIT BUFFER REQUEST QUEUE (FIFO)
// ===================================================================
struct TransitRequest {
    int id;
    std::string source;
    std::string dest;
    std::string type; // "Normal", "Security", "Emergency"
    std::string timestamp;
};

class RequestQueue {
private:
    std::vector<TransitRequest> elements;

public:
    void enqueue(const TransitRequest& req) {
        elements.push_back(req);
    }

    bool dequeue(TransitRequest& outReq) {
        if (elements.empty()) return false;
        outReq = elements.front();
        elements.erase(elements.begin());
        return true;
    }

    bool isEmpty() {
        return elements.empty();
    }

    int size() {
        return elements.size();
    }

    void printQueueBuffer() {
        std::cout << BOLD << CYAN << "\n --- TRANSIT BUFFER REQUEST QUEUE (FIFO) ---\n" << RESET;
        if (elements.empty()) {
            std::cout << " [Buffer empty. No pending transit route requests]\n";
            return;
        }

        for (size_t i = 0; i < elements.size(); i++) {
            const auto& req = elements[i];
            std::string prefix = " [Q-Slot " + std::to_string(i) + "]";
            if (i == 0) prefix = " [HEAD]";
            
            std::cout << BOLD << (i == 0 ? YELLOW : WHITE) << prefix << RESET << "\n";
            std::cout << "   Request ID : #" << req.id << "\n";
            std::cout << "   Priority   : ";
            if (req.type == "Emergency") std::cout << RED << BOLD << "EMERGENCY" << RESET << "\n";
            else if (req.type == "Security") std::cout << YELLOW << "SECURITY" << RESET << "\n";
            else std::cout << GREEN << "NORMAL" << RESET << "\n";
            std::cout << "   Transit    : " << req.source << " -> " << req.dest << "\n";
            std::cout << "   Timestamp  : " << req.timestamp << "\n\n";
        }
    }
};

// ===================================================================
// 7. DIJKSTRA SYSTEM CORE LOGIC & TRACING
// ===================================================================

std::string getTimestampString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm* parts = std::localtime(&now_c);
    char buf[128];
    std::strftime(buf, sizeof(buf), "%X", parts);
    return std::string(buf);
}

struct DijkstraResult {
    std::vector<std::string> path;
    int distance;
};

// Compute optimal Dijkstra short route
DijkstraResult computeDijkstra(CampusGraph& graph, const std::string& startId, const std::string& targetId) {
    std::map<std::string, int> distances;
    std::map<std::string, std::string> previous;
    MinHeap minHeap;

    std::vector<std::string> keys = graph.nodes.keys();
    for (const auto& key : keys) {
        distances[key] = INFINITY_VAL;
        previous[key] = "";
    }

    distances[startId] = 0;
    minHeap.insert(startId, 0);

    while (!minHeap.isEmpty()) {
        HeapItem current = minHeap.extractMin();
        std::string u = current.nodeId;
        int dist = current.distance;

        if (u == targetId) break;
        if (dist > distances[u]) continue;

        std::vector<Edge> neighbors = graph.getNeighbors(u);
        for (const auto& edge : neighbors) {
            int altDist = distances[u] + edge.weight;
            if (altDist < distances[edge.to]) {
                distances[edge.to] = altDist;
                previous[edge.to] = u;
                minHeap.insert(edge.to, altDist);
            }
        }
    }

    DijkstraResult res;
    std::string current = targetId;
    while (current != "") {
        res.path.insert(res.path.begin(), current);
        current = previous[current];
    }

    if (res.path.empty() || res.path[0] != startId) {
        res.path.clear();
        res.distance = -1;
    } else {
        res.distance = distances[targetId];
    }

    return res;
}

// Visual real-time trace walkthrough execution
void runDijkstraVisualWalkthrough(CampusGraph& graph, const std::string& startId, const std::string& targetId) {
    std::map<std::string, int> distances;
    std::map<std::string, std::string> previous;
    MinHeap minHeap;

    std::cout << "\033[2J\033[H";
    std::cout << BOLD << YELLOW << "========================================================================\n";
    std::cout << "           DIJKSTRA VISUAL WALKTHROUGH ENGINE (O(E log V))\n";
    std::cout << "========================================================================\n" << RESET;

    std::vector<std::string> keys = graph.nodes.keys();
    for (const auto& key : keys) {
        distances[key] = INFINITY_VAL;
        previous[key] = "";
    }

    distances[startId] = 0;
    minHeap.insert(startId, 0);

    std::cout << " * Initializing shortest distances map arrays...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    int stepCount = 1;
    while (!minHeap.isEmpty()) {
        std::cout << "\n" << BOLD << CYAN << "[STEP " << stepCount++ << "] " << RESET;
        std::cout << "Priority Heap Current: ";
        minHeap.printHeapState();
        std::cout << "\n";

        HeapItem current = minHeap.extractMin();
        std::string u = current.nodeId;
        int dist = current.distance;

        CampusNode nodeU;
        graph.nodes.get(u, nodeU);

        std::cout << " -> " << BOLD << GREEN << "Extracted " << nodeU.name << RESET 
                  << " (Distance: " << dist << "m)\n";

        if (u == targetId) {
            std::cout << BOLD << GREEN << " -> Target reached! Walkthrough complete.\n" << RESET;
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            break;
        }

        if (dist > distances[u]) {
            std::cout << "   Skipping (Already processed a shorter path to " << nodeU.name << ")\n";
            continue;
        }

        std::vector<Edge> neighbors = graph.getNeighbors(u);
        std::cout << "   Inspecting valid connections:\n";
        
        for (const auto& edge : neighbors) {
            CampusNode neighborNode;
            graph.nodes.get(edge.to, neighborNode);

            int altDist = distances[u] + edge.weight;
            std::cout << "     Checking edge to: " << neighborNode.name << " (" << edge.weight << "m)";
            
            if (altDist < distances[edge.to]) {
                distances[edge.to] = altDist;
                previous[edge.to] = u;
                minHeap.insert(edge.to, altDist);
                std::cout << BOLD << GREEN << " -> RELAXED. " << RESET << "New shortest distance: " << altDist << "m\n";
            } else {
                std::cout << YELLOW << " -> Unchanged." << RESET << " (Current: " << (distances[edge.to] == INFINITY_VAL ? "Inf" : std::to_string(distances[edge.to])) << "m is optimal)\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore();
    std::cin.get();
}

// ===================================================================
// 8. GRAPHICAL ASCII MAP DRAWER
// ===================================================================
void drawAsciiCampusMap() {
    std::cout << BOLD << MAGENTA << "----------------------- INTERACTIVE CAMPUS SCHEMATIC -----------------------\n" << RESET;
    std::cout << "   [Library (100m)] ------- (80m) ------ [Academic Blocks] -- (50m) -- [Science Hub]\n";
    std::cout << "          |                                     |                        /\n";
    std::cout << "       (100m)                                 (90m)                    (70m)\n";
    std::cout << "          |                                     |                      /\n";
    std::cout << "   [Hostels Blocks] --- (110m) ---------- [Health Center]             /\n";
    std::cout << "          |                                     |                    /\n";
    std::cout << "       (150m)                                 (180m)               /\n";
    std::cout << "          |                                     |                 /\n";
    std::cout << "   [Main Cafeteria] -- (130m) -- [Admin] -- (110m) -- [Security Office]\n";
    std::cout << "          |                         |                        |\n";
    std::cout << "       (200m)                    (60m)                     (70m)\n";
    std::cout << "          |                         |                        |\n";
    std::cout << "   [Sports Arena] --------------------------------------- [Main Gate]\n";
    std::cout << "                        (250m) Bypass Connect\n";
    std::cout << BOLD << MAGENTA << "----------------------------------------------------------------------------\n" << RESET;
}

// ===================================================================
// 9. APP INITIALIZATION & MAIN EXECUTION
// ===================================================================
int main() {
    CampusGraph graph;
    NavigationStack searchHistory;
    RequestQueue requestQueue;

    // Load nodes and positions match spec
    graph.addNode("hostel", "Hostel Blocks", "hostel", 180, 160);
    graph.addNode("library", "Central Library", "academic", 320, 100);
    graph.addNode("academic", "Academic Complex", "academic", 480, 140);
    graph.addNode("canteen", "Main Cafeteria", "amenity", 180, 320);
    graph.addNode("medical", "Health Center", "medical", 580, 260);
    graph.addNode("sports", "Sports Arena", "sports", 360, 420);
    graph.addNode("auditorium", "Silver Jubilee Hall", "academic", 620, 100);
    graph.addNode("security", "Central Security Office", "security", 520, 420);
    graph.addNode("gate", "Main Campus Gate", "gate", 680, 340);
    graph.addNode("admin", "Admin Block", "academic", 360, 240);
    graph.addNode("science", "Science Research Hub", "academic", 460, 50);

    // Load Connections (Undirected roads)
    graph.addEdge("hostel", "library", 100);
    graph.addEdge("hostel", "canteen", 150);
    graph.addEdge("library", "academic", 80);
    graph.addEdge("academic", "auditorium", 120);
    graph.addEdge("canteen", "sports", 200);
    graph.addEdge("sports", "gate", 250);
    graph.addEdge("medical", "academic", 90);
    graph.addEdge("security", "gate", 70);

    // Extra dynamic links for routing complexity
    graph.addEdge("hostel", "medical", 110);
    graph.addEdge("academic", "science", 50);
    graph.addEdge("science", "library", 70);
    graph.addEdge("canteen", "admin", 130);
    graph.addEdge("admin", "security", 110);
    graph.addEdge("auditorium", "admin", 90);
    graph.addEdge("sports", "medical", 180);
    graph.addEdge("admin", "academic", 60);

    // Prepopulate Queue requests
    requestQueue.enqueue({1024, "Main Campus Gate", "Hostel Blocks", "Emergency", "20:10:05"});
    requestQueue.enqueue({1025, "Central Library", "Sports Arena", "Normal", "20:12:44"});

    std::string sysMessage = "System Loaded Successfully. Complete core operational status validated.";
    std::string sysMessageColor = GREEN;

    while (true) {
        // Clear terminal screen
        std::cout << "\033[2J\033[H";
        
        std::cout << BOLD << CYAN << "========================================================================\n";
        std::cout << "   SMART CAMPUS NAVIGATION AND EMERGENCY ROUTE MANAGEMENT SYSTEM (C++)\n";
        std::cout << "========================================================================\n" << RESET;

        // Visual layout render
        drawAsciiCampusMap();

        // Status banner
        std::cout << " STATUS LOG: " << sysMessageColor << BOLD << sysMessage << RESET << "\n";
        std::cout << " Core Engines: " << BOLD << GREEN << "Dijkstra Active " << RESET << "|"
                  << " Stack Cache: " << BOLD << YELLOW << "LIFO Operational " << RESET << "|"
                  << " Transit Buffer: " << BOLD << CYAN << "FIFO Operational\n" << RESET;
        std::cout << "------------------------------------------------------------------------\n";

        // General Navigation Dashboard Options
        std::cout << BOLD << WHITE << " 1. Route Finder (Quick shortest-path calculation)\n";
        std::cout << " 2. Step-By-Step Walkthrough Debugger (Watch Dijkstra in O(E log V))\n";
        std::cout << " 3. Emergency Mode (Route directly to nearest emergency service centers)\n";
        std::cout << " 4. Campus Corridor Obstructions (Block / Unblock corridor pathways)\n";
        std::cout << " 5. DSA Struct Inspector (Live Adjacency List & Custom HashMap bucket status)\n";
        std::cout << " 6. Search History (Traceback Stack frames - LIFO)\n";
        std::cout << " 7. Transit Dispatcher Buffer (FIFO request queue managers)\n";
        std::cout << " 8. Exit System application\n" << RESET;
        std::cout << "------------------------------------------------------------------------\n";
        std::cout << " Choose Operation (1-8): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            sysMessage = "Invalid format selection input.";
            sysMessageColor = RED;
            continue;
        }

        if (choice == 8) {
            std::cout << "\nShutting down Smart Campus Navigation core. Safe routing!\n";
            break;
        }

        switch (choice) {
            case 1: { // Quick shortest path find
                std::cout << "\n Available Nodes IDs:\n";
                auto list = graph.nodes.values();
                for (const auto& n : list) {
                    std::cout << "   - " << BOLD << n.id << RESET << " (" << n.name << ")\n";
                }
                std::string src, dest;
                std::cout << "\nEnter Starting Node ID: ";
                std::cin >> src;
                std::cout << "Enter Destination Node ID: ";
                std::cin >> dest;

                CampusNode nodeSrc, nodeDst;
                if (!graph.nodes.get(src, nodeSrc) || !graph.nodes.get(dest, nodeDst)) {
                    sysMessage = "Failed pathfind: invalid node identification strings.";
                    sysMessageColor = RED;
                    break;
                }

                DijkstraResult res = computeDijkstra(graph, src, dest);
                if (res.distance == -1) {
                    sysMessage = "Path route is currently obstructed by dynamic roadblocks.";
                    sysMessageColor = RED;
                } else {
                    // Log calculation into Navigation History LIFO Stack
                    HistoryFrame frame;
                    frame.timestamp = getTimestampString();
                    frame.source = nodeSrc.name;
                    frame.dest = nodeDst.name;
                    frame.distance = res.distance;
                    for (const auto& stepId : res.path) {
                        CampusNode pathNode;
                        graph.nodes.get(stepId, pathNode);
                        frame.path.push_back(pathNode.name);
                    }
                    searchHistory.push(frame);

                    std::stringstream ss;
                    ss << "SUCCESS: Route calculated " << nodeSrc.name << " to " << nodeDst.name 
                       << " (" << res.distance << "m, " << (res.path.size() - 1) << " steps)";
                    sysMessage = ss.str();
                    sysMessageColor = GREEN;
                }
                break;
            }

            case 2: { // Visual Trace Walkthrough
                std::string src, dest;
                std::cout << "\nEnter Starting Node ID: ";
                std::cin >> src;
                std::cout << "Enter Destination Node ID: ";
                std::cin >> dest;

                CampusNode nodeSrc, nodeDst;
                if (!graph.nodes.get(src, nodeSrc) || !graph.nodes.get(dest, nodeDst)) {
                    sysMessage = "Walkthrough aborted: invalid node inputs.";
                    sysMessageColor = RED;
                } else {
                    runDijkstraVisualWalkthrough(graph, src, dest);
                    sysMessage = "Interactive walkthrough playback completed successfully.";
                    sysMessageColor = GREEN;
                }
                break;
            }

            case 3: { // Emergency Hotkeys
                std::cout << "\n Available Nodes IDs:\n";
                auto list = graph.nodes.values();
                for (const auto& n : list) {
                    std::cout << "   - " << BOLD << n.id << RESET << " (" << n.name << ")\n";
                }
                std::string src;
                std::cout << "\nEnter Current Node ID: ";
                std::cin >> src;

                CampusNode nodeSrc;
                if (!graph.nodes.get(src, nodeSrc)) {
                    sysMessage = "Emergency lookup failure: invalid starting position ID.";
                    sysMessageColor = RED;
                    break;
                }

                std::cout << "\n Choose Nearest Facility Target:\n";
                std::cout << "  1. Medical Room (Health Center)\n";
                std::cout << "  2. Security Office\n";
                std::cout << "  3. Main Gate\n";
                std::cout << " Choice (1-3): ";
                int fChoice;
                std::cin >> fChoice;

                std::string filterType = "";
                if (fChoice == 1) filterType = "medical";
                else if (fChoice == 2) filterType = "security";
                else if (fChoice == 3) filterType = "gate";
                else {
                    sysMessage = "Invalid category selection.";
                    sysMessageColor = RED;
                    break;
                }

                auto allNodes = graph.nodes.values();
                int minDistance = INFINITY_VAL;
                DijkstraResult bestRes;
                std::string facilityName = "";

                for (const auto& node : allNodes) {
                    if (node.type == filterType) {
                        DijkstraResult temp = computeDijkstra(graph, src, node.id);
                        if (temp.distance != -1 && temp.distance < minDistance) {
                            minDistance = temp.distance;
                            bestRes = temp;
                            facilityName = node.name;
                        }
                    }
                }

                if (minDistance == INFINITY_VAL) {
                    sysMessage = "EMERGENCY: All facilities are currently inaccessible due to blocks.";
                    sysMessageColor = RED;
                } else {
                    // Push to stack history
                    HistoryFrame frame;
                    frame.timestamp = getTimestampString();
                    frame.source = nodeSrc.name;
                    frame.dest = "[EMERGENCY] " + facilityName;
                    frame.distance = minDistance;
                    for (const auto& stepId : bestRes.path) {
                        CampusNode pathNode;
                        graph.nodes.get(stepId, pathNode);
                        frame.path.push_back(pathNode.name);
                    }
                    searchHistory.push(frame);

                    std::stringstream ss;
                    ss << "EMERGENCY ROUTE: Safest access line to " << facilityName << " is " << minDistance << "m";
                    sysMessage = ss.str();
                    sysMessageColor = RED;
                }
                break;
            }

            case 4: { // Obstructions Block / Unblock Module
                std::cout << "\n Toggling roadblock obstructions. Select edge parameters:\n";
                std::string src, dest;
                std::cout << "Enter source node ID (e.g., 'hostel'): ";
                std::cin >> src;
                std::cout << "Enter destination node ID (e.g., 'library'): ";
                std::cin >> dest;

                CampusNode nodeSrc, nodeDst;
                if (!graph.nodes.get(src, nodeSrc) || !graph.nodes.get(dest, nodeDst)) {
                    sysMessage = "Link check failed: invalid edge definitions.";
                    sysMessageColor = RED;
                    break;
                }

                std::cout << "Set Corridor Blocked state (1 = Blocked, 0 = Active/Clear): ";
                int blockState;
                std::cin >> blockState;
                bool isBlocked = (blockState == 1);

                graph.setBlockRoadState(src, dest, isBlocked);
                
                std::stringstream ss;
                ss << "Roadblock updated: corridor between " << nodeSrc.name << " & " << nodeDst.name 
                   << " is now " << (isBlocked ? "BLOCKED" : "CLEARED");
                sysMessage = ss.str();
                sysMessageColor = isBlocked ? YELLOW : GREEN;
                break;
            }

            case 5: { // Structural Inspector View
                std::cout << "\033[2J\033[H";
                graph.nodes.printBucketDiagnostics();
                graph.printAdjacencyList();
                std::cout << "\nPress Enter to return to main dashboard...";
                std::cin.ignore();
                std::cin.get();
                break;
            }

            case 6: { // Navigation history stack tracer (LIFO)
                std::cout << "\033[2J\033[H";
                searchHistory.printStackHistory();
                
                std::cout << " Options: \n";
                std::cout << "  1. Pop recent history frame from LIFO stack\n";
                std::cout << "  2. Return to dashboard\n";
                std::cout << "  Choice (1-2): ";
                int hChoice;
                std::cin >> hChoice;
                if (hChoice == 1) {
                    HistoryFrame popped;
                    if (searchHistory.pop(popped)) {
                        std::stringstream ss;
                        ss << "POPPED: Discarded frame trace (" << popped.source << " to " << popped.dest << ")";
                        sysMessage = ss.str();
                        sysMessageColor = YELLOW;
                    } else {
                        sysMessage = "Pop failed: history trace stack is empty.";
                        sysMessageColor = RED;
                    }
                }
                break;
            }

            case 7: { // Dispatcher queue managers (FIFO)
                std::cout << "\033[2J\033[H";
                requestQueue.printQueueBuffer();

                std::cout << " Options: \n";
                std::cout << "  1. Enqueue generic request\n";
                std::cout << "  2. Process oldest head request (FIFO pop)\n";
                std::cout << "  3. Return to dashboard\n";
                std::cout << "  Choice (1-3): ";
                int qChoice;
                std::cin >> qChoice;

                if (qChoice == 1) {
                    TransitRequest req;
                    req.id = rand() % 9000 + 1000;
                    req.source = "Main Cafeteria";
                    req.dest = "Central Security Office";
                    req.type = (rand() % 2 == 0) ? "Normal" : "Security";
                    req.timestamp = getTimestampString();
                    requestQueue.enqueue(req);
                    sysMessage = "Request successfully enqueued onto FIFO buffer dispatcher.";
                    sysMessageColor = GREEN;
                } else if (qChoice == 2) {
                    TransitRequest req;
                    if (requestQueue.dequeue(req)) {
                        std::stringstream ss;
                        ss << "DEQUEUED: Processed Head Req #" << req.id << " (" << req.source << " -> " << req.dest << ")";
                        sysMessage = ss.str();
                        sysMessageColor = GREEN;
                    } else {
                        sysMessage = "Buffer Queue is empty. No pending actions.";
                        sysMessageColor = RED;
                    }
                }
                break;
            }

            default:
                sysMessage = "Invalid choice setting input. Please try again.";
                sysMessageColor = RED;
                break;
        }
    }

    return 0;
}