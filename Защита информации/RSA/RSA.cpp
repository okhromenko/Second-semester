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

BigUnsigned BigUnsignedRandom(int bitCount) // Функция генерации случайного числа заданного размера.
{
	int blockCount = bitCount / UNSIGNED_BIT_COUNT; // Конвертация размера в битах в размер в блоках.
	vector<unsigned long> blocks(blockCount); // Создаётся массив блоков.
	for (auto& block : blocks)
	{
		block = unsigned(rand()); // Инициализируется числами обычного размера из rand.
		if (rand() & 1) // Вероятность 50%.
		{
			block |= 1 << (UNSIGNED_BIT_COUNT - 1); // rand генерирует числа от 0 до INT_MAX, а надо от 0 до ULONG_MAX.
		}
	}
	BigUnsigned result(blocks.data(), blockCount); // Создаётся длинное число из массива коротких.
	return result;
}

BigUnsigned BigUnsignedRandomOdd(int bitCount) // Генерация случайного !нечётного! числа.
{
	BigUnsigned result = BigUnsignedRandom(bitCount); // Генерируется обычное длинное случайное число.
	if ((result & 1) == 0) // Если оно было чётным,
		result |= 1; // делаем его нечётным.
	return result;
}

bool MillerRabinSingleCheck(const BigUnsigned& n, const BigUnsigned& a, const BigUnsigned& d, int s) // Тест на простоту Миллера-Рабина для конкретного числа.
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

bool MillerRabinCheck(const BigUnsigned& n, int k) // Тест на простоту Миллера-Рабина на k раундов
{
	BigUnsigned d = n - 1;
	int s = 0;
	while ((d % 2) == 0)
	{
		s++;
		d /= 2;
	}
	for (int i = 0; i < k; i++) // k раз число проверяется на простоту на случайных числах.
	{
		BigUnsigned a = BigUnsignedRandom(n.getCapacity() * UNSIGNED_BIT_COUNT) % n;
		if (!MillerRabinSingleCheck(n, a, d, s))
			return false;
	}

	return true; // Если пройдено k проверок, то число составное с вероятностью 1/4^k. Для 8 проверок это около 0.0015%
}

BigUnsigned GeneratePrimeNumber(int bits, int checkCount, PrimeNumberCheckFunction checkFunction) // Генерация простого числа.
{
	milliseconds beginTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); // Время начала.
	BigUnsigned result;
	do
	{
		result = BigUnsignedRandomOdd(bits); // Генерация случайного нечётного числа.
	} while (!checkFunction(result, checkCount)); // Если это число не прошло проверку, генерируется новое.
	milliseconds endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()); // Время окончания.
	milliseconds delta = endTime - beginTime;
	cout << "A prime number generated in " << delta.count() << " ms." << " Rounds = " << checkCount << "." << endl;
	return result;
}


struct RsaKey // RSA ключ. Публичный или приватный.
{
	BigUnsigned e;
	BigUnsigned n;
};

struct RsaKeyPair // Пара RSA ключей.
{
	RsaKey publicKey;
	RsaKey privateKey;
};

RsaKeyPair RsaGenerateKeyPair(int bits, PrimeNumberCheckFunction checkFunction) // Генерация пару чисел. Проверяем на простоту при помощи checkFunction.
{
	BigUnsigned p = GeneratePrimeNumber(bits, 16, checkFunction);
	BigUnsigned q = GeneratePrimeNumber(bits, 16, checkFunction);
	BigUnsigned n = p * q;
	BigUnsigned euler = (p - 1) * (q - 1); // Функция Эйлера для n.
	BigUnsigned e = 65537;
	BigUnsigned d = modinv(e, euler); // Генерация числа, мультипликативно обратного числу e по модулю euler(n)

	RsaKeyPair result; // Формирование пары ключей.
	result.publicKey.e = e;
	result.publicKey.n = n;
	result.privateKey.e = d;
	result.privateKey.n = n;
	return result;
}

int SaveKey(string path, const RsaKey& key) // Сохранение ключа в указанный файл.
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

int LoadKey(string path, RsaKey& key) // Загрузка ключа из указанного файла.
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

int EncryptFile(string inputPath, RsaKey& key, string outputPath) // Шифрование числа из файла.
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

int GenerateKeysMode(int argc, char** argv) // Режим програ генерации ключей.
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

	srand(unsigned(time(nullptr))); // Инициализация генератора случайных чисел.
	auto pair = RsaGenerateKeyPair(keysize, checkFunction);
	if (int exitcode = SaveKey("public-key.txt", pair.publicKey))
		return exitcode;
	if (int exitcode = SaveKey("private-key.txt", pair.privateKey))
		return exitcode;
	return 0;
}

int EncryptMode(int argc, char** argv) // Режим шифрования.
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
