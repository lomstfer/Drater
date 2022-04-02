#include "Ftint.hpp"

int ftint(float floater)
{
	int inter = int(floater + 0.5 - (floater < 0));
	return inter;
}