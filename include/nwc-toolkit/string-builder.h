// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_STRING_BUILDER_H_
#define NWC_TOOLKIT_STRING_BUILDER_H_

#include "string.h"

namespace nwc_toolkit {

class StringBuilder
{
public:
	StringBuilder() : initial_buf_(), buf_(initial_buf_), length_(0),
		size_(sizeof(initial_buf_)) { buf_[0] = '\0'; }
	~StringBuilder() { if (buf_ != initial_buf_) delete [] buf_; }

	void clear();

	char &operator[](std::size_t index);
	const char &operator[](std::size_t index) const;

	char *buf() { return buf_; }
	const char *buf() const { return buf_; }
	std::size_t length() const { return length_; }
	std::size_t size() const { return size_; }

	const char *c_str() const { return buf_; }
	const char *data() const { return buf_; }

	String str() const { return String(buf_, length_); }

	StringBuilder &operator=(const char *str) { return assign(str); }
	StringBuilder &operator=(const String &str) { return assign(str); }

	StringBuilder &assign(const char *str) { return assign(String(str)); }
	StringBuilder &assign(const char *str, std::size_t length)
	{ return assign(String(str, length)); }
	StringBuilder &assign(const String &str);

	StringBuilder &append(char byte) { return append(byte, KeepAsIs()); }
	StringBuilder &append(const char *str) { return append(str, KeepAsIs()); }
	StringBuilder &append(const char *str, std::size_t length)
	{ return append(str, length, KeepAsIs()); }
	StringBuilder &append(const String &str) { return append(str, KeepAsIs()); }

	template <typename T>
	StringBuilder &append(char byte, T filter);
	template <typename T>
	StringBuilder &append(const char *str, T filter)
	{ return append(String(str), filter); }
	template <typename T>
	StringBuilder &append(const char *str, std::size_t length, T filter)
	{ return append(String(str, length), filter); }
	template <typename T>
	StringBuilder &append(const String &str, T filter);

	StringBuilder &resize(std::size_t length);

private:
	enum { INITIAL_BUF_SIZE = sizeof(char *) };

	char initial_buf_[INITIAL_BUF_SIZE];
	char *buf_;
	std::size_t length_;
	std::size_t size_;

	void resizeBuf(std::size_t size);

	// Disallows copies.
	StringBuilder(const StringBuilder &);
	StringBuilder &operator=(const StringBuilder &);
};

inline void StringBuilder::clear()
{
	buf_[0] = '\0';
	length_ = 0;
}

inline char &StringBuilder::operator[](std::size_t index)
{
	CHESTER_ASSERT(index <= length_);

	return buf_[index];
}

inline const char &StringBuilder::operator[](std::size_t index) const
{
	CHESTER_ASSERT(index <= length_);

	return buf_[index];
}

inline StringBuilder &StringBuilder::assign(const String &str)
{
	clear();
	append(str);
	return *this;
}

template <typename T>
inline StringBuilder &StringBuilder::append(char byte, T filter)
{
	if (length_ + 1 >= size_)
		resizeBuf(length_ + 2);

	buf_[length_] = filter(byte);
	buf_[++length_] = '\0';
	return *this;
}

template <typename T>
inline StringBuilder &StringBuilder::append(const String &str, T filter)
{
	if (length_ + str.length() >= size_)
		resizeBuf(length_ + str.length() + 1);

	for (std::size_t i = 0; i < str.length(); ++i)
		buf_[length_++] = filter(str[i]);
	buf_[length_] = '\0';
	return *this;
}

inline StringBuilder &StringBuilder::resize(std::size_t length)
{
	if (length >= size_)
		resizeBuf(length + 1);

	length_ = length;
	buf_[length_] = '\0';
	return *this;
}

inline void StringBuilder::resizeBuf(std::size_t size)
{
	std::size_t new_buf_size = size_;
	while (new_buf_size < size)
		new_buf_size *= 2;

	char *new_buf = new char[new_buf_size];
	for (std::size_t i = 0; i < length_; ++i)
		new_buf[i] = buf_[i];
	if (buf_ != initial_buf_)
		delete [] buf_;
	buf_ = new_buf;
	size_ = new_buf_size;
}

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_STRING_BUILDER_H_
