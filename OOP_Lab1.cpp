#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <limits>
#include <cmath>
#include <memory>
#include <algorithm>
#include <stdexcept> // Для видачi помилки
#include <queue> // Для реалiзацiї heap
using namespace std;

// ПРОГРАМА 1
// iнтерфейс для черги з прiоритетом
class PriorityQueue {
public:
    virtual void add(int value) = 0;
    virtual int pop() = 0;
    virtual int peek() const = 0;
    virtual ~PriorityQueue() {}
};

// Вузол зв'язного списку
struct LinkedListNode {
    int value;
    LinkedListNode* next;
    LinkedListNode(int val) : value(val), next(nullptr) {}
};

// Реалiзацiя на основi зв'язного списку
class LinkedListQueue : public PriorityQueue {
private:
    LinkedListNode* head;
public:
    LinkedListQueue() : head(nullptr) {}
    void add(int value) override {
        LinkedListNode* newNode = new LinkedListNode(value);
        if (!head || head->value > value) {
            newNode->next = head;
            head = newNode;
        }
        else {
            LinkedListNode* curr = head;
            while (curr->next && curr->next->value <= value)
                curr = curr->next;
            newNode->next = curr->next;
            curr->next = newNode;
        }
    }
    int pop() override {
        if (!head) throw runtime_error("Queue is empty!");
        int value = head->value;
        LinkedListNode* temp = head;
        head = head->next;
        delete temp;
        return value;
    }
    int peek() const override {
        if (!head) throw runtime_error("Queue is empty!");
        return head->value;
    }
    ~LinkedListQueue() {
        while (head) {
            LinkedListNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Реалiзацiя на основi динамiчного масиву
class ArrayQueue : public PriorityQueue {
private:
    vector<int> data;
public:
    void add(int value) override {
        data.push_back(value);
        sort(data.begin(), data.end());
    }
    int pop() override {
        if (data.empty()) throw runtime_error("Queue is empty!");
        int value = data.front();
        data.erase(data.begin());
        return value;
    }
    int peek() const override {
        if (data.empty()) throw runtime_error("Queue is empty!");
        return data.front();
    }
};

// Вузол AVL-дерева
struct AVLTreeNode {
    int value;
    int height;
    AVLTreeNode* left;
    AVLTreeNode* right;
    AVLTreeNode(int val) : value(val), height(1), left(nullptr), right(nullptr) {}
};

// Реалiзацiя на основi AVL-дерева
class AVLTreeQueue : public PriorityQueue {
private:
    AVLTreeNode* root;

    int getHeight(AVLTreeNode* node) {
        return node ? node->height : 0;
    }
    int getBalance(AVLTreeNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    AVLTreeNode* rotateRight(AVLTreeNode* y) {
        AVLTreeNode* x = y->left;
        AVLTreeNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }
    AVLTreeNode* rotateLeft(AVLTreeNode* x) {
        AVLTreeNode* y = x->right;
        AVLTreeNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }
    AVLTreeNode* insert(AVLTreeNode* node, int value) {
        if (!node) return new AVLTreeNode(value);
        if (value < node->value)
            node->left = insert(node->left, value);
        else if (value > node->value)
            node->right = insert(node->right, value);
        else
            return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);
        if (balance > 1 && value < node->left->value)
            return rotateRight(node);
        if (balance < -1 && value > node->right->value)
            return rotateLeft(node);
        if (balance > 1 && value > node->left->value) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (balance < -1 && value < node->right->value) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }

    AVLTreeNode* findMin(AVLTreeNode* node) {
        AVLTreeNode* current = node;
        while (current && current->left) current = current->left;
        return current;
    }

    AVLTreeNode* deleteNode(AVLTreeNode* root, int key) {
        if (!root) return root;
        if (key < root->value)
            root->left = deleteNode(root->left, key);
        else if (key > root->value)
            root->right = deleteNode(root->right, key);
        else {
            if (!root->left || !root->right) {
                AVLTreeNode* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                }
                else
                    *root = *temp;
                delete temp;
            }
            else {
                AVLTreeNode* temp = findMin(root->right);
                root->value = temp->value;
                root->right = deleteNode(root->right, temp->value);
            }
        }
        if (!root) return root;

        root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
        int balance = getBalance(root);
        if (balance > 1 && getBalance(root->left) >= 0)
            return rotateRight(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0)
            return rotateLeft(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        return root;
    }

    int findMinValue(AVLTreeNode* node) const {
        AVLTreeNode* current = node;
        while (current->left) current = current->left;
        return current->value;
    }

public:
    AVLTreeQueue() : root(nullptr) {}

    void add(int value) override {
        root = insert(root, value);
    }
    int pop() override {
        if (!root) throw runtime_error("Queue is empty!");
        int minValue = findMinValue(root);
        root = deleteNode(root, minValue);
        return minValue;
    }
    int peek() const override {
        if (!root) throw runtime_error("Queue is empty!");
        return findMinValue(root);
    }
};

// Реалiзацiя на основi Heap
class HeapQueue : public PriorityQueue {
private:
    priority_queue<int, vector<int>, greater<int>> minHeap;
public:
    void add(int value) override {
        minHeap.push(value);
    }
    int pop() override {
        if (minHeap.empty()) throw runtime_error("Queue is empty!");
        int value = minHeap.top();
        minHeap.pop();
        return value;
    }
    int peek() const override {
        if (minHeap.empty()) throw runtime_error("Queue is empty!");
        return minHeap.top();
    }
};

// Тестування програми 1
void testTask1() {
    // Полiморфiзм
    unique_ptr<PriorityQueue> queue1 = make_unique<LinkedListQueue>();
    unique_ptr<PriorityQueue> queue2 = make_unique<ArrayQueue>();
    unique_ptr<PriorityQueue> queue3 = make_unique<AVLTreeQueue>();
    unique_ptr<PriorityQueue> queue4 = make_unique<HeapQueue>();

    cout << "Тестування LinkedListQueue:" << endl;
    cout << "Вхiднi данi: 5 3 7" << endl;
    queue1->add(5);
    queue1->add(3);
    queue1->add(7);
    cout << "Peek: " << queue1->peek() << endl;
    cout << "Pop: " << queue1->pop() << endl << endl;

    cout << "Тестування ArrayQueue:" << endl;
    cout << "Вхiднi данi: 5 3 7" << endl;
    queue2->add(5);
    queue2->add(3);
    queue2->add(7);
    cout << "Peek: " << queue2->peek() << endl;
    cout << "Pop: " << queue2->pop() << endl << endl;

    cout << "Тестування AVLTreeQueue:" << endl;
    cout << "Вхiднi данi: 15 10 20 5" << endl;
    queue3->add(15);
    queue3->add(10);
    queue3->add(20);
    queue3->add(5);
    cout << "Peek: " << queue3->peek() << endl;
    cout << "Pop: " << queue3->pop() << endl;
    cout << "Peek after pop: " << queue3->peek() << endl << endl;

    cout << "Тестування HeapQueue:" << endl;
    cout << "Вхiднi данi: 15 10 20 5" << endl;
    queue4->add(15);
    queue4->add(10);
    queue4->add(20);
    queue4->add(5);
    cout << "Peek: " << queue4->peek() << endl;
    cout << "Pop: " << queue4->pop() << endl;
    cout << "Peek after pop: " << queue4->peek() << endl << endl;
}

// ПРОГРАМА 2
// Середовище для моделювання руху
class Environment {
private:
    static map<string, vector<pair<string, double>>> neighbors;

public:
    static void setDistance(const string& pointA, const string& pointB, double distance) {
        neighbors[pointA].emplace_back(pointB, distance);
        neighbors[pointB].emplace_back(pointA, distance);
    }

    static vector<pair<string, double>> getNeighbors(const string& point) {
        return neighbors[point];
    }

    static double getDistance(const string& pointA, const string& pointB) {
        auto it = neighbors.find(pointA);
        if (it != neighbors.end()) {
            for (const auto& neighbor : it->second) {
                if (neighbor.first == pointB) {
                    return neighbor.second;
                }
            }
        }
        return numeric_limits<double>::infinity(); // Якщо точки не пов'язанi
    }
};

map<string, vector<pair<string, double>>> Environment::neighbors;

// Абстрактний клас транспортного засобу
class Transport {
protected:
    string name;
    double speed; // швидкiсть у км/год
public:
    Transport(const string& name, double speed) : name(name), speed(speed) {}
    virtual ~Transport() {}

    virtual double move(const string& start, const string& end) = 0;
    string getName() const { return name; }
};

// Наземний транспорт
class LandTransport : public Transport {
public:
    LandTransport(const string& name, double speed) : Transport(name, speed) {}

    double move(const string& start, const string& end) override {
        double distance = Environment::getDistance(start, end);
        cout << "Наземний транспорт " << name << "[швидкiсть " << speed << " км/год] рухається з пункту " << start << " до пункту " << end
            << ". Вiдстань: " << distance << " км." << endl;
        return distance / speed;
    }
};

class Car : public LandTransport {
    double fuelConsumption; // витрата пального, л/100 км
public:
    Car(const string& name, double speed, double fuelConsumption)
        : LandTransport(name, speed), fuelConsumption(fuelConsumption) {}

    double getFuelConsumption() const { return fuelConsumption; }
};

class Bicycle : public LandTransport {
public:
    Bicycle(const string& name, double speed) : LandTransport(name, speed) {}
};

// Водний транспорт
class WaterTransport : public Transport {
public:
    WaterTransport(const string& name, double speed) : Transport(name, speed) {}

    double move(const string& start, const string& end) override {
        double distance = Environment::getDistance(start, end);
        cout << "Водний транспорт " << name << "[швидкiсть " << speed << " км/год] рухається з пункту " << start << " до пункту " << end
            << ". Вiдстань: " << distance << " км." << endl;
        return distance / speed;
    }
};

class Boat : public WaterTransport {
    int capacity; // кiлькiсть пасажирiв
public:
    Boat(const string& name, double speed, int capacity)
        : WaterTransport(name, speed), capacity(capacity) {}

    int getCapacity() const { return capacity; }
};

// Повiтряний транспорт
class AirTransport : public Transport {
public:
    AirTransport(const string& name, double speed) : Transport(name, speed) {}

    double move(const string& start, const string& end) override {
        double distance = Environment::getDistance(start, end);
        cout << "Повiтряний транспорт " << name << "[швидкiсть "<< speed<<" км/год] рухається з пункту " << start << " до пункту " << end
            << ". Вiдстань: " << distance << " км." << endl;
        return distance / speed;
    }
};

class Plane : public AirTransport {
    int altitude; // висота польоту у метрах
public:
    Plane(const string& name, double speed, int altitude)
        : AirTransport(name, speed), altitude(altitude) {}

    int getAltitude() const { return altitude; }
};

// Алгоритм обчислення маршруту з використанням алгоритму Дейкстри
class RoutePlanner{
public:
    // Пошук оптимального маршруту мiж пунктами
    static double findOptimalRoute(shared_ptr<Transport> transport, const string & start, const string & end) {
        unordered_map<string, double> distances; // Вiдстань до кожної точки
        unordered_map<string, string> previous; // Для вiдновлення шляху
        set<string> visited; // Вiдвiданi точки
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<>> pq;

        // iнiцiалiзацiя вiдстаней
        distances[start] = 0;
        pq.emplace(0, start);

        while (!pq.empty()) {
            auto [currentDistance, currentPoint] = pq.top();
            pq.pop();

            if (visited.count(currentPoint)) continue;
            visited.insert(currentPoint);

            if (currentPoint == end) break;

            for (const auto& neighbor : Environment::getNeighbors(currentPoint)) {
                string nextPoint = neighbor.first;
                double edgeDistance = neighbor.second;

                double newDistance = currentDistance + edgeDistance;

                if (distances.find(nextPoint) == distances.end() || newDistance < distances[nextPoint]) {
                    distances[nextPoint] = newDistance;
                    previous[nextPoint] = currentPoint;
                    pq.emplace(newDistance, nextPoint);
                }
            }
        }

        // Виведення маршруту
        if (distances.find(end) == distances.end()) {
            cout << "Неможливо знайти маршрут мiж " << start << " i " << end << endl;
            return numeric_limits<double>::infinity();
        }

        // Вiдновлення маршруту
        vector<string> path;
        for (string at = end; at != ""; at = previous[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());

        cout << "Оптимальний маршрут: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i != path.size() - 1) cout << " -> ";
        }
        cout << endl;

        // Обчислення часу подорожi
        double totalDistance = distances[end];
        double travelTime = totalDistance / transport->move(path[0], path[1]); // Використовуємо швидкiсть транспорту

        return totalDistance / travelTime;
    }
};

// Тестування програми 2
void testTask2() {
    Environment::setDistance("A", "B", 100);
    Environment::setDistance("B", "C", 150);
    Environment::setDistance("A", "C", 250);

    cout << "Вхiднi данi:" << endl;
    cout << "A -> B " << Environment::getDistance("A", "B") << " km" << endl;
    cout << "B -> C " << Environment::getDistance("B", "C") << " km" << endl;
    cout << "A -> C " << Environment::getDistance("A", "C") << " km" << endl;

    // Транспортнi засоби (використання полiморфiзму)
    shared_ptr<Transport> car = make_shared<Car>("Седан", 60, 8);
    shared_ptr<Transport> bike = make_shared<Bicycle>("Гiрський велосипед", 20);
    shared_ptr<Transport> boat = make_shared<Boat>("Яхта", 30, 10);
    shared_ptr<Transport> plane = make_shared<Plane>("Боїнг 747", 900, 10000);

    // Рух мiж пунктами
    cout << "Час подорожi автомобiлем: " << RoutePlanner::findOptimalRoute(car, "A", "B") << " годин" << endl;
    cout << "Час подорожi велосипедом: " << RoutePlanner::findOptimalRoute(bike, "A", "B") << " годин" << endl;
    cout << "Час подорожi яхтою: " << RoutePlanner::findOptimalRoute(boat, "A", "B") << " годин" << endl;
    cout << "Час подорожi лiтаком: " << RoutePlanner::findOptimalRoute(plane, "A", "B") << " годин" << endl;
}


int main() {
    setlocale(LC_ALL, "ukr");
    cout << "ПРОГРАМА 1" << endl;
    testTask1();

    cout << "ПРОГРАМА 2" << endl;
    testTask2();
    return 0;
}
