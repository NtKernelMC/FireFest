#pragma once
#define INVALID_ELEMENT_ID 0xFFFFFFFF
template <typename T, unsigned long INITIAL_MAX_STACK_SIZE>
class CStack
{
public:
	CStack()
	{
		m_ulCapacity = 0;
		ExpandBy(INITIAL_MAX_STACK_SIZE - 1);
	}

	unsigned long GetCapacity() const { return m_ulCapacity; }

	unsigned long GetUnusedAmount() const { return m_Queue.size(); }

	void ExpandBy(unsigned long ulAmount)
	{
		const unsigned long ulOldSize = m_ulCapacity;
		const unsigned long ulNewSize = m_ulCapacity + ulAmount;

		// Add ID's for new items
		for (T ID = ulOldSize + 1; ID <= ulNewSize; ++ID)
		{
			m_Queue.push_front(ID);
		}
		m_ulCapacity = ulNewSize;
	}

	bool Pop(T& dest)
	{
		// Got any items? Pop from the back
		if (m_Queue.size() > 0)
		{
			T ID = m_Queue.back();
			dest = ID;
			m_Queue.pop_back();
			return true;
		}

		// No IDs left
		return false;
	}

	void Push(T ID)
	{
		assert(m_Queue.size() < m_ulCapacity);
		// Push to the front
		m_Queue.push_front(ID);
	}

private:
	unsigned long m_ulCapacity;
	std::deque<T> m_Queue;
};

struct ElementID
{
public:
	ElementID(const unsigned int& value = INVALID_ELEMENT_ID) : m_value(value) {}
	ElementID& operator=(const unsigned int& value)
	{
		m_value = value;
		return *this;
	}
	bool       operator==(const ElementID& ID) const { return m_value == ID.m_value; }
	bool       operator!=(const ElementID& ID) const { return m_value != ID.m_value; }
	bool       operator>(const ElementID& ID) const { return m_value > ID.m_value; }
	bool       operator>=(const ElementID& ID) const { return m_value >= ID.m_value; }
	bool       operator<(const ElementID& ID) const { return m_value < ID.m_value; }
	bool       operator<=(const ElementID& ID) const { return m_value <= ID.m_value; }
	ElementID& operator+=(const ElementID& ID)
	{
		m_value += ID.m_value;
		return *this;
	}
	ElementID& operator-=(const ElementID& ID)
	{
		m_value += ID.m_value;
		return *this;
	}
	ElementID operator+(const ElementID& ID) const { return m_value + ID.m_value; }
	ElementID operator-(const ElementID& ID) const { return m_value - ID.m_value; }
	ElementID operator++(int)
	{
		ElementID ret(m_value);
		++m_value;
		return ret;
	}
	ElementID& operator++()
	{
		++m_value;
		return *this;
	}
	unsigned int& Value() { return m_value; }
	const unsigned int& Value() const { return m_value; }

private:
	unsigned int m_value;
};
class CClientEntity;
// Allow 2^17 server elements and 2^17 client elements
#define MAX_SERVER_ELEMENTS 131072
#define MAX_CLIENT_ELEMENTS 131072
template <class T, int SIZE>
struct SFixedArray
{
	T& operator[](uint uiIndex)
	{
		assert(uiIndex < SIZE);
		return data[uiIndex];
	}

	const T& operator[](uint uiIndex) const
	{
		assert(uiIndex < SIZE);
		return data[uiIndex];
	}

	T data[SIZE];
};
class CElementIDs
{
public:
	static void Initialize();

	static CClientEntity* GetElement(ElementID ID);
	static void           SetElement(ElementID ID, CClientEntity* pEntity);

	static ElementID PopClientID();
	static void      PushClientID(ElementID ID);

private:
	static SFixedArray<CClientEntity*, MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS> m_Elements;
	static CStack<ElementID, MAX_CLIENT_ELEMENTS - 2>                             m_ClientStack;
};