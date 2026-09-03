# 🚀 C++ In-Memory Key-Value Store

A lightweight, super-fast key-value database built entirely from scratch in C++. Think of it like a mini version of Redis. It stores data in your computer's memory (RAM) for instant access and uses smart data structures to make sure it never uses up too much space.

## ✨ Features

* **Smart Memory Management (LRU Cache):** You can set a maximum capacity for the database. When it gets full, it automatically deletes the "Least Recently Used" (LRU) item to make room for new data. 
* **Lightning Fast Prefix Search:** It uses a special data structure called a **Trie** (Prefix Tree). This means if you search for all keys starting with `user:`, it finds them instantly without having to scan the entire database.
* **Thread-Safe:** Built with mutex locks, meaning multiple parts of a program (or multiple users) can read and write to the database at the exact same time without crashing or corrupting the data.
* **Instant Lookups:** Uses hash maps and doubly linked lists under the hood to make sure reading and writing data happens in $O(1)$ time (which means it takes the exact same amount of time whether you have 10 items or 10,000 items).

## 🛠️ Supported Commands

When you run the program, you get an interactive terminal where you can type these commands:

* `SET <key> <value>` - Saves a new key-value pair (or updates an existing one).
* `GET <key>` - Fetches the value for that key.
* `DEL <key>` - Deletes the key from the database.
* `KEYS <prefix>` - Finds all keys that start with a specific word or letter.
* `EXIT` - Closes the database.

## 💻 How to Run It

You don't need any special libraries to run this, just a standard C++ compiler.

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/dheerajisop/cpp-kv-store.git](https://github.com/dheerajisop/cpp-kv-store.git)
   cd cpp-kv-store
   g++ -std=c++17 -pthread main.cpp -o kvstore
   ./kvstore
🎮 Example Usage
> SET user:1 Dheeraj
OK
> SET user:2 Rahul
OK
> GET user:1
"Dheeraj"
> KEYS user:
1) "user:1"
2) "user:2"
> DEL user:1
(integer) 1
