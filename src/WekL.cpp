#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void error(int errCode, const char* msg, unsigned line = 0)
{
	if (line >= 0)
		std::cout << "\u001b[31m---\nE" << errCode << " (line " << line << "): " << msg << "\n---\u001b[0m";
	else
		std::cout << "\u001b[31m---\nE" << errCode << ": " << msg << "\n---\u001b[0m";
	exit(errCode);
}

int main(int argc, char** argv)
{
	if (argc <= 1)
		error(1, "No file specified!");
	if (std::string(argv[1]).substr(std::string(argv[1]).length() - 5) != ".wekl")
		error(1, "File isn't WEKL program!");
	std::ifstream file(argv[1]);
	std::string line, tmp;
	int lineCounter = 1;
	std::vector<std::string> stack;
	bool special_char = false, in_string = false;
	int if_dec_state = 0, if_state = 0;
	std::string if_value1, if_value2;
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
				//Ignore comment
				if (in_string != true && tmp == "//")
				{
					tmp.clear();
					break;
				}

				//String operations
				if (in_string)
				{
					if (tmp.back() == '\"')
					{
						in_string = false;
						tmp.pop_back();
						if (if_dec_state == 1)
						{
							if_dec_state = 2;
							if_value1 = tmp;
						}
						else if (if_dec_state == 2)
						{
							if_dec_state = 3;
							if_value2 = tmp;
						}
						else
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
						case '\$':
							tmp.pop_back();
							tmp.append(stack.back());
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
					in_string = true;
					tmp.clear();
				}

				//Non-string operations
				else if (!in_string)
				{
					if (if_dec_state != 4)
					{
						if (tmp == "outs")
						{
							if (stack.size() <= 0)
							{
								error(2, "Tried to access stack, but it was empty!", lineCounter);
							}
							std::cout << stack.back();
							tmp.clear();
						}
						else if (tmp == "pop")
						{
							if (stack.size() <= 0)
							{
								error(2, "Tried to access stack, but it was empty!", lineCounter);
							}
							stack.pop_back();
							tmp.clear();
						}
						else if (tmp == "dup")
						{
							if (stack.size() <= 0)
							{
								error(2, "Tried to access stack, but it was empty!", lineCounter);
							}
							stack.push_back(stack.back());
							tmp.clear();
						}
						else if (tmp == "add")
						{
							if (stack.size() <= 1)
							{
								error(3, "Tried to access stack, but it had less than 2 values!", lineCounter);
								return 1;
							}
							stack.push_back(std::to_string(std::stoi(stack.at(0)) + std::stoi(stack.at(1))));
							tmp.clear();
						}
						else if (tmp == "sub")
						{
							if (stack.size() <= 1)
							{
								error(3, "Tried to access stack, but it had less than 2 values!", lineCounter);
								return 1;
							}
							stack.push_back(std::to_string(std::stoi(stack.at(0)) - std::stoi(stack.at(1))));
							tmp.clear();
						}
						else if (tmp == "mul")
						{
							if (stack.size() <= 1)
							{
								error(3, "Tried to access stack, but it had less than 2 values!", lineCounter);
								return 1;
							}
							stack.push_back(std::to_string(std::stoi(stack.at(0)) * std::stoi(stack.at(1))));
							tmp.clear();
						}
						else if (tmp == "div")
						{
							if (stack.size() <= 1)
							{
								error(3, "Tried to access stack, but it had less than 2 values!", lineCounter);
								return 1;
							}
							stack.push_back(std::to_string(std::stoi(stack.at(0)) / std::stoi(stack.at(1))));
							tmp.clear();
						}
						else if (tmp == "if")
						{
							if (if_dec_state == 0)
								if_dec_state = 1;
							tmp.clear();
						}
						else if (tmp == "=")
						{
							if (if_dec_state == 2)
							{
								if_state = 1;
							}
							tmp.clear();
						}
						else if (tmp == "!")
						{
							if (if_dec_state == 2)
							{
								if_state = 4;
							}
							tmp.clear();
						}
						else if (tmp == "<")
						{
							if (if_dec_state == 2)
							{
								if_state = 2;
							}
							tmp.clear();
						}
						else if (tmp == ">")
						{
							if (if_dec_state == 2)
							{
								if_state = 3;
							}
							tmp.clear();
						}
						else if (tmp == "{")
						{
							if ((if_state == 1 && if_value1 == if_value2) || (if_state == 2 && if_value1 < if_value2) || (if_state == 3 && if_value1 > if_value2) || (if_state == 4 && if_value1 != if_value2))
							{
								if_dec_state = 0;
								if_dec_state = 0;
							}
							else
							{
								if_dec_state = 4;
								if_state = 0;
							}
							tmp.clear();
						}
					}
					else if (if_dec_state == 4 && tmp.back() == '}')
					{
						if_dec_state = 0;
					}
				}
			}
			lineCounter++;
		}
		file.close();
	}
	else
		error(1, "Couldn't open file!");

	return 0;
}