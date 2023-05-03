#include <iostream>
#include <functional>

void TrimIf(std::string& str, std::function<bool(char)> func)
{
	int i = 0;
	for (auto it = str.begin(); it != str.end(); it++)
	{
		if (!func(*it)) break;
		i++;
	}

	str.erase(str.begin(), str.begin() + i);

	i = 0;
	for (auto it = str.rbegin(); it != str.rend(); it++)
	{
		if (!func(*it)) break;
		i++;
	}

	str.erase(str.end() - i, str.end());

}


bool isWhitespace(char c)
{
	return (c == 'x') ? true : false;
}

int main()
{
	std::string x = "xxdasda d da xx";

	TrimIf(x, [](char c) {return c == 'x' ? true : false; });

	std::cout << x;
}

