#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class BinaryNodeValue
{
	public: virtual void OnInserionRepeat() = 0;

	public: virtual string ToString() = 0;
};

class CharInfoNodeValue : BinaryNodeValue
{
	public: int Char;

	public: int Count;

	public: CharInfoNodeValue()
	{
		Char = 0;
		Count = 0;
	}

	public: CharInfoNodeValue(int c, int count = 1)
	{
		Char = c;
		Count = count;
	}

	public: void OnInserionRepeat()
	{
		Count++;
	}

	public: string ToString()
	{
		return to_string(Char);
	}
};

template
<
	typename TValue,
	typename = enable_if_t<is_base_of_v<BinaryNodeValue, TValue>>
>
class BinaryNode
{
	public: BinaryNode* LeftNode;

	public: BinaryNode* RightNode;

	public: TValue Value;

	private: int (*_compare)(TValue, TValue);

	public: BinaryNode(TValue value, int (*compare)(TValue, TValue))
	{
		Value = value;
		LeftNode = NULL;
		RightNode = NULL;
		_compare = compare;
	}

	public: ~BinaryNode()
	{
		if (LeftNode != NULL)
			delete LeftNode;
		if (RightNode != NULL)
			delete RightNode;
	}

	public: void Add(TValue value)
	{
		int compareResult = _compare(Value, value);

		if (compareResult == 0)
		{
			Value.OnInserionRepeat();
			return;
		}

		if (compareResult < 0)
		{
			if (LeftNode != NULL)
				LeftNode->Add(value);
			else
			{
				BinaryNode* newTree = new BinaryNode(value, _compare);
				LeftNode = newTree;
			}
		}
		else
		{
			if (RightNode != NULL)
				RightNode->Add(value);
			else
			{
				BinaryNode* newTree = new BinaryNode(value, _compare);
				RightNode = newTree;
			}
		}
	}

	public: BinaryNode<TValue>* Find(TValue value, int* steps)
	{
		int compareResult = _compare(Value, value);

		++*steps;
		if (compareResult == 0)
			return this;

		if (compareResult < 0 && LeftNode != NULL)
			return LeftNode->Find(value, steps);

		if (compareResult >  0 && RightNode != NULL)
			return RightNode->Find(value, steps);

		return NULL;
	}

	// Прямой обход - NLR
	public: void PreOrderTraversal()
	{
		Print();
		if (LeftNode != NULL)
			LeftNode->PreOrderTraversal();
		if (RightNode != NULL)
			RightNode->PreOrderTraversal();
	}

	// Центрированный обход - LNR
	public: void InOrderTraversal()
	{
		if (LeftNode != NULL)
			LeftNode->InOrderTraversal();
		Print();
		if (RightNode != NULL)
			RightNode->InOrderTraversal();
	}

	// Обратный обход - LRN
	public: void PostOrderTraversal()
	{
		if (LeftNode != NULL)
			LeftNode->PostOrderTraversal();
		if (RightNode != NULL)
			RightNode->PostOrderTraversal();
		Print();
	}

	public: void Print()
	{
		cout << Value.ToString() << " ";
	}
};


int GetCharCountConsistently(string text, char c, int* stepsCount) 
{
	int result = 0;
	for (int i = 0; i < text.length(); i++)
	{
		++*stepsCount;
		if (text[i] == c)
			result++;
	}
	return result;
}

int GetCharCountBinary(string text, char c, int* stepsCount)
{
	vector<CharInfoNodeValue> array = vector<CharInfoNodeValue>();
	for (int i = 0; i < text.length(); i++)
	{
		bool arrayHasCharInfo = false;
		CharInfoNodeValue newCharInfo = CharInfoNodeValue(text[i]);
		for (int j = 0; j < array.size(); j++)
			if (array[j].Char == newCharInfo.Char)
			{
				array[j].Count++;
				arrayHasCharInfo = true;
				break;
			}
		if (!arrayHasCharInfo)
			array.push_back(newCharInfo);
	}

	sort(array.begin(), array.end(), [](CharInfoNodeValue& charInfo1, CharInfoNodeValue& charInfo2) {return charInfo1.Char < charInfo2.Char; });

	int size = array.size();
	int first = 0;
	int last = size - 1;

	while (last >= first)
	{
		++*stepsCount;
		int current = first + (last - first) / 2;

		if (array[current].Char == c)
		{
			return array[current].Count;
		}
		else if (array[current].Char > c)
		{
			last = current - 1;
		}
		else
		{
			first = current + 1;
		}

	}
	return -1;
}

int Compare(CharInfoNodeValue charInfo1, CharInfoNodeValue charInfo2)
{
	if (charInfo1.Char == charInfo2.Char)
		return 0;
	else if (charInfo1.Char < charInfo2.Char)
		return 1;
	else
		return -1;
}

int main()
{
	string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam";
	int length = text.length();
	CharInfoNodeValue preLastLetterInfo = CharInfoNodeValue(text[text.length() - 2]);

	int binaryTreeSteps = 0;
	int consistentSearchSteps = 0;
	int binarySearchSteps = 0;

	int binaryTreeCount = 0;
	int consistentSearchCount = 0;
	int binarySearchCount = 0;

	// Поиск в бинарном дереве
	BinaryNode<CharInfoNodeValue> mainNode = BinaryNode<CharInfoNodeValue>(CharInfoNodeValue(100), Compare);
	for (int i = 0; i < length; i++)
	{
		CharInfoNodeValue charInfo = CharInfoNodeValue(text[i]);
		mainNode.Add(charInfo);

	}
	BinaryNode<CharInfoNodeValue>* founded = mainNode.Find(preLastLetterInfo, &binaryTreeSteps);
	if (founded != NULL)
		binaryTreeCount = founded->Value.Count;

	// Последовательный поиск
	consistentSearchCount = GetCharCountConsistently(text, preLastLetterInfo.Char, &consistentSearchSteps);

	// Бинарный поиск
	binarySearchCount = GetCharCountBinary(text, preLastLetterInfo.Char, &binarySearchSteps);

	cout << "Given text: " << text << endl;
	cout << "Searching for \"" << (char)preLastLetterInfo.Char << "\" (" << preLastLetterInfo.Char << ")" << endl;
	cout << "Binary Tree Search: " << binaryTreeCount << ", Steps: " << binaryTreeSteps << endl;
	cout << "Consistent Search: " << consistentSearchCount << ", Steps: " << consistentSearchSteps << endl;
	cout << "Binary Search: " << binarySearchCount << ", Steps: " << binarySearchSteps << endl;

	cout << "Tree traversals:" << endl;
	cout << "Pre-order traversal: " << endl;
	mainNode.PreOrderTraversal();
	cout << endl;

	cout << "Post-order traversal: " << endl;
	mainNode.PostOrderTraversal();
	cout << endl;

	cout << "In-order traversal: " << endl;
	mainNode.InOrderTraversal();
	cout << endl;

}