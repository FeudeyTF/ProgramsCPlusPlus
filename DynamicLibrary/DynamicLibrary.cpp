#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

typedef unsigned short Color;

typedef double(*MathFunction)(double, double);

typedef double(*MyFunction)(double, double, double);

typedef const char*(*StringFunction)();

typedef Color (*GetColorFunction)();

void ThrowDllError(string error)
{
	cerr << "Произошла ошибка в DLL: " << error << endl;
}

double GetResidual(double f, double F)
{
	return sqrt(abs(f * f - F * F));
}

double EPSILON = 0.000001;

class ITableColumn
{
	public: virtual string ToString() = 0;
};

class ITableRow
{
	public: virtual vector<ITableColumn*>* GetTableColumns() = 0;

	public: virtual bool SetAfterRowBorder() = 0;
};

class Table
{
	private: vector<ITableRow*> _rows;

	private: int _columnsCount;

	private: Color _color;

	public: Table(int columnsCount, Color color)
	{
		_columnsCount = columnsCount;
		_color = color;
	}

	public: void Print(HANDLE console)
	{
		int maxColumnSize = 0;
		for (int i = 0; i < _rows.size(); i++)
		{
			vector<ITableColumn*>* columns = _rows[i]->GetTableColumns();
			for (int j = 0; j < columns->size(); j++)
			{
				string columnValue = columns->at(j)->ToString();
				if (maxColumnSize < columnValue.size())
					maxColumnSize = columnValue.size();
			}
		}

		int offset = 2;
		int tableWidth = (maxColumnSize + offset) * _columnsCount + 5;

		SetConsoleTextAttribute(console, _color);
		DrawLine(tableWidth, '<', '>', '-');
		for (int i = 0; i < _rows.size(); i++)
		{
			vector<ITableColumn*>* columns = _rows[i]->GetTableColumns();
			vector<string> columnValues{};
			for (int j = 0; j < _columnsCount; j++)
			{
				string columnValue = "";
				if(j < columns->size())
					columnValue = columns->at(j)->ToString();
				columnValues.push_back(columnValue);
			}

			cout << "|";
			for (int j = 0; j < _columnsCount; j++)
			{
				string value = columnValues[j];
				cout << " " << value << setw(maxColumnSize - value.size() + offset) << " |";
			}

			cout << endl;
			if (_rows[i]->SetAfterRowBorder())
				DrawLine(tableWidth, '<', '>', '-');
		}
		DrawLine(tableWidth, '<', '>', '-');
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}

	public: void AddRow(ITableRow* row)
	{
		_rows.push_back(row);
	}

	private: void DrawLine(short width, char start, char end, char medium)
	{
		cout << start;
		for (short i = 0; i < width - 2; i++)
		{
			cout << medium;
		}
		cout << end << endl;
	}
};

class StringColumn : public ITableColumn
{
	private: string _value;

	public: StringColumn(string value)
	{
		_value = value;
	}

	public: string ToString() override
	{
		return _value;
	}
};

class DoubleColumn : public ITableColumn
{
	private: double _value;

	public: DoubleColumn(double value)
	{
		_value = value;
	}

	public: string ToString() override
	{
		return to_string(_value);
	}
};

class HeaderTableRow : public ITableRow
{
	private: string _programAuthorName;

	private: string _dllAuthorName;

	private: vector<ITableColumn*>* _cachedColumns;

	public: HeaderTableRow(string programAuthorName, string dllAuthorName)
	{
		_programAuthorName = programAuthorName;
		_dllAuthorName = dllAuthorName;
		_cachedColumns = GetColumns();
	}

	~HeaderTableRow()
	{
		delete _cachedColumns;
	}

	private: vector<ITableColumn*>* GetColumns()
	{
		vector<ITableColumn*>* columns = new vector<ITableColumn*>();
		StringColumn* programAuthorTitle = new StringColumn("Автор программы:");
		StringColumn* programAuthorName = new StringColumn(_programAuthorName);
		StringColumn* dllAuthorTitle = new StringColumn("Автор DLL:");
		StringColumn* dllAuthorName = new StringColumn(_dllAuthorName);
		columns->push_back(programAuthorTitle);
		columns->push_back(programAuthorName);
		columns->push_back(dllAuthorTitle);
		columns->push_back(dllAuthorName);
		return columns;
	}
	
	public: vector<ITableColumn*>* GetTableColumns() override
	{
		return _cachedColumns;
	}

	public: bool SetAfterRowBorder() override
	{
		return true;
	}
};

class StringTableRow : public ITableRow
{
	private: vector<string> _values;

	private: vector<ITableColumn*>* _cachedColumns;

	private: bool _setAfterRowBorder;

	public: StringTableRow(vector<string> values, bool setAfterRowBorder = false)
	{
		_values = values;
		_cachedColumns = GetColumns();
		_setAfterRowBorder = setAfterRowBorder;
	}

	~StringTableRow()
	{
		delete _cachedColumns;
	}

	private: vector<ITableColumn*>* GetColumns()
	{
		vector<ITableColumn*>* columns = new vector<ITableColumn*>();
		for (int i = 0; i < _values.size(); i++)
		{
			StringColumn* column = new StringColumn(_values[i]);
			columns->push_back(column);
		}
		return columns;
	}

	public: vector<ITableColumn*>* GetTableColumns() override
	{
		return _cachedColumns;
	}

	public: bool SetAfterRowBorder() override
	{
		return _setAfterRowBorder;
	}
};

class DoubleTableRow : public ITableRow
{
	private: vector<double> _numbers;

	private: vector<ITableColumn*>* _cachedColumns;

	private: bool _setAfterRowBorder;

	public: DoubleTableRow(vector<double> numbers, bool setAfterRowBorder = false)
	{
		_numbers = numbers;
		_cachedColumns = GetColumns();
		_setAfterRowBorder = setAfterRowBorder;
	}

	~DoubleTableRow()
	{
		delete _cachedColumns;
	}

	private: vector<ITableColumn*>* GetColumns()
	{
		vector<ITableColumn*>* columns = new vector<ITableColumn*>();
		for (int i = 0; i < _numbers.size(); i++)
		{
			DoubleColumn* column = new DoubleColumn(_numbers[i]);
			columns->push_back(column);
		}
		return columns;
	}

	public: vector<ITableColumn*>* GetTableColumns() override
	{
		return _cachedColumns;
	}

	public: bool SetAfterRowBorder() override
	{
		return _setAfterRowBorder;
	}
};

int main()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	setlocale(LC_ALL, "");
	HMODULE library = LoadLibrary(L"DLL4");
	if (library != NULL)
	{
		MathFunction mathFunction = (MathFunction)GetProcAddress(library, "myf_math");
		MyFunction myFunction = (MyFunction)GetProcAddress(library, "myf_3");
		StringFunction authorName = (StringFunction)GetProcAddress(library, "AuthorName");
		StringFunction functionName = (StringFunction)GetProcAddress(library, "ToString");
		GetColorFunction getColorFunction = (GetColorFunction)GetProcAddress(library, "GetTableColor");

		if (mathFunction == NULL)
		{
			ThrowDllError("Невозможно найти функцию из библиотеки - myf_math(double, double, double)");
			return 1;
		}

		if (getColorFunction == NULL)
		{
			ThrowDllError("Невозможно найти функцию из библиотеки для выбора цвета таблицы - GetTableColor()");
			return 1;
		}

		if (myFunction == NULL)
		{
			ThrowDllError("Невозможно найти пользовательскую функцию - myf_3(double, double)");
			return 1;
		}

		if (authorName == NULL)
		{
			ThrowDllError("Невозможно найти функцию для получения имени автора - AuthorName()");
			return 1;
		}

		if (functionName == NULL)
		{
			ThrowDllError("Невозможно найти функцию для получения названия функции - ToString()");
			return 1;
		}

		double x;
		double deltaX;
		double idealX;
		cout << "Введите стартовый X" << endl;
		cin >> x;
		cout << "Введите изменение X" << endl;
		cin >> deltaX;

		Table deltaTable(4, getColorFunction());

		HeaderTableRow header = HeaderTableRow("Морданенко Ян", authorName());
		StringTableRow functionInfo = StringTableRow({ "Имя функции:", functionName() }, true);
		StringTableRow dataNames = StringTableRow({ "x", "f(x)", "F(x)", "Невязка" }, true);

		deltaTable.AddRow(&header);
		deltaTable.AddRow(&functionInfo);
		deltaTable.AddRow(&dataNames);

		for (int i = 0; i < 10; i++)
		{
			double myResult = myFunction(x + deltaX * i, 0, EPSILON);
			double realResult = mathFunction(x + deltaX * i, 0);
			double residual = GetResidual(myResult, realResult);
			if (residual > 1000)
				continue;
			DoubleTableRow* row = new DoubleTableRow({ x + deltaX * i , myResult, realResult, residual });
			deltaTable.AddRow(row);
		}
		
		deltaTable.Print(console);

		cout << endl;

		cout << "Введите иделальный X" << endl;
		cin >> idealX;

		Table idealTable(4, getColorFunction());

		StringTableRow idealTableNames = StringTableRow({ "Эпсилон", "f(x)", "F(x)", "Невязка"}, true);

		idealTable.AddRow(&idealTableNames);

		for (double i = 0.1; i >= 0.0000001; i /= 10)
		{
			double myResult = myFunction(idealX, 0, i);
			double realResult = mathFunction(idealX, 0);
			double residual = GetResidual(myResult, realResult);

			DoubleTableRow* row = new DoubleTableRow({ i, myResult, realResult , residual });
			idealTable.AddRow(row);
		}
		idealTable.Print(console);
		cout << endl;
		FreeLibrary(library);
	}
	else
	{
		ThrowDllError("Необходимого DLL файла не обнаружено!");
		return 1;
	}
}
