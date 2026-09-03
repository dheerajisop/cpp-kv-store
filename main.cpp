#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <vector>
#include <memory>
#include <sstream>

// Node structure for the Prefix Search Trie
struct TrieNode {
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;
    bool is_end = false;
};

// Trie data structure for fast key indexing and prefix matching
class PrefixTrie {
private:
    std::shared_ptr<TrieNode> root;

    void dfs(std::shared_ptr<TrieNode> node, std::string current_prefix, std::vector<std::string>& results) {
        if (!node) return;
        if (node->is_end) {
            results.push_back(current_prefix);
        }
        for (auto& [ch, child] : node->children) {
            dfs(child, current_prefix + ch, results);
        }
    }

public:
    PrefixTrie() : root(std::make_shared<TrieNode>()) {}

    void insert(const std::string& key) {
        auto current = root;
        for (char ch : key) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = std::make_shared<TrieNode>();
            }
            current = current->children[ch];
        }
        current->is_end = true;
    }

    void remove(const std::string& key) {
        auto current = root;
        for (char ch : key) {
            if (current->children.find(ch) == current->children.end()) return;
            current = current->children[ch];
        }
        current->is_end = false;
    }

    std::vector<std::string> get_keys_with_prefix(const std::string& prefix) {
        auto current = root;
        std::vector<std::string> results;
        for (char ch : prefix) {
            if (current->children.find(ch) == current->children.end()) {
                return results;
            }
            current = current->children[ch];
        }
        dfs(current, prefix, results);
        return results;
    }
};

// In-Memory Key-Value Store with LRU eviction and Thread Safety
class KeyValueStore {
private:
    size_t capacity;
    std::list<std::pair<std::string, std::string>> lru_list; // {key, value} pairs
    using ListIter = std::list<std::pair<std::string, std::string>>::iterator;
    std::unordered_map<std::string, ListIter> cache_map;
    PrefixTrie trie;
    mutable std::mutex store_mutex;

public:
    explicit KeyValueStore(size_t cap) : capacity(cap) {}

    // SET command: insert or update key-value pair
    void set(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(store_mutex);
        
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            // Update existing key and move to front (Most Recently Used)
            it->second->second = value;
            lru_list.splice(lru_list.begin(), lru_list, it->second);
            return;
        }

        // Evict LRU item if capacity limit reached
        if (cache_map.size() >= capacity) {
            auto last = lru_list.back();
            trie.remove(last.first);
            cache_map.erase(last.first);
            lru_list.pop_back();
        }

        // Add new key-value pair
        lru_list.push_front({key, value});
        cache_map[key] = lru_list.begin();
        trie.insert(key);
    }

    // GET command: fetch value by key
    bool get(const std::string& key, std::string& value) {
        std::lock_guard<std::mutex> lock(store_mutex);
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return false;
        }
        // Move accessed element to front of LRU list
        lru_list.splice(lru_list.begin(), lru_list, it->second);
        value = it->second->second;
        return true;
    }

    // DEL command: delete key
    bool del(const std::string& key) {
        std::lock_guard<std::mutex> lock(store_mutex);
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return false;
        }
        lru_list.erase(it->second);
        cache_map.erase(it);
        trie.remove(key);
        return true;
    }

    // KEYS command: search keys by prefix
    std::vector<std::string> keys(const std::string& prefix = "") {
        std::lock_guard<std::mutex> lock(store_mutex);
        return trie.get_keys_with_prefix(prefix);
    }
};

int main() {
    // Capacity of 5 for demonstration
    KeyValueStore store(5);

    std::cout << "=== High-Performance C++ In-Memory Key-Value Store ===" << std::endl;
    std::cout << "Commands: SET <key> <val> | GET <key> | DEL <key> | KEYS [prefix] | EXIT\n" << std::endl;

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line) || line == "EXIT") break;

        std::stringstream ss(line);
        std::string cmd, key, val;
        ss >> cmd;

        if (cmd == "SET") {
            if (ss >> key >> val) {
                store.set(key, val);
                std::cout << "OK" << std::endl;
            } else {
                std::cout << "Usage: SET <key> <val>" << std::endl;
            }
        } else if (cmd == "GET") {
            if (ss >> key) {
                if (store.get(key, val)) {
                    std::cout << "\"" << val << "\"" << std::endl;
                } else {
                    std::cout << "(nil)" << std::endl;
                }
            } else {
                std::cout << "Usage: GET <key>" << std::endl;
            }
        } else if (cmd == "DEL") {
            if (ss >> key) {
                if (store.del(key)) {
                    std::cout << "(integer) 1" << std::endl;
                } else {
                    std::cout << "(integer) 0" << std::endl;
                }
            } else {
                std::cout << "Usage: DEL <key>" << std::endl;
            }
        } else if (cmd == "KEYS") {
            std::string prefix = "";
            ss >> prefix;
            auto matching_keys = store.keys(prefix);
            if (matching_keys.empty()) {
                std::cout << "(empty list or set)" << std::endl;
            } else {
                for (size_t i = 0; i < matching_keys.size(); ++i) {
                    std::cout << i + 1 << ") \"" << matching_keys[i] << "\"" << std::endl;
                }
            }
        } else if (!cmd.empty()) {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }

    return 0;
}
