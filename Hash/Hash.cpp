#include <iostream>
#include <string>

typedef unsigned int uint;
typedef uint8_t byte;

using namespace std;

string _alphabet = "0123456789ABCDEF";

class Utils 
{
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

class SHA256
{
	// Массив  из первых 32 цифр дробной части кубических корней из простых чисел (2-19)
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

	private: uint _hashValues[8];

	private: int _blockLength = 512;

	private: int _iterationNumber = 64;

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
	}

	public: uint* Hash(byte* message, size_t size)
	{
		Reset();

		int remainingSize = size;

		byte block[64]{};

		for (int i = 0; i < size; i++)
			block[i] = message[i];
		block[size] = 0x80;

		
		size_t bitLength = size * 8;
		block[63] = bitLength;
		block[62] = bitLength >> 8;
		block[61] = bitLength >> 16;
		block[60] = bitLength >> 24;
		block[59] = bitLength >> 32;
		block[58] = bitLength >> 40;
		block[57] = bitLength >> 48;
		block[56] = bitLength >> 56;

		uint words[64]{};

		for (int i = 0; i < 16; i++)
		{
			words[i] = (uint)block[i * 4] << 24 | (uint)block[i * 4 + 1] << 16 | (uint)block[i * 4 + 2] << 8 | (uint)block[i * 4 + 3];
		}

		for (int i = 16; i < 64; i++)
			words[i] = words[i - 16] + Sigma0(words[i - 15]) + words[i - 7] + Sigma1(words[i - 2]);


		uint a = _hashValues[0];
		uint b = _hashValues[1];
		uint c = _hashValues[2];
		uint d = _hashValues[3];
		uint e = _hashValues[4];
		uint f = _hashValues[5];
		uint g = _hashValues[6];
		uint h = _hashValues[7];


		for (int i = 0; i < 64; i++)
		{
			int temp2 = RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
			temp2 += Majority(a, b, c);
			int temp1 = h;
			temp1 += RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
			temp1 += Choose(e, f, g) + _partsOfCubicRoots[i] + words[i];

			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}

		_hashValues[0] += a;
		_hashValues[1] += b;
		_hashValues[2] += c;
		_hashValues[3] += d;
		_hashValues[4] += e;
		_hashValues[5] += f;
		_hashValues[6] += g;
		_hashValues[7] += h;

		return _hashValues;
	}

	// Логический сдвиг вправо
	private: uint ShiftRight(uint value, int bits)
	{
		return value >> bits;
	}

	// Циклический сдвиг вправо
	private: uint RotateRight(uint value, int bits)
	{
		return (value >> bits) | (value << (32 - bits));
	}

	private: uint Choose(uint e, uint f, uint g)
	{
		return (e & f) ^ (~e & g);
	}

	private: uint Majority(uint a, uint b, uint c)
	{
		return (a & (b | c)) | (b & c);
	}

	private: uint Sigma0(uint number)
	{
		return RotateRight(number, 7) ^ RotateRight(number, 18) ^ ShiftRight(number, 3);
	}

	private: uint Sigma1(uint number)
	{
		return RotateRight(number, 17) ^ RotateRight(number, 19) ^ ShiftRight(number, 10);
	}
};

class HashUtils
{
	private: static SHA256 _hasher;

	public: static string Hash(string text)
	{
		return "";// _hasher.Hash(text);
	}
};

class User
{
	public: string Name;

	private: string _hashedPassword;

	public: User(string name, string password)
	{
		Name = name; 
		_hashedPassword = HashUtils::Hash(password);
	}

	public: bool Verify(string password)
	{
		return HashUtils::Hash(password) == _hashedPassword;
	}
};

int main()
{
	SHA256 s = SHA256();
	string message = "Yaner2004";
	int size = message.size();
	byte* data = (byte*)calloc(size, sizeof(byte));
	for (int i = 0; i < size; i++)
		data[i] = message[i];
	uint* hash = s.Hash(data, size);

	cout << "HASH:" << endl;
	for (int i = 0; i < 8; i++)
	{
		cout << hash[i];
	}
	cout << endl;

	cout << "Hex:" << endl;
	for (int i = 0; i < 8; i++)
	{
		cout << Utils::ToHex(hash[i]);
	}
	cout << endl;

	free(data);
}