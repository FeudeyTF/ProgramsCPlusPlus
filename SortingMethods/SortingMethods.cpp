#include <iostream>
#include <array>
#include <iomanip>
#include <string>

using namespace std;

enum SortingMethod
{
	Bubble,
	Selection,
	Insertion,
	Shell,
	Quick
};

string EnumToString(SortingMethod method)
{
	switch (method)
	{
	case SortingMethod::Bubble:
		return "Bubble";
	case SortingMethod::Selection:
		return "Selection";
	case SortingMethod::Insertion:
		return "Insertion";
	case SortingMethod::Shell:
		return "Shell";
	default:
		return "Quick";
	}
}

class Sorting
{
public:
	static void PrintArray(int* array, int count)
	{
		cout << "[ ";
		for (int i = 0; i < count; i++)
			cout << array[i] << " ";
		cout << " ]" << endl;
	}

	static int* Sort(int* values, int count, int (*comparer)(int, int), int* swapsCount, int* assignmentsCount, int* comparisonsCount, SortingMethod method = SortingMethod::Quick)
	{
		switch (method)
		{
		case SortingMethod::Bubble:
			return BubbleSort(values, count, comparer, swapsCount, comparisonsCount);
		case SortingMethod::Insertion:
			return InsertionSort(values, count, comparer, assignmentsCount, comparisonsCount);
		case SortingMethod::Selection:
			return SelectionSort(values, count, comparer, swapsCount, comparisonsCount);
		case SortingMethod::Shell:
			return ShellSort(values, count, comparer, assignmentsCount, comparisonsCount);
		default:
			return QuickSort(values, count, comparer, swapsCount, comparisonsCount);
		}
	}

private:
	static void Swap(int* i, int* j)
	{
		int z = *i;
		*i = *j;
		*j = z;
	}

	static int* BubbleSort(int* values, int count, int (*comparer)(int, int), int* swapsCount, int* comparisonsCount)
	{
		for (int i = 0; i < count; i++)
		{
			for (int j = i + 1; j < count; j++)
			{
				if (comparer(values[i], values[j]))
				{
					Swap(&values[i], &values[j]);
					++*swapsCount;
				}
				++ * comparisonsCount;
			}
		}
		return values;
	}

	static int* SelectionSort(int* values, int count, int (*comparer)(int, int), int* swapsCount, int* comparisonsCount)
	{
		for (int i = 0; i < count; i++)
		{
			int minValueIndex = i;
			for (int j = i + 1; j < count; j++)
			{
				if (comparer(values[minValueIndex], values[j]))
					minValueIndex = j;
				++*comparisonsCount;
			}

			if (i != minValueIndex)
			{
				Swap(&values[i], &values[minValueIndex]);
				++*swapsCount;
			}
		}
		return values;
	}

	static int* InsertionSort(int* values, int count, int (*comparer)(int, int), int* assignmentsCount, int* comparisonsCount)
	{
		for (int i = 1; i < count; i++)
		{
			int valueIndex = i;
			int value = values[i];
			++*assignmentsCount;
			for (int j = i; j > 0; j--)
			{
				++*comparisonsCount;
				if (!comparer(values[j - 1], value))
					break;
				valueIndex--;

				values[j] = values[j - 1];
				++*assignmentsCount;
			}

			if (i != valueIndex)
			{
				values[valueIndex] = value;
				++*assignmentsCount;
			}
		}
		return values;
	}

	static int* ShellSort(int* values, int count, int (*comparer)(int, int), int* assignmentsCount, int* comparisonsCount)
	{
		for (int step = count / 2; step > 0; step /= 2)
		{
			for (int i = step; i < count; i++)
			{
				int temp = values[i];
				int j = 0;
				for (j = i; j >= step && comparer(values[j - step], temp); j -= step)
				{
					++*comparisonsCount;
					values[j] = values[j - step];
					++*assignmentsCount;
				}

				values[j] = temp;
				++*assignmentsCount;
			}
		}

		return values;
	}

	static int* QuickSort(int* values, int count, int (*comparer)(int, int), int* swapsCount, int* comparisonsCount)
	{
		return QuickSortSorter(values, comparer, 0, count - 1, swapsCount, comparisonsCount);
	}

	static int* QuickSortSorter(int* values, int (*comparer)(int, int), int start, int end, int* swapsCount, int* comparisonsCount)
	{
		if (start < end)
		{
			int pivot = values[end];
			int swappingItemIndex = start;

			for (int j = start; j < end; j++)
			{
				++*comparisonsCount;
				if (comparer(pivot, values[j]))
				{
					if (swappingItemIndex != j)
					{
						Swap(&values[swappingItemIndex], &values[j]);
						++*swapsCount;
					}
					swappingItemIndex++;
				}
			}

			Swap(&values[swappingItemIndex], &values[end]);
			++*swapsCount;

			QuickSortSorter(values, comparer, start, swappingItemIndex - 1, swapsCount, comparisonsCount);
			QuickSortSorter(values, comparer, swappingItemIndex + 1, end, swapsCount, comparisonsCount);
		}
		return values;
	}
};

class Drawer
{
public:
	static void DrawHorizontalLine(int width, char start, char end)
	{
		cout << start << string(width, (char)0xCD) << end << endl;
	}


	static void DrawHorizontalLine(int width, bool top)
	{
		DrawHorizontalLine(width, top ? (char)0xC9 : (char)0xC8, top ? (char)0xBB : (char)0xBC);
	}

	static void DrawTextBox(string text, int width)
	{
		DrawHorizontalLine(width, true);
		cout << (char)0xBA << " " << left << setw(width - 2) << text << " " << (char)0xBA << endl;
		DrawHorizontalLine(width, false);
	}

	static void DisplayMatrix(int** matrix, int height, int width, string title)
	{
		int maxNumWidth = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int len = to_string(matrix[i][j]).length();
				maxNumWidth = max(maxNumWidth, len);
			}
		}

		int boxWidth = (maxNumWidth + 1) * width + 2;

		DrawTextBox(title, boxWidth);

		DrawHorizontalLine(boxWidth, true);
		for (int i = 0; i < height; i++)
		{
			cout << (char)0xBA << " ";
			for (int j = 0; j < width; j++)
			{
				cout << setw(maxNumWidth) << matrix[i][j] << " ";
			}
			cout << " " << (char)0xBA << endl;
		}
		DrawHorizontalLine(boxWidth, false);
	}

	static void DisplaySortingInformation(string methodName, int comparisons, int swaps, int assignments)
	{
		int width = 50;
		DrawTextBox("Statistics", width + 2);
		DrawHorizontalLine(width + 2, true);
		cout << (char)0xBA << " " << left << setw(width) << methodName + " sort" << " " << (char)0xBA << endl;
		cout << (char)0xBA << " " << left << setw(width - to_string(comparisons).length()) << "Comparisons: " << comparisons << " " << (char)0xBA << endl;
		cout << (char)0xBA << " " << left << setw(width - to_string(swaps).length()) << "Swaps: " << swaps << " " << (char)0xBA << endl;
		cout << (char)0xBA << " " << left << setw(width - to_string(assignments).length()) << "Assignments: " << assignments << " " << (char)0xBA << endl;
		cout << (char)0xBA << " " << left << setw(width - to_string(assignments + 3 * swaps).length()) << "True Assignments: " << assignments + 3 * swaps << " " << (char)0xBA << endl;
		DrawHorizontalLine(width + 2, false);
	}
};

int GetDigitsSum(int number)
{
	int sum = 0;
	while (number != 0)
	{
		sum += number % 10;
		number /= 10;
	}
	return sum;
}
int Comparer1(int a, int b)
{
	return a < b;
}

int Comparer(int a, int b)
{
	return GetDigitsSum(a) < GetDigitsSum(b);
}


int main()
{
	srand(time(0));
	int comparisonsCount = 0;
	int swapsCount = 0;
	int assignmentsCount = 0;

	int width;
	int height;

	cout << "Enter matrix width:" << endl;
	cin >> width;
	cout << "Enter matrix height:" << endl;
	cin >> height;

	int** matrix = (int**)calloc(height, sizeof(int*));
	int** reservedMatrix = (int**)calloc(height, sizeof(int*));
	int* sortingArray = (int*)calloc(height, sizeof(int));

	for (int i = 0; i < height; i++)
	{
		matrix[i] = (int*)calloc(width, sizeof(int));
		reservedMatrix[i] = (int*)calloc(width, sizeof(int));
		for (int j = 0; j < width; j++)
		{
			matrix[i][j] = (int)round(rand());
			reservedMatrix[i][j] = matrix[i][j];
		}
	}

	Drawer::DisplayMatrix(matrix, height, width, "Initial Matrix");
	cout << endl;

	for (int k = 0; k < 5; k++)
	{
		SortingMethod method = (SortingMethod)k;

		for (int i = 0; i < width; i += 2)
		{
			for (int j = 0; j < height; j++)
				sortingArray[j] = matrix[j][i];
			Sorting::Sort(sortingArray, height, Comparer, &swapsCount, &assignmentsCount, &comparisonsCount, method);
			for (int j = 0; j < height; j++)
				reservedMatrix[j][i] = sortingArray[j];
		}
		cout << endl;
		cout << endl;
		Drawer::DisplayMatrix(reservedMatrix, height, width, "Sorted Matrix (" + EnumToString(method) + ")");
		Drawer::DisplaySortingInformation(EnumToString(method), comparisonsCount, swapsCount, assignmentsCount);

		comparisonsCount = 0;
		swapsCount = 0;
		assignmentsCount = 0;
	}

	for (int i = 0; i < height; i++)
	{
		free(matrix[i]);
		free(reservedMatrix[i]);
	}
	free(sortingArray);
	return 0;
}
