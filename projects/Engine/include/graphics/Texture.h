#ifndef texture_h__
#define texture_h__

#include <cstdint>
#include <vector>

enum class ETextureInternalFormat : uint32_t
{
	RGBA_8 = 0x8058,
	UNKNOWN = 0
};

enum class ETextureFormat : uint32_t
{
	RED = 0x1903,
	RG = 0x8227,
	RGB = 0x1907,
	BGR = 0x80E0,
	RGBA = 0x1908,
	BGRA = 0x80E1,
	DEPTH = 0x1902,
	STENCIL = 0x1901,
	UNKNOWN = 0
};

enum class ETextureDataType : uint32_t
{
	UBYTE = 0x1401,
	BYTE = 0x1400,
	USHORT = 0x1403,
	SHORT = 0x1402,
	UINT = 0x1405,
	INT = 0x1404,
	FLOAT = 0x1406,
	UNKNOWN = 0
};

class Texture
{
	public:
		Texture();
		Texture(const Texture&) = delete;
		Texture(Texture&&) noexcept = delete;
		Texture(uint32_t aWidth, uint32_t aHeight, ETextureInternalFormat aInternal, ETextureFormat aFormat);
		~Texture();

		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&&) noexcept = delete;

		void setData(std::vector<void*> aData, uint32_t aWidth, uint32_t aHeight, ETextureInternalFormat aInternal, ETextureFormat aFormat, ETextureDataType aDataType);
		void bind(uint32_t aLocation) const;

		[[nodiscard]] uint32_t getWidth() const;
		[[nodiscard]] uint32_t getHeight() const;
		[[nodiscard]] uint32_t getId() const;

		[[nodiscard]] ETextureFormat getFormat() const;
		
	private:
		uint32_t mId;
		uint32_t mWidth;
		uint32_t mHeight;

		ETextureFormat mFormat;
		ETextureInternalFormat mInternal;
};

#endif // texture_h__