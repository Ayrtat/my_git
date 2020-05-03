#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>

int uniquePaths(int m, int n) {
    if (m == 0 && n == 0)
        return 0;

    if (m == 1 && n == 1)
        return 1;

    int moveRight = 1;
    int moveDown = 1;

    std::vector < std::vector<int> > routes(m);
    for (auto i = 0; i < routes.size(); i++)
        routes[i].resize(n);

    routes[0][0] = 0;

    for (auto i = 0; i < m; i++)
    {
        for (auto j = 0; j < n; j++)
        {
            if (i == 0 && j == 0)
                continue;

            if (i == 0 && j > 0)
            {
                routes[i][j] =  moveRight;
                continue;
            }

            if (i > 0 && j == 0)
            {
                routes[i][j] =  moveDown;
                continue;
            }

            if (j > 0 && i > 0)
            {
                routes[i][j] = routes[i][j - 1] + routes[i - 1][j];
                continue;
            }
        }
    }

    if (m == 1)
        return routes[0][n - 1];

    if (n == 1)
        return routes[m - 1][0];

    return routes[m - 2][n - 1] + routes[m - 1][n - 2];
}

int main()
{
    std::cout << uniquePaths(7, 3);
    return 0;
}
