//https://codeforces.com/problemset/problem/1334/C

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>

int main() {
    int input_health[] = { 7, 2, 5 };
    int input_damage[] = { 15, 14, 3 };

    std::vector<int> monster_health(input_health, input_health+3);
    std::vector<int> monster_damage(input_damage, input_damage+3);

    std::vector<int> bullets(monster_health.size());

    auto N = monster_health.size();

    for (int i = 0; i < N; i++)
    {
        int damageFromNeighbor;

        if (i > 1)
        {
            damageFromNeighbor = monster_damage[i - 1];
        }
        else
        {
            damageFromNeighbor = monster_damage[N - 1];
        }

        bullets[i] = monster_health[i] - damageFromNeighbor;
    }

    int sum = 0;
    for (auto quantity : bullets)
    {
        quantity > 0 ? sum += quantity : sum++;
    }

    return 0;
}
