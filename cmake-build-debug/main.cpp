#include <iostream>
#include <string>
int main()
{
	int test1,test2,test3;
	test1 = 3;
	test2 = 0;
	test3 = 0;
	switch(test1){
		case 1:
			test2 = 0;
			break;
		case 5:
			test2 = 0;
		case 3:
			test2 = 7;
	}
	switch(test1){
		case 1:
			test3 = 0;
			break;
		case 3:
			test3 = 7;
		case 5:
			test3 = 5;
		case 8:
			test3 = 9;
	}
	return 0;
}