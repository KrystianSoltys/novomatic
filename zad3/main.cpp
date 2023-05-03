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
	return (c == ' ') ? true : false;
}

class IsDigit
{
public:
    bool operator()(char c) {return (c>='0' && c<='9') ? true : false;}
};

int main()
{
	std::string a = "xxxxAla ma kotaxxxxxxxx";
    TrimIf(a, [](char c) {return c == 'x' ? true : false; });

    std::string b = "   przykładowy string  ";
    TrimIf(b, &isWhitespace);

    IsDigit isd;
    std::string c = "3Abc12";
    TrimIf(c, isd);

    std::cout << "Test #1: " << ((a == "Ala ma kota") ? "passed" : "failed") << std::endl;
    std::cout << "Test #2: " << ((b == "przykładowy string") ? "passed" : "failed") << std::endl;
    std::cout << "Test #3: " << ((c == "Abc") ? "passed" : "failed") << std::endl;

}
