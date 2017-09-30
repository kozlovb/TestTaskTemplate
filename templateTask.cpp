#include <iostream>
#include <string.h>

#define DEFINE_Str_To_Find_and_Put_For_Type(char_type,str,str_rep) \
template<>\
const std::basic_string<char_type> ConstsTraits<char_type>::KStr_to_Find = str;\
template<>\
const std::basic_string<char_type> ConstsTraits<char_type>::KStr_to_Put = str_rep;

namespace {

template<class CharType>
struct ConstsTraits {
  static const std::basic_string<CharType> KStr_to_Find;
  static const std::basic_string<CharType> KStr_to_Put;
};
DEFINE_Str_To_Find_and_Put_For_Type(char,"Ab","AB")
DEFINE_Str_To_Find_and_Put_For_Type(wchar_t,L"Ab",L"AB")

template<class CharType>
struct DefaultFindSubstrPolicy {
  typedef std::basic_string<CharType> my_string;
  static size_t find_substr(const my_string& strToAnalyse,const my_string& strToFind,size_t pos)
  {//wraper of std::basic_string::find. Might be redefined to use KMP or other algos in different policies.
    return strToAnalyse.find(ConstsTraits<CharType>::KStr_to_Find,pos);
  } 
};

} //namespace

template<class CharType,template <class> class FindPolicy = DefaultFindSubstrPolicy>
class MyClass
{
public:
  MyClass(const CharType* strToAnayse) : _data(strToAnayse) {}

private:
  typedef std::basic_string<CharType> my_string;

public:
  const my_string& replaceAbWithAB() {
    for_each_substr(
      [this] (auto pos) { _data.replace(pos, ConstsTraits<CharType>::KStr_to_Put.size(), ConstsTraits<CharType>::KStr_to_Put);});
      return _data;
    }

  size_t getAbCount() const {   
    size_t counter = 0;
    for_each_substr(
      [&counter] (auto /*dummy*/){ ++counter;}
    );
    return counter;
  }

private:
	
  template<class Callable> 
  void for_each_substr(Callable callable) const {
    size_t pos = 0;
    do {
         pos = _data.find(ConstsTraits<CharType>::KStr_to_Find,pos);
         pos = FindPolicy<CharType>::find_substr(_data,ConstsTraits<CharType>::KStr_to_Find,pos);
         if(pos != my_string::npos) {
           callable(pos);
	   pos += ConstsTraits<CharType>::KStr_to_Find.size();
	 } 
       } while(pos != my_string::npos);
    return;	
  }
	
  my_string _data;
};

int main(int argc, char** argv) {

    const char *szTestString1 = "Ab aB AB aB Ab";
    const wchar_t *szTestString2 = L"Ab aB AB aB Ab";

    MyClass<char> obj_char(szTestString1);
    MyClass<wchar_t> obj_wchar(szTestString2);

    std::cout<< "Found "<< obj_char.getAbCount()<< " occurrences of Ab. New string: ";
    std::cout << obj_char.replaceAbWithAB()<<std::endl;

    std::cout<< "Found "<< obj_wchar.getAbCount()<< " occurrences of Ab. New string: ";
    std::wcout << obj_wchar.replaceAbWithAB()<<std::endl;

    return 0;
}
