#ifndef file_h__
#define file_h__

#include <string>

#include "core/Types.h"

enum class EFileSaveMode : uint32_t
{
	Truncate,
	Append
};

class File
{
	friend class FileSystem;
	public:
		File();
		~File();

		void write(const char* aData);
		void write(const std::string& aData);

		void save(const EFileSaveMode& aMode = EFileSaveMode::Truncate) const;

		size_t size() const;
		std::string data() const;

	private:
		Path mPath;
		std::string mData;
};

#endif // file_h__