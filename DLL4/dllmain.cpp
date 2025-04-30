#include "pch.h"
#include <iostream>

using namespace std;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

double PI = 3.14159;

double GetFirstNumber(double x)
{
	return 1 / (x * x - 1);
}

double Addition(double x, int n)
{
	if (x * x == n * n)
		return INFINITY;
	return 1 + (2 * n - 1) / (x * x - n * n);
}

double GetNumber(double x, int n, double last)
{
	return last * Addition(x, n);
}

extern "C" __declspec(dllexport) const char* AuthorName()
{
	return "Морданенко Ян";
}

extern "C" __declspec(dllexport) const char* ToString()
{
	return "ctg(pi*x)";
}

extern "C" __declspec(dllexport) double myf_math(double x, double a)
{
	return 1 / tan(PI * x);
}

extern "C" __declspec(dllexport) double myf_3(double x, double a, double epsilon)
{
	double y1 = 1 / x / PI;
	double y2 = 2 * x / PI;
	int n = 1;
	double lastValue = GetFirstNumber(x);
	double sum = lastValue;
	while (true)
	{
		double newValue = GetNumber(x, ++n, lastValue);
		if (newValue == INFINITY)
			return INFINITY;
		if (abs(lastValue - newValue) < epsilon)
			break;
		lastValue = newValue;
		sum += lastValue;
	}
	return y1 + y2 * sum;
}

extern "C" __declspec(dllexport) unsigned short GetTableColor()
{
	return FOREGROUND_RED | FOREGROUND_GREEN  | BACKGROUND_GREEN;
}
