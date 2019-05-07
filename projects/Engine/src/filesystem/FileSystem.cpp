#include "filesystem/FileSystem.h"

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

void FileSystem::mount(const Path& aPath)
{
	if(exists(aPath))
	{
		mMountedPath = aPath;
		mMountedPath += "/";

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

File* FileSystem::load(const Path& aPath) const
{
	if(exists(aPath))
	{
		File* file = new File();
		const std::string data = loadToString(aPath);

		file->mData = data;
		
		Path fullPath = mMountedPath;
		fullPath += aPath;

		file->mPath = fullPath;

		return file;
	}

	PRIMAL_INTERNAL_ERROR("File does not exist at path: {0}", aPath.string());

	return nullptr;
}

std::string FileSystem::loadToString(const Path& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	if(exists(loadPath))
	{
		std::ifstream stream(loadPath.string());
		std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

		return str;
	}

	PRIMAL_INTERNAL_ERROR("File does not exist at path: {0}", loadPath.string());

	return "";
}

bool FileSystem::exists(const Path& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	return std::filesystem::exists(loadPath);
}

bool FileSystem::isFile(const Path& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	return std::filesystem::is_regular_file(loadPath);
}

bool FileSystem::isDirectory(const Path& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	return std::filesystem::is_directory(loadPath);
}

File* FileSystem::create(const std::string& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	if(exists(loadPath))
	{
		return load(loadPath);
	}

	File* file = new File();
	file->mPath = loadPath;

	return file;
}

void FileSystem::createDirectory(const Path& aPath) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	if(!exists(loadPath))
	{
		std::filesystem::create_directory(loadPath);
	}
}

std::vector<FileInfo> FileSystem::getFilesInPath(const Path& aPath, const bool aRecursive) const
{
	Path loadPath = mMountedPath;
	loadPath += aPath;

	std::vector<FileInfo> files;

	if (aRecursive)
	{
		for (const auto& p : std::filesystem::recursive_directory_iterator(loadPath))
		{
			if (p.is_directory())
				continue;

			const auto& path = p.path();
			files.push_back({ path.stem().string(), path.extension().string(), path.filename().string(), path.parent_path().string(), path.root_path().string() });
		}
	}
	else
	{
		for (const auto& p : std::filesystem::directory_iterator(loadPath))
		{
			if (p.is_directory())
				continue;

			const auto& path = p.path();
			files.push_back({ path.stem().string(), path.extension().string(), path.filename().string(), path.parent_path().string(), path.root_path().string() });
		}
	}

	return files;
}
