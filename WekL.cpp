#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using std::cout;
using std::string;

int main(int argc, char** argv)
{
	if (string(argv[1]).substr(string(argv[1]).length() - 5) != ".wekl")
	{
		cout << "File isn't WEKL program!";
		return 1;
	}
	std::ifstream file(argv[1]);
	string line, tmp;
	std::vector<string> stack;
	bool special_char = false, in_string = false, in_if = false, in_loop = false;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			for (unsigned i = 0; i < line.size(); i++)
			{
				tmp.push_back(line.at(i));

				//Ignore whitespace
				if (in_string != true && (tmp.back() == ' ' || tmp.back() == '\t' || tmp.back() == '\n'))
				{
					tmp.pop_back();
					continue;
				}

				//String operations
				if (in_string)
				{
					if (tmp.back() == '\"')
					{
						in_string = false;
						tmp.pop_back();
						stack.push_back(tmp);
						tmp.clear();
					}
					else if (special_char)
					{
						switch (tmp.back())
						{
						case 'n':
							tmp.pop_back();
							tmp.push_back('\n');
							break;
						case 't':
							tmp.pop_back();
							tmp.push_back('\t');
							break;
						case '\\':
							tmp.pop_back();
							tmp.push_back('\\');
							break;
						default:
							break;
						}
						special_char = false;
					}
					else if (tmp.back() == '\\')
					{
						special_char = true;
						tmp.pop_back();
					}
				}
				else if (tmp.back() == '\"' && in_string == false)
				{
					tmp.clear();
					in_string = true;
				}

				//Standard functions
				if (!in_string)
				{
					if (tmp == "out")
					{
						cout << stack.at(stack.size() - 1);
						tmp.clear();
					}
					else if (tmp == "pop")
					{
						stack.pop_back();
						tmp.clear();
					}
					if (tmp == "if")
					{
						in_if = true;
					}
					else if (tmp == "fi")
					{
						in_if = false;
					}
					if (tmp == "loop")
					{
						in_loop = true;
					}
					else if (tmp == "pool")
					{
						in_loop = false;
					}
				}
			}
		}
		file.close();
	}
	else
	{
		cout << "Couldn't open file!";
		return 1;
	}
}