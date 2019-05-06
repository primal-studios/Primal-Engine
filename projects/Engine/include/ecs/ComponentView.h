#ifndef componentview_h__
#define componentview_h__

#include <vector>

#include "ecs/Component.h"

template<typename T>
class ComponentIterator
{
	public:
		ComponentIterator(std::vector<Component*>& aData, const size_t aIndex)
			: mData(aData)
		{
			mIndex = aIndex;
		}

		ComponentIterator& operator++ ()
		{
			++mIndex;

			return *this;
		}

		ComponentIterator operator++ (int)
		{
			ComponentIterator<T> copy = *this;
			mIndex++;

			return copy;
		}

		ComponentIterator& operator-- ()
		{
			++mIndex;

			return *this;
		}

		ComponentIterator operator-- (int)
		{
			ComponentIterator<T> copy = *this;
			mIndex--;

			return copy;
		}

		T* operator* ()
		{
			return reinterpret_cast<T*>(mData[mIndex]);
		}

		const T* operator* () const
		{
			return reinterpret_cast<T*>(mData[mIndex]);
		}

		bool operator== (const ComponentIterator& aOther) const noexcept
		{
			return mData.data() == aOther.mData.data() && mIndex == aOther.mIndex;
		}

		bool operator != (const ComponentIterator& aOther) const noexcept
		{
			return mIndex != aOther.mIndex || mData.data() != aOther.mData.data();
		}

	private:
		std::vector<Component*>& mData;
		size_t mIndex;
};

template<typename T>
class ComponentView
{
	public:
		explicit ComponentView(std::vector<Component*>& aData)
			: mData(aData)
		{

		}

		ComponentIterator<T> begin()
		{
			return ComponentIterator<T>(mData, 0);
		}

		ComponentIterator<T> end()
		{
			return ComponentIterator<T>(mData, mData.size());
		}

		T* operator[] (const size_t aIndex)
		{
			return reinterpret_cast<T*>(mData[aIndex]);
		}

		ComponentIterator<T> begin() const
		{
			return ComponentIterator<T>(mData, 0);
		}

		ComponentIterator<T> end() const
		{
			return ComponentIterator<T>(mData, mData.size());
		}

		const T* operator[] (const size_t aIndex) const
		{
			return reinterpret_cast<T*>(mData[aIndex]);
		}

		size_t size() const noexcept
		{
			return mData.size();
		}

	private:
		std::vector<Component*>& mData;
};

#endif // componentview_h__