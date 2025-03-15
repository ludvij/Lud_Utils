#ifndef LUD_CL_HEADER
#define LUD_CL_HEADER

#include <string>
#include <version>

// WORK IN PROGRESS

namespace Lud
{

class Cl
{
public:
	Cl(std::string_view title, char delim, std::string_view version);
	void ShowHelp();
	void ShowTitle();

private:
	std::string m_title;
	char m_delim;
	std::string m_version;
};
}




#ifdef LUD_CL_IMPLEMENTATION

Lud::Cl::Cl(std::string_view title, char delim, std::string_view version)
	: m_title(title)
	, m_delim(delim)
	, m_version(version)
{
}


#endif//LUD_CL_IMPLEMENTATION
#endif//LUD_CL_HEADER