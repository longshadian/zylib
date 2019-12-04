#include <algorithm>
#include <iostream>
#include <iterator>

#include "zylibsoft/string/string_tokeniser.h"

int main()
{
    stlsoft::string_tokeniser<std::string, std::string> st{"a;fsdf;ab;", ";"};
    /*
    for (const auto& it : st) {
        std::cout << it << "\t";
    }
    */
    std::copy(st.begin(), st.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    system("pause");

    return 0;
}
