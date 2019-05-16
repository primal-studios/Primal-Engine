#ifndef attachmentoperations_h__
#define attachmentoperations_h__

#include <cstdint>

enum class EAttachmentLoadOperation : uint32_t
{
	LOAD		= 0,
	CLEAR		= 1,
	DONT_CARE	= 2
};

enum class EAttachmentStoreOperation : uint32_t
{
	STORE		= 0,
	DONT_CARE	= 1
};

#endif // attachmentoperations_h__