#include "third-party/BigIntegerLibrary.hh"
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

constexpr int UNSIGNED_BIT_COUNT = sizeof(unsigned long) * 16;

using PrimeNumberCheckFunction = bool(*)(const BigUnsigned&, int);

BigUnsigned BigUnsignedRandom(int bitCount) // ������� ��������� ���������� ����� ��������� �������.
{
	int blockCount = bitCount / UNSIGNED_BIT_COUNT; // ����������� ������� � ����� � ������ � ������.
	vector<unsigned long> blocks(blockCount); // �������� ������ ������.
	for (auto& block : blocks)
	{
		block = unsigned(rand()); // ���������������� ������� �������� ������� �� rand.
		if (rand() & 1) // ����������� 50%.
		{
			block |= 1 << (UNSIGNED_BIT_COUNT - 1); // rand ���������� ����� �� 0 �� INT_MAX, � ���� �� 0 �� ULONG_MAX.
		}
	}
	BigUnsigned result(blocks.data(), blockCount); // �������� ������� ����� �� ������� ��������.
	return result;
}

BigUnsigned BigUnsignedRandomOdd(int bitCount) // ��������� ���������� !���������! �����.
{
	BigUnsigned result = BigUnsignedRandom(bitCount); // ������������ ������� ������� ��������� �����.
	if ((result & 1) == 0) // ���� ��� ���� ������,
		result |= 1; // ������ ��� ��������.
	return result;
}

bool MillerRabinSingleCheck(const BigUnsigned& n, const BigUnsigned& a, const BigUnsigned& d, int s) // ���� �� �������� �������-������ ��� ����������� �����.
{
	if (modexp(a, d, n) == 1) // a^d = 1 (mod n)
		return true;

	for (int r = 0; r < s; r++)
	{
		BigUnsigned power = (BigUnsigned(1) << r) * d; // power = 2^r * d
		if (modexp(a, power, n) == (n - 1)) // a^power = -1 (mod n)
		{
			return true;
		}
	}
	return false;
}

bool MillerRabinCheck(const BigUnsigned& n, int k) // ���� �� �������� �������-������ �� k �������
{
	BigUnsigned d = n - 1;
	int s = 0;
	while ((d % 2) == 0)
	{
		s++;
		d /= 2;
	}
	for (int i = 0; i < k; i++) // k ��� ����� ����������� �� �������� �� ��������� ������.
	{
		BigUnsigned a = BigUnsignedRandom(n.getCapacity() * UNSIGNED_BIT_COUNT) % n;
		if (!MillerRabinSingleCheck(n, a, d, s))
			return false;
	}

	return true; // ���� �������� k ��������, �� ����� ��������� � ������������ 1/4^k. ��� 8 �������� ��� ����� 0.0015%
}

BigUnsigned GeneratePrimeNumber(int bits, int checkCount, PrimeNumberCheckFunction checkFunction) // ��������� �������� �����.
{
	milliseconds beginTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); // ����� ������.
	BigUnsigned result;
	do
	{
		result = BigUnsignedRandomOdd(bits); // ��������� ���������� ��������� �����.
	} while (!checkFunction(result, checkCount)); // ���� ��� ����� �� ������ ��������, ������������ �����.
	milliseconds endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); // ����� ���������.
	milliseconds delta = endTime - beginTime;
	cout << "A prime number generated in " << delta.count() << " ms." << " Rounds = " << checkCount << "." << endl;
	return result;
}


struct RsaKey // RSA ����. ��������� ��� ���������.
{
	BigUnsigned e;
	BigUnsigned n;
};

struct RsaKeyPair // ���� RSA ������.
{
	RsaKey publicKey;
	RsaKey privateKey;
};

RsaKeyPair RsaGenerateKeyPair(int bits, PrimeNumberCheckFunction checkFunction) // ��������� ���� �����. ��������� �� �������� ��� ������ checkFunction.
{
	BigUnsigned p = GeneratePrimeNumber(bits, 16, checkFunction);
	BigUnsigned q = GeneratePrimeNumber(bits, 16, checkFunction);
	BigUnsigned n = p * q;
	BigUnsigned euler = (p - 1) * (q - 1); // ������� ������ ��� n.
	BigUnsigned e = 65537;
	BigUnsigned d = modinv(e, euler); // ��������� �����, ���������������� ��������� ����� e �� ������ euler(n)

	RsaKeyPair result; // ������������ ���� ������.
	result.publicKey.e = e;
	result.publicKey.n = n;
	result.privateKey.e = d;
	result.privateKey.n = n;
	return result;
}

int SaveKey(string path, const RsaKey& key) // ���������� ����� � ��������� ����.
{
	ofstream stream(path);
	if (stream.fail())
	{
		cerr << "Can't open " << path << ".";
		return 1;
	}
	stream << key.e << endl << key.n;
	stream.close();
	return 0;
}

int LoadKey(string path, RsaKey& key) // �������� ����� �� ���������� �����.
{
	ifstream stream(path);
	if (stream.fail())
	{
		cerr << "Can't open " << path << ".";
		return 1;
	}
	string e;
	string n;

	stream >> e >> n;
	int exitCode = 0;
	try
	{
		key.e = stringToBigUnsigned(e);
		key.n = stringToBigUnsigned(n);
	}
	catch(...)
	{
		cerr << "Invalid key.";
		exitCode = 1;
	}

	stream.close();
	return exitCode;
}

int EncryptFile(string inputPath, RsaKey& key, string outputPath) // ���������� ����� �� �����.
{
	ifstream input(inputPath);
	if (input.fail())
	{
		cerr << "Can't open " << inputPath << ".";
		return 1;
	}
	ofstream output(outputPath);
	if (output.fail())
	{
		cerr << "Can't open " << inputPath << ".";
		return 1;
	}

	string strValue;
	input >> strValue;
	BigUnsigned value;
	try
	{
		value = stringToBigUnsigned(strValue);
	}
	catch (...)
	{
		cerr << "File contains invalide data.";
		return 1;
	}
	BigUnsigned encryptedValue = modexp(value, key.e, key.n); // value^e % n
	output << encryptedValue;

	input.close();
	output.close();
	return 0;
}

int GenerateKeysMode(int argc, char** argv) // ����� ������ ��������� ������.
{
	if (argc != 4)
	{
		cerr << "Invalid command line parameters.";
		return 1;
	}

	PrimeNumberCheckFunction checkFunction;
	if (argv[2] == string("Miller-Rabin"))
		checkFunction = MillerRabinCheck;
	else
	{
		cerr << "Invalid algorithm.";
		return 1;
	}

	int keysize;
	if (argv[3] == string("512"))
		keysize = 512;
	else
	{
		cerr << "Invalid key size.";
		return 1;
	}

	srand(unsigned(time(nullptr))); // ������������� ���������� ��������� �����.
	auto pair = RsaGenerateKeyPair(keysize, checkFunction);
	if (int exitcode = SaveKey("public-key.txt", pair.publicKey))
		return exitcode;
	if (int exitcode = SaveKey("private-key.txt", pair.privateKey))
		return exitcode;
	return 0;
}

int EncryptMode(int argc, char** argv) // ����� ����������.
{
	if (argc != 3 && argc != 4)
	{
		cerr << "Invalid command line parameters.";
		return 1;
	}
	RsaKey key;
	if (auto exitCode = LoadKey(argv[2], key))
		return exitCode;

	string inputPath = argv[1];
	string outputPath = (argc == 4) ? argv[3] : (inputPath + ".encrypted");
	if (auto exitCode = EncryptFile(inputPath, key, outputPath))
		return exitCode;

	return 0;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		cout << R"(OVERVIEW: RSA

USAGE: RSA /generate-keys algorithm keysize
       RSA filepath keypath [output]

ALGORITHMS:
	Miller-Rabin

KEY SIZES:
	512
)";
		return 0;
	}

	if (argv[1] == string("/generate-keys"))
		return GenerateKeysMode(argc, argv);
	else
		return EncryptMode(argc, argv);
}
