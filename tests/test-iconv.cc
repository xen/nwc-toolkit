#include <iconv.h>

#include <iostream>

namespace {

bool TestEncoding(const char *encoding)
{
	::iconv_t id = ::iconv_open(encoding, encoding);
	if (id == (::iconv_t)-1)
	{
		std::cout << "iconv: " << encoding << ": unsupported" << std::endl;
		return false;
	}
	std::cout << "iconv: " << encoding << ": ok" << std::endl;
	::iconv_close(id);
	return true;
}

}  // namespace

int main()
{
	bool all_ok = true;

	all_ok &= TestEncoding("SHIFT_JIS");
	all_ok &= TestEncoding("CP932");
	all_ok &= TestEncoding("EUC-JP");
	all_ok &= TestEncoding("EUC-JP-MS");
	all_ok &= TestEncoding("ISO-2022-JP");
	all_ok &= TestEncoding("ISO-2022-JP-2");
	all_ok &= TestEncoding("UTF-8");

	return all_ok ? 0 : 1;
}
