#ifndef indexbuffer_h__
#define indexbuffer_h__

#include <cstdint>

class IndexBuffer
{
	public:
		IndexBuffer(uint32_t* aData, const uint32_t aCount);
		~IndexBuffer();

		void bind() const;
		void unbind() const;

		uint32_t getCount() const { return mCount; }

	private:
		uint32_t mId;
		uint32_t mCount;
};

#endif // indexbuffer_h__