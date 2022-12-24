
#pragma once

#include <any>
#include <unordered_map>

template <class T>
class Event
{
public:
	void SetData(T value)
	{
		data = value;
	}

	template<typename T>
	T GetData()
	{
		return data;
	}

private:
	T data;
};
