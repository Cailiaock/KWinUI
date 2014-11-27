#include "registry_value.h"
#include <iostream>

typedef registry_string<double> regdouble;
typedef registry_binary<RECT> regrect;
typedef registry_int<bool> regbool;
typedef registry_binary<POINT> regpoint;
typedef registry_string<std::string> regstring;

int main()
{
	regdouble d("Software\\RegValueTest\\double", HKEY_CURRENT_USER);
	regrect r("Software\\RegValueTest\\rect", HKEY_CURRENT_USER);
	regbool b("Software\\RegValueTest\\bool", HKEY_CURRENT_USER);
	regpoint p("Software\\RegValueTest\\point", HKEY_CURRENT_USER);
	regstring s("Software\\RegValueTest\\string", HKEY_CURRENT_USER);

	RECT testrect = {2,3,4,5};
	POINT testpoint = {99,77};

	r = testrect;
	d = 98.888;
	b = true;
	p = testpoint;
	s = "hejsan hoppsan";
	
	bool bb = b;
	double dd = d;
	RECT rr = r;
	POINT pp = p;
	std::string ss = s;

	std::cout << bb << std::endl;
	std::cout << dd << std::endl;
	std::cout << rr.left << " " << rr.top << " "<< rr.right << " "<< rr.bottom << std::endl;
	std::cout << pp.x << " " << pp.y << std::endl;
	std::cout << ss << std::endl;

	r.remove_value();
	d.remove_value();
	b.remove_value();
	p.remove_value();
	s.remove_value();

	s.remove_key();

	return 0;
}
