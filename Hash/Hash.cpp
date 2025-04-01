#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream> 
#include <windows.h>
#include <conio.h>

typedef unsigned int uint;
typedef uint8_t byte;

using namespace std;

string _alphabet = "0123456789ABCDEF";

class Utils
{
	public: static void Split(string str, char separator, vector<string>& v)
	{
		size_t start;
		size_t end = 0;

		while ((start = str.find_first_not_of(separator, end)) != string::npos)
		{
			end = str.find(separator, start);
			v.push_back(str.substr(start, end - start));
		}
	}

	public: static string ToBinary(uint decimal, size_t minLength = 0)
	{
		return ToBase(decimal, 2, minLength);
	}

	public: static string ToHex(uint decimal, size_t minLength = 0)
	{
		return ToBase(decimal, 16, minLength);
	}

	public: static string ToBase(uint decimal, int base, size_t minLength = 0)
	{
		string result = "";
		while (decimal != 0)
		{
			result = _alphabet[decimal % base] + result;
			decimal /= base;
		}

		int numberOfZeros = minLength - result.size();
		for (int i = 0; i < numberOfZeros; i++)
			result = "0" + result;
		return result;
	}
};

const int _maxIterationsCount = 80;

const int _maxBlockLength = 1024;

class SHA2
{
	// Массив  из первых 32 цифр дробной части кубических корней из простых чисел (2-311)
	private: static constexpr uint _partsOfCubicRoots[64] =
	{
	   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	protected: uint _hashValues[8];

	protected: int _blockLength = 512;

	protected: int _iterationsCount = 64;

	protected: int _wordLength = 32;

	protected: virtual void Reset()
	{
	}

	public: virtual const int GetDigestLength()
	{
		return 256;
	}

	private: void ProcessBlock(byte* block)
	{
		int wordsCount = _blockLength / _wordLength;
		uint words[_maxIterationsCount]{};

		for (int i = 0; i < wordsCount; i++)
		{
			words[i] = (uint)block[i * 4] << 24 | (uint)block[i * 4 + 1] << 16 | (uint)block[i * 4 + 2] << 8 | (uint)block[i * 4 + 3];
		}

		for (int i = wordsCount; i < _iterationsCount; i++)
		{
			words[i] = words[i - wordsCount] + SmallSigma0(words[i - wordsCount + 1]) + words[i - 7] + SmallSigma1(words[i - 2]);
		}

		// значения A, B, C, D, E, F, G, H
		uint tempValues[8]{};
		for (int i = 0; i < 8; i++)
			tempValues[i] = _hashValues[i];

		for (int i = 0; i < _iterationsCount; i++)
		{
			int temp2 = BigSigma0(tempValues[0]) + Majority(tempValues[0], tempValues[1], tempValues[2]);
			int temp1 = tempValues[7] + BigSigma1(tempValues[4]) + Choose(tempValues[4], tempValues[5], tempValues[6]) + _partsOfCubicRoots[i] + words[i];

			for (int j = 7; j > 0; j--)
			{
				tempValues[j] = tempValues[j - 1];
				if (j == 4)
					tempValues[j] += temp1;
			}
			tempValues[0] = temp1 + temp2;
		}

		for (int i = 0; i < 8; i++)
			_hashValues[i] += tempValues[i];
	}

	private: void FragmentMessageOnBlocks(byte* message, size_t size, byte** buffer)
	{
		size_t bitLength = size * 8;
		size_t blocksForMessageCount = bitLength / _blockLength + 1;
		int bytesInBlockCount = _blockLength / 8;
		size_t blocksCount = (bitLength + 64) / _blockLength + 1;
		size_t remainingSize = size;

		for (int i = 0; i < blocksForMessageCount; i++)
		{
			byte* block = buffer[i];
			int end = remainingSize > bytesInBlockCount ? bytesInBlockCount : remainingSize;
			for (int j = 0; j < end; j++)
				block[j] = message[i * bytesInBlockCount + j];
			if (remainingSize < bytesInBlockCount)
				remainingSize = 0;
			else
				remainingSize -= bytesInBlockCount;
		}

		buffer[blocksForMessageCount - 1][size % bytesInBlockCount] = 0x80;

		for (int j = 0; j < 8; j++)
			buffer[blocksCount - 1][63 - j] = bitLength >> 8 * j;
	}

	public: uint* Hash(byte* message, size_t size)
	{
		if (_iterationsCount > _maxIterationsCount)
		{
			cerr << "Invalid iteration number! Max value is: " << _maxIterationsCount << endl;
			return NULL;
		}
		if (_blockLength > _maxBlockLength)
		{
			cerr << "Invalid block length! Max value is: " << _maxBlockLength << endl;
			return NULL;
		}

		size_t blocksCount = (size * 8 + 64) / _blockLength + 1;

		byte** blocks = (byte**)calloc(blocksCount, sizeof(byte*));
		for (int i = 0; i < blocksCount; i++)
			blocks[i] = (byte*)calloc(_blockLength / sizeof(byte), sizeof(byte));

		FragmentMessageOnBlocks(message, size, blocks);

		for (int i = 0; i < blocksCount; i++)
			ProcessBlock(blocks[i]);

		for (int i = 0; i < blocksCount; i++)
			free(blocks[i]);
		free(blocks);
		return _hashValues;
	}

	// Логический сдвиг вправо
	protected: uint ShiftRight(uint value, int bits)
	{
		return value >> bits;
	}

	// Циклический сдвиг вправо
	protected: uint RotateRight(uint value, int bits)
	{
		return (value >> bits) | (value << (32 - bits));
	}

	protected: uint Choose(uint x, uint y, uint z)
	{
		return (x & y) ^ (~x & z);
	}

	protected: uint Majority(uint x, uint y, uint z)
	{
		return (x & (y | z)) | (y & z);
	}

	protected: uint BigSigma0(uint x)
	{
		return RotateRight(x, 2) ^ RotateRight(x, 13) ^ RotateRight(x, 22);
	}

	protected: uint BigSigma1(uint x)
	{
		return RotateRight(x, 6) ^ RotateRight(x, 11) ^ RotateRight(x, 25);
	}

	protected: uint SmallSigma0(uint x)
	{
		return RotateRight(x, 7) ^ RotateRight(x, 18) ^ ShiftRight(x, 3);
	}

	protected: uint SmallSigma1(uint x)
	{
		return RotateRight(x, 17) ^ RotateRight(x, 19) ^ ShiftRight(x, 10);
	}

	private: void PrintBlock(byte* block, int num)
	{
		cout << "Block " << num + 1 << endl;

		for (int j = 0; j < 61; j += 4)
		{
			cout << Utils::ToBinary(block[j], 8) << " " << Utils::ToBinary(block[j + 1], 8) << " " << Utils::ToBinary(block[j + 2], 8) << " " << Utils::ToBinary(block[j + 3], 8) << endl;
		}
		cout << endl;
	}
};

class SHA256 : public SHA2
{
	public: SHA256()
	{
		Reset();
	}

	public: void Reset()
	{
		// Наполняем массив первыми 32 цифрами дробной части квадратных корней из простых чисел (2-19)
		_hashValues[0] = 0x6A09E667;
		_hashValues[1] = 0xBB67AE85;
		_hashValues[2] = 0x3C6EF372;
		_hashValues[3] = 0xA54FF53A;
		_hashValues[4] = 0x510E527F;
		_hashValues[5] = 0x9B05688C;
		_hashValues[6] = 0x1F83D9AB;
		_hashValues[7] = 0x5BE0CD19;

		_blockLength = 512;
		_iterationsCount = 64;
		_wordLength = 32;
	}
};

class SHA224 : public SHA2
{
	public: int HashLength = 224;

	public: SHA224()
	{
		HashLength = 224;
		Reset();
	}

	public: const int GetDigestLength() override
	{
		return 224;
	}

	public: void Reset()
	{
		// Наполняем массив первыми 32 цифрами дробной части квадратных корней из простых чисел (2-19)
		_hashValues[0] = 0xC1059ED8;
		_hashValues[1] = 0x367CD507;
		_hashValues[2] = 0x3070DD17;
		_hashValues[3] = 0xF70E5939;
		_hashValues[4] = 0xFFC00B31;
		_hashValues[5] = 0x68581511;
		_hashValues[6] = 0x64F98FA7;
		_hashValues[7] = 0xBEFA4FA4;

		_blockLength = 512;
		_iterationsCount = 64;
		_wordLength = 32;
	}
};

enum HashType
{
	Sha256,
	Sha224
};

SHA224 Sha224Hasher = SHA224();

SHA256 Sha256Hasher = SHA256();

class HashUtils
{
	public: static string Hash(string text, HashType type)
	{
		return Hash(text, GetHasherByType(type));
	}

	public: static string Hash(string text, SHA2* hasher)
	{
		string result = "";
		int size = text.size();
		byte* data = (byte*)calloc(size, sizeof(byte));
		for (int i = 0; i < size; i++)
			data[i] = text[i];

		uint* hash = hasher->Hash(data, size);
		for (int i = 0; i < hasher->GetDigestLength() / 32; i++)
			result += Utils::ToHex(hash[i], 8);

		free(data);
		return result;
	}

	public: static SHA2* GetHasherByType(HashType type)
	{
		switch (type)
		{
		case HashType::Sha224:
			return &Sha224Hasher;
		default:
			return &Sha256Hasher;
		}
	}

	public: static HashType ParseHashType(string str)
	{
		return (HashType)atoi(str.c_str());
	}
};

class Form
{
	public: string Title;

	public: vector<string> Fields;

	public: WORD Colors;

	private: bool (*_callback)(Form form, vector<string>*);

	private: vector<string> _answers;

	private: int _currentField;

	private: COORD _currentCoords;

	private: HANDLE* _console;

	private: bool _isRunning;

	public: Form(string title, vector<string> fields, bool (callback)(Form form, vector<string>*), WORD colors)
	{
		Title = title;
		Fields = fields;
		Colors = colors;
		_callback = callback;
		_answers = vector<string>();
		_currentField = 0;
		_currentCoords = { 0, 0 };
		_console = nullptr;
		_isRunning = false;
	}

	public: void Show(HANDLE console, short x, short y, short width, short height)
	{
		_console = &console;
		SetConsoleTextAttribute(console, Colors);

		short titleOffset = Title == "" ? 2 : 3;
		ClearBox(x, y, width, _isRunning ? Fields.size() * 2 + titleOffset : height);
		DrawBox(x, y, width, height, Title);
		if (Title != "")
			DrawLine(x, y + 2, width, 0xCC, 0xB9, 0xCD);
		_currentCoords.X = x + 2;
		_currentCoords.Y = y + titleOffset;
		SetConsoleCursorPosition(console, _currentCoords);

		_isRunning = true;
		while (_isRunning)
		{
			if (_answers.size() == Fields.size())
			{
				_isRunning = !_callback(*this, &_answers);
				Dispose();
				if (_isRunning)
					Show(console, x, y, width, height);
				break;
			}

			WriteLine(Fields[_currentField]);
			string input = ReadLine();
			_answers.push_back(input);
			_currentField++;
		}
	}

	public: void WriteError(string error)
	{
		WriteLine(error, BACKGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	public: void WriteSuccess(string error)
	{
		WriteLine(error, BACKGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	public: void WriteLine(string text, WORD colors = 0)
	{
		if (_console != nullptr)
		{
			HANDLE console = *_console;
			if (colors != 0)
				SetConsoleTextAttribute(console, colors);
			cout << text;
			if (colors != 0)
				SetConsoleTextAttribute(console, Colors);
			_currentCoords.Y++;
			SetConsoleCursorPosition(console, _currentCoords);
		}
	}

	public: string ReadLine()
	{
		string input = "";
		getline(cin, input);
		if (_console != nullptr)
		{
			_currentCoords.Y++;
			SetConsoleCursorPosition(*_console, _currentCoords);
		}
		return input;
	}

	public: void Dispose()
	{
		_answers.clear();
		_currentField = 0;
		_currentCoords = { 0, 0 };
		_console = nullptr;
	}

	private: void DrawTextField(short x, short y, string text)
	{
		SetConsoleCursorPosition(*_console, { x, y });
		cout << text;
	}

	private: void DrawBox(short x, short y, short width, short height, string title = "")
	{
		HANDLE console = *_console;
		if (title != "")
			DrawTextField(x + width / 2 - title.size() / 2, y + 1, title);
		for (short i = x; i < x + width; i++)
		{
			for (short j = y; j < y + height; j++)
			{
				char c = 0;
				if (i == x && j == y)
					c = 0xC9;
				else if (i == x && j == y + height - 1)
					c = 0xC8;
				else if (i == x + width - 1 && j == y)
					c = 0xBB;
				else if (i == x + width - 1 && j == y + height - 1)
					c = 0xBC;
				else if (i == x + width - 2 && j == y)
					c = 'X';
				else if (i == x + width - 3 && j == y)
					c = '-';
				else if (i == x || i == x + width - 1)
					c = 0xBA;
				else if (j == y || j == y + height - 1)
					c = 0xCD;

				if (c != 0)
				{
					SetConsoleCursorPosition(console, { i, j });
					cout << c;
				}
			}
		}
	}

	private: void ClearBox(int x, int y, int width, int height)
	{
		HANDLE console = *_console;
		for (short i = x; i < x + width; i++)
		{
			for (short j = y; j < y + height; j++)
			{
				SetConsoleCursorPosition(console, { i, j });
				cout << ' ';
			}
		}
	}

	private: void DrawLine(short x, short y, short width, char start, char end, char medium)
	{
		HANDLE console = *_console;
		COORD coords = { x, y };
		SetConsoleCursorPosition(console, coords);
		cout << start;
		for (short i = 0; i < width - 2; i++)
		{
			coords.X++;
			SetConsoleCursorPosition(console, coords);
			cout << medium;
		}
		coords.X++;
		SetConsoleCursorPosition(console, coords);
		cout << end;
	}
};

const char* fileName = "db.txt";
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
Form* loginForm;
Form* registerForm;
vector<string> users{};
string* user = nullptr;
Form* runningForm = nullptr;

void WriteUserToDataBase(string user)
{
	string buffer;
	ofstream out;
	out.open(fileName, ios::app);
	out << user << endl;
	out.close();
}

bool HandleLoginForm(Form form, vector<string>* answers)
{
	string inputString = (*answers)[0] + (*answers)[1];

	string hash224 = HashUtils::Hash(inputString, HashType::Sha224);
	string hash256 = HashUtils::Hash(inputString, HashType::Sha256);

	bool founded = false;
	for (int i = 0; i < users.size(); i++)
	{
		if(users[i] == hash224 || users[i] == hash256)
		{
			if (user == nullptr)
			{
				user = &users[i];
				form.WriteSuccess("Logged as " + (*answers)[0] + "!");
			}
			else
			{
				form.WriteError("Collision detected!");
			}
		}
	}
	if (user == nullptr)
		form.WriteError("Not authorized!");

	return user != nullptr;
}

bool HandleRegisterForm(Form form, vector<string>* answers)
{
	string name = (*answers)[0];
	string password = (*answers)[1];

	HashType hashType{};
	string strHashType = (*answers)[2];

	if (strHashType == "SHA256")
		hashType = HashType::Sha256;
	else if (strHashType == "SHA224")
		hashType = HashType::Sha224;
	else
	{
		form.WriteError("Invalid hash type!");
		form.WriteError("Proper: SHA256/SHA224");
		return false;
	}
	string inputString = name + password;
	string hash = HashUtils::Hash(inputString, hashType);

	string hash224 = HashUtils::Hash(inputString, HashType::Sha224);
	string hash256 = HashUtils::Hash(inputString, HashType::Sha256);

	for (int i = 0; i < users.size(); i++)
	{
		if (users[i] == hash224 || users[i] == hash256)
		{
			form.WriteError("This user is already exist! (Collision)");
			return false;
		}
	}

	user = &hash;
	users.push_back(*user);
	WriteUserToDataBase(*user);
	form.WriteSuccess("Registered account:");
	form.WriteSuccess(name);
	return true;
}

bool HandleChooseForm(Form form, vector<string>* answers)
{
	string option = (*answers)[0];
	if (option == "Login")
	{
		runningForm = loginForm;
		return true;
	}
	else if (option == "Register")
	{
		runningForm = registerForm;
		return true;
	}
	else
		form.WriteError("Invalid form!");
	return false;
}

int main()
{
	vector<string> loginFields = vector<string>();
	loginFields.push_back("Enter username:");
	loginFields.push_back("Enter password:");

	vector<string> registerFields = vector<string>();
	registerFields.push_back("Enter username:");
	registerFields.push_back("Enter password:");
	registerFields.push_back("Enter hash type:");

	vector<string> menuFields = vector<string>();
	menuFields.push_back("Choose option: Login/Register");

	loginForm = new Form("Login", loginFields, HandleLoginForm, BACKGROUND_BLUE | BACKGROUND_GREEN);
	registerForm = new Form("Register", registerFields, HandleRegisterForm, BACKGROUND_GREEN | BACKGROUND_RED);
	Form menuForm = Form("Menu", menuFields, HandleChooseForm, BACKGROUND_RED | BACKGROUND_BLUE);

	short x = 40;
	short y = 7;
	short width = 35;
	short height = 15;

	string buffer;
	ifstream out;
	out.open(fileName);
	while (getline(out, buffer))
		users.push_back(buffer);
	out.close();

	menuForm.Show(console, x, y, width, height);
	if (runningForm)
		runningForm->Show(console, x, y, width, height);

	return 0;
}