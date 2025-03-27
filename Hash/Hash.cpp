#include <iostream>

using namespace std;

static const uint32_t partsOfCubicRoots[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

class SHA256
{
	public: uint32_t PartsOfSquareRoots[8];

	public: SHA256()
	{
		PartsOfSquareRoots[0] = 0x6A09E667;
		PartsOfSquareRoots[1] = 0xBB67AE85;
		PartsOfSquareRoots[2] = 0x3C6EF372;
		PartsOfSquareRoots[3] = 0xA54FF53A;
		PartsOfSquareRoots[4] = 0x510E527F;
		PartsOfSquareRoots[5] = 0x9B05688C;
		PartsOfSquareRoots[6] = 0x1F83D9AB;
		PartsOfSquareRoots[7] = 0x5BE0CD19;
	}


	public: uint32_t Hash(uint32_t message)
	{
		uint32_t result = message | 1;

	}

	// Логический сдвиг вправо
	private: uint32_t Shift(uint32_t value, int bits)
	{
		return value >> bits;
	}

	// Циклический сдвиг вправо
	private: uint32_t Rotate(uint32_t value, int bits)
	{
		return value >> bits | value << (32 - bits);
	}
};

class HashUtils
{
	private: static SHA256 _hasher;

	public: static string Hash(string text)
	{
		return _hasher.Hash(text);
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
	
}