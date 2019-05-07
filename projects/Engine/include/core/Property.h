#ifndef property_h__
#define property_h__

#include <functional>

namespace detail
{
	template<typename T>
	struct IsBaseType
	{
		static constexpr bool Value = std::is_floating_point<T>::value || std::is_integral<T>::value;
	};

	template<typename T>
	inline constexpr bool is_base_type_v = IsBaseType<T>::Value;

	struct No {};
	template<typename T, typename Arg> No operator+= (const T&, const Arg&);
	template<typename T, typename Arg> No operator-= (const T&, const Arg&);
	template<typename T, typename Arg> No operator*= (const T&, const Arg&);
	template<typename T, typename Arg> No operator/= (const T&, const Arg&);
	template<typename T, typename Arg> No operator+ (const T&, const Arg&);
	template<typename T, typename Arg> No operator- (const T&, const Arg&);
	template<typename T, typename Arg> No operator* (const T&, const Arg&);
	template<typename T, typename Arg> No operator/ (const T&, const Arg&);

	template<typename T, typename Arg = T>
	struct PlusEqualsExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) += *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool plus_equals_exists_v = PlusEqualsExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct MinusEqualsExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) -= *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool minus_equals_exists_v = MinusEqualsExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct TimesEqualsExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) *= *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool times_equals_exists_v = TimesEqualsExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct DivideEqualsExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) /= *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool divide_equals_exists_v = DivideEqualsExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct PlusExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) + *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool plus_exists_v = PlusExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct MinusExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) - *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool minus_exists_v = MinusExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct TimesExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) * *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool times_exists_v = TimesExists<T, Arg>::Value;

	template<typename T, typename Arg = T>
	struct DivideExists
	{
		static constexpr bool Value = !std::is_same<decltype(*static_cast<T*>(nullptr) / *static_cast<Arg*>(nullptr)), No>::value;
	};

	template<typename T, typename Arg = T>
	inline constexpr bool divide_exists_v = DivideExists<T, Arg>::Value;
}

template<typename T>
class Property
{
	public:
		Property(T aValue)
		{
			mValue = aValue;
		}

		void setCallback(const std::function<T(T)>& aCallback)
		{
			mSetCallback = aCallback;
		}

		void getCallback(const std::function<T(T&)>& aCallback) 
		{
			mGetCallback = aCallback;
		}

		T operator * ()
		{
			if(mGetCallback)
				return mGetCallback(mValue);

			return mValue;
		}

		T operator = (const T& aValue)
		{
			mValue = aValue;

			if(mSetCallback)
				mValue = mSetCallback(mValue);

			return mValue;
		}

		// Operators
		template<typename = std::enable_if_t<detail::plus_equals_exists_v<T> || detail::is_base_type_v<T>>>
		T operator+=(const T & aValue)
		{
			mValue += aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return aValue;
		}

		template<typename = std::enable_if_t<detail::minus_equals_exists_v<T> || detail::is_base_type_v<T>>>
		T operator-=(const T & aValue)
		{
			mValue -= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return aValue;
		}

		template<typename = std::enable_if_t<detail::times_equals_exists_v<T> || detail::is_base_type_v<T>>>
		T operator*=(const T & aValue)
		{
			mValue *= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return aValue;
		}

		template<typename = std::enable_if_t<detail::divide_equals_exists_v<T> || detail::is_base_type_v<T>>>
		T operator/=(const T & aValue)
		{
			mValue /= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::plus_equals_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator+=(const Other& aValue)
		{
			mValue += aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::minus_equals_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator-=(const Other& aValue)
		{
			mValue -= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return mValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::times_equals_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator*=(const Other& aValue)
		{
			mValue *= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return mValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::divide_equals_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator/=(const Other& aValue)
		{
			mValue /= aValue;

			if (mSetCallback)
				mValue = mSetCallback(mValue);

			return mValue;
		}

		template<typename = std::enable_if_t<detail::plus_exists_v<T> || detail::is_base_type_v<T>>>
		T operator+(const T & aValue)
		{
			return mValue + aValue;
		}

		template<typename = std::enable_if_t<detail::minus_exists_v<T> || detail::is_base_type_v<T>>>
		T operator-(const T & aValue)
		{
			return mValue - aValue;
		}

		template<typename = std::enable_if_t<detail::times_exists_v<T> || detail::is_base_type_v<T>>>
		T operator*(const T & aValue)
		{
			return mValue * aValue;
		}

		template<typename = std::enable_if_t<detail::divide_exists_v<T> || detail::is_base_type_v<T>>>
		T operator/(const T & aValue)
		{
			return mValue / aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::plus_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator+(const Other& aValue)
		{
			return mValue + aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::minus_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator-(const Other& aValue)
		{
			return mValue - aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::times_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator*(const Other& aValue)
		{
			return mValue * aValue;
		}

		template<typename Other, typename = std::enable_if_t<detail::divide_exists_v<T, Other> || detail::is_base_type_v<T>>>
		T operator/(const Other& aValue)
		{
			return mValue / aValue;
		}

	private:
		T mValue;

		std::function<T(T)> mSetCallback;
		std::function<T(T&)> mGetCallback;
};

#endif // property_h__