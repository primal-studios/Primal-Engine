#ifndef quadtree_h__
#define quadtree_h__

#include "core/PrimalAssert.h"
#include "math/Vector2.h"

enum class EDirection2D
{
	NW,
	NE,
	SW,
	SE,
	UNKNOWN
};

class Comparable2D
{
	public:
		Comparable2D() = default;
		Comparable2D(const Comparable2D&) = default;
		Comparable2D(Comparable2D&&) noexcept = default;
		virtual ~Comparable2D() = default;

		Comparable2D& operator=(const Comparable2D&) = delete;
		Comparable2D& operator=(Comparable2D&&) noexcept = delete;

		virtual Vector2d getPosition() const = 0;
};

namespace detail
{
	inline EDirection2D DirectionTo(const Vector2d& aFrom, const Vector2d& aTo)
	{
		const auto toVector = aFrom - aTo;

		if (toVector.x >= 0.0 && toVector.y > 0.0) return EDirection2D::NE;
		if (toVector.x > 0.0 && toVector.y <= 0.0) return EDirection2D::SE;
		if (toVector.x < 0.0 && toVector.y >= 0.0) return EDirection2D::NW;
		if (toVector.x <= 0.0 && toVector.y < 0.0) return EDirection2D::SW;
		return EDirection2D::UNKNOWN;
	}

	inline EDirection2D DirectionTo(Comparable2D& aFrom, Comparable2D& aTo)
	{
		return DirectionTo(aFrom, aTo);
	}
} // detail

template<typename T>
class Quadtree final
{
	struct AbstractNode
	{
		explicit AbstractNode(bool aIsInternal)
			: isInternal(aIsInternal)
		{}

		virtual ~AbstractNode() = default;

		bool isInternal;
		Vector2d center;
		Vector2d halfBounds;
	};

	struct InternalNode final : AbstractNode
	{
		InternalNode() : AbstractNode(true) {}
		~InternalNode() override;

		AbstractNode* nw = nullptr;
		AbstractNode* ne = nullptr;
		AbstractNode* sw = nullptr;
		AbstractNode* se = nullptr;
	};

	struct LeafNode final : AbstractNode
	{
		LeafNode() : AbstractNode(false) {}
		~LeafNode() override;

		InternalNode* parent = nullptr;
		T* value = nullptr;;
	};
public:
	Quadtree() = delete;

	Quadtree(Vector2d aCenter, Vector2d aHalfBounds);

	Quadtree(const Quadtree&) = delete;

	Quadtree(Quadtree&&) noexcept;

	~Quadtree();

	Quadtree& operator=(const Quadtree&) = delete;

	Quadtree& operator=(Quadtree&&) noexcept;

	bool insert(const T& aValue);
private:
	bool _insert(T* aValue, AbstractNode** aNode);
	void _destroy(AbstractNode* aRoot);

	AbstractNode* mRoot;
	Vector2d mCenter;
	Vector2d mHalfBounds;
};

template <typename T>
Quadtree<T>::InternalNode::~InternalNode()
{
	if (nw) delete nw;
	if (ne) delete ne;
	if (sw) delete sw;
	if (se) delete se;
}

template <typename T>
Quadtree<T>::LeafNode::~LeafNode()
{
	if (value) delete value;
}

template <typename T>
Quadtree<T>::Quadtree(Vector2d aCenter, Vector2d aHalfBounds)
	: mRoot(nullptr), mCenter(aCenter), mHalfBounds(aHalfBounds)
{
	static_assert(std::is_same<Comparable2D, T>::value, "Type cannot be Comparable2D abstract class.");
	static_assert(std::is_base_of<Comparable2D, T>::value, "Type does not derive from Comparable2D.");
}

template <typename T>
inline Quadtree<T>::Quadtree(Quadtree&& aOther) noexcept
{
	mRoot = aOther.mRoot;
	aOther.mRoot = nullptr;
}

template <typename T>
Quadtree<T>::~Quadtree()
{
	_destroy(mRoot);
	mRoot = nullptr;
}

template <typename T>
Quadtree<T>& Quadtree<T>::operator=(Quadtree&& aOther) noexcept
{
	mRoot = aOther.mRoot;
	aOther.mRoot = nullptr;
}

template <typename T>
bool Quadtree<T>::insert(const T& aValue)
{
	return _insert(new T(aValue), &mRoot);
}

template <typename T>
bool Quadtree<T>::_insert(T* aValue, AbstractNode** aNode)
{
	if (aNode == nullptr) // case should only happen on root node
	{
		LeafNode* node = new LeafNode();
		node->value = aValue;
		node->center = mCenter;
		node->halfBounds = mHalfBounds;

		*aNode = node;
	}
	const auto valuePos = aValue->getPosition();
	const auto nodePos = (*aNode)->center;

	const EDirection2D dir = detail::DirectionTo(nodePos, valuePos);

	return false; 
}

template <typename T>
void Quadtree<T>::_destroy(AbstractNode* aRoot)
{
	if (aRoot) delete aRoot;
}

#endif // quadtree_h__
