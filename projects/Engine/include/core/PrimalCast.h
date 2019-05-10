#ifndef primalcast_h__
#define primalcast_h__

template<typename T, typename T2>
T primal_cast(T2 aOther)
{
#if defined(_DEBUG)
	return dynamic_cast<T>(aOther);
#endif
	return reinterpret_cast<T>(aOther);
}

#endif // primalcast_h__