#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class BinaryNodeValue
{
	public: virtual int GetCompareNumber() = 0;

	public: virtual void OnRepeatAdd() = 0;

	public: virtual string ToString() = 0;
};

class CharInfo : BinaryNodeValue
{
	public: int Char;

	public: int Count;

	public: CharInfo()
	{
		Char = 0;
		Count = 0;
	}

	public: CharInfo(int c, int count = 1)
	{
		Char = c;
		Count = count;
	}

	public: int GetCompareNumber()
	{
		return Char;
	}

	public: void OnRepeatAdd()
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

	public: BinaryNode(TValue value)
	{
		Value = value;
		LeftNode = NULL;
		RightNode = NULL;
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
		int nodeValueNumber = Value.GetCompareNumber();
		int argsValueNumber = value.GetCompareNumber();

		if (nodeValueNumber == argsValueNumber)
		{
			Value.OnRepeatAdd();
			return;
		}

		if (argsValueNumber < nodeValueNumber)
		{
			if (LeftNode != NULL)
				LeftNode->Add(value);
			else
			{
				BinaryNode* newTree = new BinaryNode(value);
				LeftNode = newTree;
			}
		}
		else
		{
			if (RightNode != NULL)
				RightNode->Add(value);
			else
			{
				BinaryNode* newTree = new BinaryNode(value);
				RightNode = newTree;
			}
		}
	}

	public: BinaryNode<TValue>* Find(TValue value, int* steps)
	{
		int nodeValueNumber = Value.GetCompareNumber();
		int argsValueNumber = value.GetCompareNumber();

		++*steps;
		if (nodeValueNumber == argsValueNumber)
			return this;

		if (argsValueNumber < nodeValueNumber && LeftNode != NULL)
			return LeftNode->Find(value, steps);

		if (argsValueNumber > nodeValueNumber && RightNode != NULL)
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

bool Compare(CharInfo& charInfo1, CharInfo& charInfo2)
{
	return charInfo1.Char > charInfo2.Char;
}


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
	vector<CharInfo> array = vector<CharInfo>();
	for (int i = 0; i < text.length(); i++)
	{
		bool arrayHasCharInfo = false;
		CharInfo newCharInfo = CharInfo(text[i]);
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

	sort(array.begin(), array.end(), [](CharInfo& charInfo1, CharInfo& charInfo2) {return charInfo1.Char > charInfo2.Char; });

	int size = array.size();
	int first = 0;
	int last = size - 1;


	for (int i = size / 2; i < size;)
	{
		++*stepsCount;
		if (array[i].Char == c)
		{
			return array[i].Count;
		}
		else if (array[i].Char > c)
		{
			i += (size - i) / 2 ;
		}
		else
		{
			i -= (size - i) / 2;
		}
	}
	return -1;
}

int main()
{
	string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam";
	int length = text.length();
	CharInfo preLastLetterInfo = CharInfo(text[text.length() - 2]);

	int binaryTreeSteps = 0;
	int consistentSearchSteps = 0;
	int binarySearchSteps = 0;

	int binaryTreeCount = 0;
	int consistentSearchCount = 0;
	int binarySearchCount = 0;

	// Поиск в бинарном дереве
	BinaryNode<CharInfo> mainNode = BinaryNode<CharInfo>(100);
	for (int i = 0; i < length; i++)
	{
		CharInfo charInfo = CharInfo(text[i]);
		mainNode.Add(charInfo);

	}
	BinaryNode<CharInfo>* founded = mainNode.Find(preLastLetterInfo, &binaryTreeSteps);
	if (founded != NULL)
		binaryTreeCount = founded->Value.Count;

	// Последовательный поиск
	consistentSearchCount = GetCharCountConsistently(text, preLastLetterInfo.Char, &consistentSearchSteps);


	for (int i = 0; i < text.length(); i++) {
		cout << text[i] << " " << i << endl;
		cout << GetCharCountBinary(text, text[i], &binarySearchSteps) << endl;

	}
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