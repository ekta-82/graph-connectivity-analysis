#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <iostream>
#include <string>

using namespace std;

// 🔹 BFS Connectivity Check (Includes Diagonals)
bool isConnected(int rows, int cols, const vector<bool> &active)
{
    int n = rows * cols;
    vector<bool> visited(n, false);
    int start = -1;
    for (int i = 0; i < n; i++)
    {
        if (active[i])
        {
            start = i;
            break;
        }
    }
    if (start == -1)
        return true;

    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty())
    {
        int curr = q.front();
        q.pop();
        int r = curr / cols;
        int c = curr % cols;
        vector<pair<int, int>> dirs = {
            {r, c + 1}, {r, c - 1}, {r + 1, c}, {r - 1, c}, {r - 1, c - 1}, {r - 1, c + 1}, {r + 1, c - 1}, {r + 1, c + 1}};
        for (auto [nr, nc] : dirs)
        {
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
            {
                int next = nr * cols + nc;
                if (active[next] && !visited[next])
                {
                    visited[next] = true;
                    q.push(next);
                }
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        if (active[i] && !visited[i])
            return false;
    }
    return true;
}

// 🔹 Degree function (Counts active neighbors)
int getActiveDegree(int index, int rows, int cols, const vector<bool> &active)
{
    int r = index / cols;
    int c = index % cols;
    int count = 0;
    vector<pair<int, int>> dirs = {
        {r, c + 1}, {r, c - 1}, {r + 1, c}, {r - 1, c}, {r - 1, c - 1}, {r - 1, c + 1}, {r + 1, c - 1}, {r + 1, c + 1}};
    for (auto [nr, nc] : dirs)
    {
        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
        {
            if (active[nr * cols + nc])
                count++;
        }
    }
    return count;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({1400, 900}), "Real-Time Mesh Connectivity");

    int lastNode = -1;
    int lastEdges = 0;
    bool wasRemoved = false; // Track if the last action was a removal or addition

    const int rows = 5;
    const int cols = 7;
    const float spacing = 150.f;
    const float radius = 20.f;

    vector<sf::CircleShape> nodes;
    vector<sf::Vector2f> centers;
    vector<bool> active(rows * cols, true);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            sf::CircleShape node(radius);
            node.setFillColor(sf::Color::Green);
            float x = 200.f + j * spacing;
            float y = 150.f + i * spacing;
            node.setPosition({x, y});
            nodes.push_back(node);
            centers.push_back({x + radius, y + radius});
        }
    }

    sf::Font font;
    if (!font.openFromFile("DejaVuSans.ttf"))
        cout << "Font load failed!" << endl;

    sf::Text infoText(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::Yellow);
    infoText.setPosition({10.f, 10.f});

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseEvent->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f mpos = window.mapPixelToCoords({mouseEvent->position.x, mouseEvent->position.y});

                    for (int i = 0; i < (int)nodes.size(); i++)
                    {
                        float dx = mpos.x - centers[i].x;
                        float dy = mpos.y - centers[i].y;

                        if (dx * dx + dy * dy <= (radius + 5) * (radius + 5))
                        {
                            lastNode = i;
                            // Update edges based on current state before switching
                            lastEdges = getActiveDegree(i, rows, cols, active);

                            if (active[i])
                            {
                                active[i] = false;
                                nodes[i].setFillColor(sf::Color::Red);
                                wasRemoved = true;
                            }
                            else
                            {
                                active[i] = true;
                                nodes[i].setFillColor(sf::Color::Green);
                                wasRemoved = false;
                            }
                        }
                    }
                }
            }
        }

        window.clear();

        // Draw Edges
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                int index = i * cols + j;
                if (!active[index])
                    continue;
                vector<pair<int, int>> neighbors = {{i, j + 1}, {i + 1, j}, {i + 1, j + 1}, {i + 1, j - 1}};
                for (auto [nr, nc] : neighbors)
                {
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols)
                    {
                        int nIdx = nr * cols + nc;
                        if (active[nIdx])
                        {
                            sf::Vertex line[] = {sf::Vertex{centers[index]}, sf::Vertex{centers[nIdx]}};
                            window.draw(line, 2, sf::PrimitiveType::Lines);
                        }
                    }
                }
            }
        }

        for (auto &node : nodes)
            window.draw(node);

        // 🔹 Real-Time Text Update
        if (lastNode != -1)
        {
            string status = wasRemoved ? "Removed" : "Restored";
            infoText.setString("Node: " + to_string(lastNode) + " | " + status + " Edges: " + to_string(lastEdges));
        }
        else
        {
            infoText.setString("Mesh Connectivity: Click a node to toggle");
        }

        window.draw(infoText);
        window.display();
    }
    return 0;
}