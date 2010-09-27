// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_POOL_H_
#define NWC_TOOLKIT_STRING_POOL_H_

#include "string.h"

namespace nwc_toolkit {

class StringPool
{
public:
	explicit StringPool(std::size_t chunk_size = 0);
	~StringPool();

	std::size_t chunk_size() const { return chunk_size_; }

	std::size_t num_strings() const { return num_strings_; }
	std::size_t total_length() const { return total_length_; }
	std::size_t total_size() const { return total_size_; }

	void clear();

	String append(const char *str) { return append(String(str)); }
	String append(const char *str, std::size_t length)
	{ return append(String(str, length)); }
	String append(const String &str);

private:
	enum { DEFAULT_CHUNK_SIZE = 4096 };

	std::size_t chunk_size_;
	std::size_t num_strings_;
	std::size_t total_length_;
	std::size_t total_size_;
	char **chunks_;
	std::size_t num_chunks_;
	std::size_t max_num_chunks_;
	char **custom_chunks_;
	std::size_t num_custom_chunks_;
	std::size_t max_num_custom_chunks_;
	std::size_t chunk_id_;
	char *ptr_;
	std::size_t avail_;

	void appendChunk();
	String appendCustomString(const String &str);

	// Disallows copies.
	StringPool(const StringPool &);
	StringPool &operator=(const StringPool &);
};

inline StringPool::StringPool(std::size_t chunk_size) : chunk_size_(chunk_size),
	num_strings_(0), total_length_(0), total_size_(0),
	chunks_(NULL), num_chunks_(0), max_num_chunks_(0),
	custom_chunks_(NULL), num_custom_chunks_(0), max_num_custom_chunks_(0),
	chunk_id_(0), ptr_(NULL), avail_(0)
{
	if (chunk_size_ == 0)
		chunk_size_ = DEFAULT_CHUNK_SIZE;
}

inline StringPool::~StringPool()
{
	clear();

	for (std::size_t i = 0; i < num_chunks_; ++i)
	{
		if (chunks_[i] != NULL)
		{
			delete [] chunks_[i];
			chunks_[i] = NULL;
		}
	}

	if (custom_chunks_ != NULL)
	{
		delete [] custom_chunks_;
		custom_chunks_ = NULL;
	}
}

inline void StringPool::clear()
{
	num_strings_ = 0;
	total_length_ = 0;
	total_size_ = num_chunks_ * chunk_size_;

	for (std::size_t i = 0; i < num_custom_chunks_; ++i)
	{
		if (custom_chunks_[i] != NULL)
		{
			delete [] custom_chunks_[i];
			custom_chunks_[i] = NULL;
		}
	}

	num_custom_chunks_ = 0;
	chunk_id_ = 0;
	ptr_ = NULL;
	avail_ = 0;
}

inline String StringPool::append(const String &str)
{
	if (str.length() >= avail_)
	{
		if (str.length() >= chunk_size_)
			return appendCustomString(str);

		appendChunk();
		total_size_ += chunk_size_;
	}

	String str_copy(ptr_, str.length());
	for (std::size_t i = 0; i < str.length(); ++i)
		*ptr_++ = str[i];
	*ptr_++ = '\0';
	avail_ -= str.length() + 1;

	++num_strings_;
	total_length_ += str.length();
	return str_copy;
}

inline String StringPool::appendCustomString(const String &str)
{
	if (num_custom_chunks_ == max_num_custom_chunks_)
	{
		std::size_t new_max_num_custom_chunks = (max_num_custom_chunks_ != 0)
			? (max_num_custom_chunks_ * 2) : 1;
		char **new_custom_chunks = new char *[new_max_num_custom_chunks];

		for (std::size_t i = 0; i < num_custom_chunks_; ++i)
			new_custom_chunks[i] = custom_chunks_[i];

		delete [] custom_chunks_;
		custom_chunks_ = new_custom_chunks;
		max_num_custom_chunks_ = new_max_num_custom_chunks;
	}

	char *new_custom_chunk = new char[str.length() + 1];
	custom_chunks_[num_custom_chunks_++] = new_custom_chunk;

	++num_strings_;
	total_length_ += str.length();
	total_size_ += str.length() + 1;

	for (std::size_t i = 0; i < str.length(); ++i)
		new_custom_chunk[i] = str[i];
	new_custom_chunk[str.length()] = '\0';

	return String(new_custom_chunk, str.length());
}

inline void StringPool::appendChunk()
{
	if (chunk_id_ == num_chunks_)
	{
		if (num_chunks_ == max_num_chunks_)
		{
			std::size_t new_max_num_chunks = (max_num_chunks_ != 0)
				? (max_num_chunks_ * 2) : 1;
			char **new_chunks = new char *[new_max_num_chunks];

			for (std::size_t i = 0; i < num_chunks_; ++i)
				new_chunks[i] = chunks_[i];

			delete [] chunks_;
			chunks_ = new_chunks;
			max_num_chunks_ = new_max_num_chunks;
		}

		char *new_chunk = new char[chunk_size_];
		chunks_[num_chunks_++] = new_chunk;
	}

	ptr_ = chunks_[chunk_id_++];
	avail_ = chunk_size_;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_POOL_H_
