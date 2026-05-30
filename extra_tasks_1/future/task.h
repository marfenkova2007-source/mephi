#pragma once
#include "ThreadPool.h"

#include <algorithm>
#include <vector>
#include <deque>
#include <cstdint>
#include <atomic>
#include <memory>
#include <future>
// нельзя инклюдить set и queue

/*
    Задание -  реализовать многопоточный алгоритм Дейкстры (подробнее: https://neerc.ifmo.ru/wiki/index.php?title=Алгоритм_Дейкстры).
    Вам надо написать также ThreadPool, который необходимо использовать для распараллеливания вычислений.
    Для этого требуется реализовать функци findMinVertex и relaxEdges.

    `findMinVertex` - Находит вершину с минимальным расстоянием, которая ещё не была посещена и возвращает ее.

    `relaxEdges` - Производит релаксацию ребер из полученной в `findMin` вершины по всем исходящим вершинам

    Обе функции должны работать с графом в ThreadPool.

    Чтобы проверить, исполнена задача или нет
    if(future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    получение и обработка результата
    }

    Гарантируется что не случится переполнение int.
*/

// Важно реализовать алгоритм с асимптотикой O(n^2)

class Dijkstra
{
public:
    Dijkstra(int numVertices, int numEdges, int numThreads) : threadPool(numThreads), numVertices_(numVertices), numThreads_(numThreads) {
        d.resize(numVertices+1);
        used.resize(numVertices+1);
        parent.resize(numVertices+1);
        graf.resize(numVertices+1);
    };

    // Добавляет ребра между двумя вершинами с указанным весом в неориентированном графе.
    void addEdge(int from, int to, int weight){
        graf[from].emplace_back(to, weight);
        graf[to].emplace_back(from, weight);
    };

    // Выполняет сам алгоритм Дейкстры многопоточно.
    void run(int start, int finish)
    {
        const int INF = 1e9;
        for (int v = 1; v<=numVertices_; ++v) {
            d[v] = INF;
            used[v] = false;
            parent[v] = -1;
        }
        d[start] = 0;

        for (int i = 1; i<=numVertices_; ++i) {

            int currentVertex = findMinVertex();
            if (currentVertex == -1 || d[currentVertex] == INF) {
                break;
            }
            if (currentVertex == finish) {
                break;
            }
            
            used[currentVertex] = true;
            
            relaxEdges(currentVertex);
        }
    }

    // Получает кратчайший путь  в виде последовательности вершин и возвращает его. (вершины выводятся по возврастанию)
    // Если не существует пути, требуется вернуть пустую deque
    std::deque<int> getPath(int start, int finish){
        std::deque<int> kr_pyt; 
        if (d[finish] == 1e9) {
            return kr_pyt; 
        }
        for (int v = finish; v != -1; v = parent[v]) {
            kr_pyt.push_front(v);
        }
        
        return kr_pyt;
    };

private:
    ThreadPool threadPool;

    // Находит вершину с минимальным расстоянием, которая ещё не была посещена.
    int findMinVertex();

    // Производит релаксацию ребер из vertex
    void relaxEdges(int vertex);
    int numVertices_;
    int numThreads_;
    std::vector<int> d;     
    std::vector<bool> used;  
    std::vector<int> parent;
    std::vector<std::vector<std::pair<int, int>>> graf;
    std::mutex d_mutex;
};

int Dijkstra::findMinVertex() {
    int k = numVertices_/numThreads_;
    if (k == 0) {
        k = 1; 
    }
    std::vector<std::future<std::pair<int, int>>> futures;
    
    int start = 1;
    for (int t = 0; t < numThreads_; ++t) {
        int end = start + k;
        if (t == numThreads_ - 1) {
            end = numVertices_ + 1;
        }
        if (start > numVertices_) break;
        
        auto promise = std::make_shared<std::promise<std::pair<int, int>>>();
        futures.push_back(promise->get_future());
        
        threadPool.PushTask([this, start, end, promise]() {
            int local_v = -1;
            int local_dist = 1e9;
            for (int i=start; i<end; ++i) {
                if (!used[i] and d[i]<local_dist) {
                    local_dist = d[i];
                    local_v = i;
                }
            }
            promise->set_value({local_v, local_dist});
        });
        
        start = end;
    }
    
    int best_v = -1;
    int best_dist = 1e9;
    for (auto& fut : futures) {
        auto [v, dist] = fut.get();
        if (v != -1 && dist < best_dist) {
            best_dist = dist;
            best_v = v;
        }
    }
    return best_v;
};

void Dijkstra::relaxEdges(int v) {
    int k = graf[v].size() / numThreads_;
    int ost= graf[v].size() % numThreads_;
    
    std::vector<std::future<void>> futures;
    
    int start = 0;
    for (int t = 0; t < numThreads_; ++t) {
        int end = start + k;
        if (t == numThreads_ - 1) {
            end += ost;
        }
        if (start >= (int)graf[v].size()) break;
        if (end == start) continue;
        
        auto promise = std::make_shared<std::promise<void>>();
        futures.push_back(promise->get_future());
        
        threadPool.PushTask([this, v, start, end, promise]() {
            for (int i=start; i<end; ++i) {
                const auto& edge = graf[v][i];
                int to = edge.first;
                int weight = edge.second;
                
                std::lock_guard<std::mutex> lock(d_mutex);
                int new_dist = d[v] + weight;
                if (new_dist < d[to]) {
                    d[to] = new_dist;
                    parent[to] = v;
                }
            }
            promise->set_value();
        });
        
        start = end;
    }
    
    for (auto& fut : futures) {
        fut.wait();
    }
};