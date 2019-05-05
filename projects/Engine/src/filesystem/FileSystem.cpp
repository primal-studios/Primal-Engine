#include "filesystem/FileSystem.h"

#if defined(PRIMAL_PLATFORM_WINDOWS)
#include "filesystem/direntwin.h"
#include <direct.h>
#else
#include <dirent.h>
#endif

#include <fstream>
#include "core/Log.h"

FileSystem::FileSystem()
{
	mMountedPath = "";
}

FileSystem& FileSystem::instance()
{
	static FileSystem* instance = new FileSystem();
	return *instance;
}

void FileSystem::mount(const std::string& aPath)
{
	if(exists(aPath))
	{
		mMountedPath = aPath;
		PRIMAL_INTERNAL_INFO("Path mounted: {0}", aPath);
		return;
	}

	PRIMAL_INTERNAL_ERROR("Cannot mount path, directory not found: {0}", aPath);
}

void FileSystem::unmount()
{
	PRIMAL_INTERNAL_INFO("Path unmounted");
	mMountedPath = "";
}

File* FileSystem::load(const std::string& aPath) const
{
	const std::string loadPath = mMountedPath + "/" + aPath;

	if(exists(loadPath))
	{
		File* file = new File();
		const std::string data = loadToString(loadPath);

		file->mData = data;
		file->mPath = loadPath;

		return file;
	}

	PRIMAL_INTERNAL_ERROR("File does not exist at path: {0}", loadPath);

	return nullptr;
}

std::string FileSystem::loadToString(const std::string& aPath) const
{
	const std::string loadPath = mMountedPath + "/" + aPath;

	if(exists(loadPath))
	{
		std::ifstream stream(loadPath);
		std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

		return str;
	}

	PRIMAL_INTERNAL_ERROR("File does not exist at path: {0}", loadPath);

	return "";
}

bool FileSystem::exists(const std::string& aPath) const
{
	const std::string loadPath = mMountedPath + "/" + aPath;

	struct stat buffer;
	return (stat(loadPath.c_str(), &buffer) == 0);
}

File* FileSystem::create(const std::string& aPath) const
{
	const std::string loadPath = mMountedPath + "/" + aPath;

	if(exists(loadPath))
	{
		return load(loadPath);
	}

	File* file = new File();
	file->mPath = loadPath;

	return file;
}

void FileSystem::createDirectory(const std::string& aPath) const
{
	const std::string loadPath = mMountedPath + "/" + aPath;

	if(!exists(loadPath))
	{
		#if defined(PRIMAL_PLATFORM_WINDOWS)
				_mkdir(loadPath.c_str());
		#elif defined (PRIMAL_PLATFORM_LINUX)
				mkdir(loadPath.c_str(), 0777);
		#endif
	}
}

std::vector<FileInfo> FileSystem::getAllFilesInDirectory(const std::string& aPath, const bool aRecursive) const
{
	std::vector<FileInfo> fileList;
	std::string loadPath = mMountedPath + "/" + aPath;

	struct dirent* entry;
	DIR* dir = opendir(loadPath.c_str());

	if(dir == nullptr)
	{
		PRIMAL_INTERNAL_ERROR("Directory does not exist: {0}", loadPath);
		return fileList;
	}

	while((entry = readdir(dir)) != nullptr)
	{
		FileInfo info {};

		const std::string fullPath = loadPath + "\\" + entry->d_name;
		info.path = fullPath;

		info.name = entry->d_name;
		info.size = static_cast<size_t>(entry->d_reclen);

		fileList.push_back(info);
	}

	closedir(dir);

	if(aRecursive)
	{
		std::vector<FileInfo> children;
		for(const auto& item : fileList)
		{
			if(item.name.find('.') != std::string::npos)
			{
				// item is a directory
				auto childItems = getAllFilesInDirectory(item.path, aRecursive);

				for(const auto& child : childItems)
				{
					children.push_back(child);
				}
			}
		}

		for(const auto& child : children)
		{
			fileList.push_back(child);
		}
	}

	return fileList;
}







