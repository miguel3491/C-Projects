#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

// Addition for single precision floating-point numbers
float faddition(float x, float y);

// Subtraction for single precision floating-point numbers
float fsubtraction(float x, float y);

// Multiplication single precision floating-point numbers
float fmultiplication(float x, float y);

// Division for single precision floating-point numbers
float fdivision(float dividend, float divisor);

// Square roots for single precision floating-poiint number
float fsqareroot(float ff);

// Absolute value
float fabsolute(float f);

// Fahrenheit to Celsius temperature conversion
float ftemperature_Conv(float fahrenheit);

// Addition for double precision floating-point numbers
double daddition(double x, double y);

// Subtraction for double precision floating-point numbers
double dsubtraction(double x, double y);

// Multiplication for double precision floating-point numbers
double dmultiplication(double x, double y);

// Division for double precision floating-point numbers
double ddivision(double dividend, double divisor);

// Power of n(d^n)
double dpower(double d, double power);

int main()
{
	// Declare variables here
	char ch, ch1, ch2, ch3;
	// Single precision floating-point variables
	float f1, f2, fsum, fsub, fprod, fquot, fqareroot, fabso, TemperatureC;
	// Doubles precision floating-point variables
	double d1, d2, dsum, dsub, dmult, ddiv, dpo;

	cout << "Start the floating point calculator Y/N, enter Y(y) or N(n)" << endl;
	cin >> ch;
	ch1 = ch;

	while (ch1 == 'Y' || ch1 == 'y')
	{
		cout << "Menu:\n";
		cout << "1, Single precision floating point arithmetic operation (32-bit)\n";
		cout << "2, Double precision floating point arithmetic operation (64-bit)\n";
		cout << "3, Exit\n";

		cout << "Menu Options:\n";
		cin >> ch;
		ch2 = ch;
		if (ch2 == '1')
		{
		Submenu1:
			cout << "Submenu - input your choice:\n";
			cout << "a, Input two floating point operands for addition, and display" << "The sum in the format of a decimal number.\n";
			cout << "b, Input two floating point operands for subtraction, and display" << "The difference in the format of a decimal number.\n";
			cout << "c, Input two floating point operands for multiplication, and display" << "The product in the format of a decimal number.\n";
			cout << "d, Input two floating point operands for division, and display" << "The quotient in the format of a decimal number.\n";
			cout << "e, Input a floating-point operand, and display\n" << "The square roots in decimal format.\n";
			cout << "f, Input a floating-point operand, and display\n" << "The absolute value in decimal format.\n";
			cout << "g, Input a Fanrenheit temperature, and display \n" << "The Celsius temperature in decimal format.\n";
			cout << "q, Quit\n";

			cin >> ch;
			ch3 = ch;
			switch (ch3)
			{
			case 'a':
			{
				cout << "Input two floating point operands in decimal format: \n";
				cin >> f1;
				cin >> f2;
				cout << "The first floating point value is:" << f1 << endl;
				cout << "The second flaoting point value is:" << f2 << endl;
				fsum = faddition(f1, f2);
				cout << "The sum of:" << f1 << " and " << f2 << " is " << fsum << endl;
				printf("=======================================================\n");
				break;
			}
			case 'b':
			{
				printf("Input two floating point operands in decimal format:\n");
				cin >> f1;
				cin >> f2;
				cout << "The first floating point value is: " << f1 << endl;
				cout << "The second floating point value is: " << f2 << endl;
				fsub = fsubtraction(f1, f2);
				cout << "The difference of:" << f1 << " minus " << f2 << " is " << fsub << endl;
				printf("=======================================================\n");
				break;
			}
			case 'c':
			{
				printf("Input two floating point operands in decimal format:\n");
				cin >> f1;
				cin >> f2;
				cout << "The first floating point value is: " << f1 << endl;
				cout << "The second floating point value is: " << f2 << endl;
				fprod = fmultiplication(f1, f2);
				cout << "The product of:" << f1 << " and " << f2 << " is " << fprod << endl;
				printf("=======================================================\n");
				break;
			}
			case 'd':
			{
				printf("Input two floating point operands in decimal format:\n");
				cin >> f1;
				cin >> f2;
				cout << "The first floating point value is: " << f1 << endl;
				cout << "The second floating point value is:  " << f2 << endl;
				fquot = fdivision(f1, f2);
				cout << "The quotient of the dividend:" << f1 << " divided by the divisor " << f2 << " is " << fquot << endl;
				printf("=======================================================\n");
				break;
			}
			case 'e':
			{
				printf("Input one floating point operands in decimal format:\n");
				cin >> f1;
				
				cout << "The first floating point value is:" << f1 << endl;
			
				fqareroot = fsqareroot(f1);
				cout << "The square root value: " << f1  << " is " << fqareroot << endl;
				printf("=======================================================\n");
				break;
			}
			case 'f':
			{
				printf("Input one floating point operands in decimal format:\n");
				cin >> f1;
				
				cout << "The first floating point value is: " << f1 << endl;
				
				fabso = fabsolute(f1);
				cout << "The absoluve value:" << f1  << " is " << fabso << endl;
				printf("=======================================================\n");
				break;
			}
			case 'g':
			{
				printf("Input one floating point operands in decimal format:\n");
				cin >> f1;

				cout << "The first floating point value is: " << f1 << endl;

				TemperatureC = ftemperature_Conv(f1);
				cout << "The Temperature:" << f1 << " is " << TemperatureC << endl;
				printf("=======================================================\n");
				break;
			}
			default: goto QuitLable;
			}

			goto Submenu1;
		}
		else if (ch2 == '2')
		{
			Submenu2:
			cout << "Submenu - input your choice\n";
			cout << "a, Input two floating point operands for addition, and display " << "The sum in the format of a decimal number.\n";
			cout << "b, Input two floating point operands for subtraction, and display " << "The difference in the format of a decimal number.\n";
			cout << "c, Input two floating-point operands for multiplication, and display\n" << "The product in the format of a decimal number.\n";
			cout << "d, Input two floating-point operands for division, and display \n" << "The quotient in the format of a decimal number.\n";
			cout << "e, Input two floating-point operands, and display\n" << "The power of n in the format of a decimal number.\n";
			cout << "q. Quit\n";

			cin >> ch;
			ch3 = ch;
			switch (ch3)
			{
			case 'a':
			{
				cout << "Input two floating point operands in decimal format:\n";
				cin >> d1;
				cin >> d2;
				cout << "The first floating point operant is: " << d1 << endl;
				cout << "The second floating point operant is: " << d2 << endl;
				dsum = daddition(d1, d2);
				cout << "The sum of:" << d1 << " and " << d2 << " is " << dsum << endl;
				printf("=======================================================\n");
				break;
			}
			case 'b':
			{
				printf("Input two floating point operands ind ecimal format:\n");
				cin >> d1;
				cin >> d2;
				cout << "The first floating point operand is: " << d1 << endl;
				cout << "The seconf floating point operant is: " << d2 << endl;
				dsub = dsubtraction(d1, d2);
				cout << "The difference of:" << d1 << " minus " << d2 << " is " << dsub << endl;
				printf("=======================================================\n");
				break;
			}
			case 'c':
			{
				printf("Input two floating point operands ind ecimal format:\n");
				cin >> d1;
				cin >> d2;
				cout << "The first floating point operand is: " << d1 << endl;
				cout << "The seconf floating point operant is: " << d2 << endl;
				dmult = dmultiplication(d1, d2);
				cout << "The difference of:" << d1 << " multiplication " << d2 << " is " << dmult << endl;
				printf("=======================================================\n");
				break;
			}
			case 'd':
			{
				printf("Input two floating point operands ind ecimal format:\n");
				cin >> d1;
				cin >> d2;
				cout << "The first floating point operand is: " << d1 << endl;
				cout << "The seconf floating point operant is: " << d2 << endl;
				ddiv = ddivision(d1, d2);
				cout << "The difference of: " << d1 << " Division " << d2 << " is " << ddiv << endl;
				printf("=======================================================\n");
				break;
			}
			case 'e':
			{
				printf("Input two floating point operands ind ecimal format:\n");
				cin >> d1;
				cin >> d2;
				cout << "The first floating point operand is: " << d1 << endl;
				cout << "The seconf floating point operant is: " << d2 << endl;
				dpo = dpower(d1, d2);
				cout << "The difference of:" << d1 << " Division " << d2 << " is " << dpo << endl;
				printf("=======================================================\n");
				break;
			}
			default: goto QuitLable;
			}
			goto Submenu2;
		}
		else
		{
			goto EndLable;
		}

	QuitLable:
		cout << "Do You like to continue the floating point arithmetic operations (Y/N)?" << "Enter Y(y) or N(n)" << endl;
		cin >> ch;
		ch1 = ch;
	}

EndLable:
	cout << "Exit Program" << endl;

	system("pause");
	exit(0);

	return 0;
}
 
// Addition for single precision floating-point numbers
float faddition(float x, float y)
{
	float f;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FADD;
		
		FST f
	}
	return f;
}

// Subtraction for single precision floating-point numbers
float fsubtraction(float x, float y)
{
	float f;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FSUB ST(1), ST(0);
		
		FXCH ST(1);
		
		FST f;
	}
	return f;
}

// Multiplication for single precision floating-point numbers
float fmultiplication(float x, float y)
{
	float f;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FMUL;
		
		FSTP f;
	}
	return f;
}

// division for single precision floating-point numbers
float fdivision(float x, float y)
{
	float f;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FDIV ST(1), ST(0);
		
		FXCH ST(1);
		
		FSTP f;
	}
	return f;
}

// Square roots for single precision floating-poiint number
float fsqareroot(float x)
{
	float f;

	_asm
	{
		FLD x;
		FSQRT;
		FSTP f;
	}
	return f;
}

// Absolute value
float fabsolute(float x)
{
	float f;
	_asm
	{
		FLD x;
		FABS;
		FSTP f;
	}
	return f;
}

float ftemperature_Conv(float x)
{
	float f;
	float part_1 = 32.0;
	float part_2 = 0.555555;

	_asm
	{
		FLD x;
		FLD part_1;
		FSUB;
		FLD part_2;
		FMUL ST(0), ST(1);
		FSTP f;
	}
	return f;
}

// Addition for double precision float point numbers
double daddition(double x, double y)
{
	double d;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FADD;
		
		FST d;
	}
	return d;
}

// subtraction for double precision float point numbers
double dsubtraction(double x, double y)
{
	double d;
	_asm
	{
		FLD x;
		
		FLD y;
		
		FSUB ST(1), ST(0);
		
		FXCH ST(1);
		
		FST d;
	}
	return d;
}
// subtraction for double precision float point numbers
double dmultiplication(double x, double y)
{
	double d;
	_asm
	{
		
		FLD x;
		
		FLD y;
		
		FMUL;
		
		FSTP d;
	}
	return d;
}

double ddivision(double dividend, double divisor)
{
	double d;
	_asm
	{
		
		FLD dividend;
		
		FLD divisor;
		
		FDIV ST(1), ST(0);
		
		FXCH ST(1);
		
		FSTP d;
	}
	return d;
}

double dpower(double x, double y)
{
	double d;
	_asm
	{
		fld x;
	}
	for (int i = 1; i < y; i++)
	{
		_asm {
			FLD x;
			FMUL
		}
	}
	_asm {

	}
	return d;
}



	
