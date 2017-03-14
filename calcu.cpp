
/*
Done by Nil Quera Gener-Febrer 2017 (Ajuda Bjarne Stroustrup)

This program implements a basic expression calculator.
Input from cin; output to cout.
The grammar for input is:

Statement:
	Expression
	Print
	Quit
	
Print:
	;
	
Quit:
	q
	
Expression:
	Term
	Expression + Term
	Expression  Term

Term:
	Second
	Term * Second
	Term / Second
	Term % Second

Second: (fet per mi)
	Primary
	Primary !

Primary:
	sqrt ( Expression )
	pow ( Expression , Primary )
	F
	( Expression )
	 Primary
	+ Primary
	
F:
	floating-point-literal
	
Input comes from cin through the Token_stream called ts.
*/

#include "std_lib_facilities.h"
#include "Token.h"
#include "Token_stream.h"

// variables per - o + previ al número
bool previousplus = false;
bool previousminus = false;

//Classe Variables (matemàtiques)
class Variable {
	public:
		string name;
		double value;
		char type;
	Variable (string n, double v, char t)
		: name(n), value(v), type(t) {}
};


//Vector de variables
vector<Variable> var_table;

//Retorna el valor d'una variable anomenada s
double get_value (string s)
{
	for (const Variable& v : var_table)
		if (v.name == s) return v.value;
	error ("get: undefined variable ", s);
}

//a la variable "s" li dona el valor "d"
void set_value (string s, double d)
{
	for (Variable& v : var_table)
		if (v.name == s){
			v.value = d;
			return;
		}
	error ("set: undefined variable ", s);
}

//proveeix de get() i putback()
Token_stream ts;

//necessaria per fer factorial. Implementada al final
int factorial (int);

//declaració per a que primary() pugui cridar empression()
double expression();

//deal with Fs and parentheses
double primary()
{
    Token t = ts.get();
    switch (t.kind) {
	    case '(':    // handle '(' expression ')'
	        {    
	            double d = expression();
	            t = ts.get();
	            if (t.kind != ')') error("')' expected");
	            return d;
	        }
	    case '{':    // handle '(' expression ')'
	        {    
	            double d = expression();
	            t = ts.get();
	            if (t.kind != '}') error("'}' expected");
	            return d;
	        }
	    case '+':
	    	if (previousplus || previousminus) {
	    		previousplus = false; previousminus = false;
	    		error ("primary expected");
			}
	    	return primary();
	    case '-':
	    	if (previousplus || previousminus) {
	    		previousplus = false; previousminus = false;
	    		error ("primary expected");
			}
	    	return - primary();
	    case square: //handle square root
	    	{
	    		t = ts.get();
	    		if (t.kind != '(') error ("'(' expected");
	    		double d = expression();
	    		if (d<0) error ("You can't calculate square root of the negative F", d);
	    		Token t2 = ts.get();
	    		if (t2.kind != ')') error ("')' expected");
	    		return sqrt(d);
			}
		case pw: //handle power
			{
				t = ts.get();
	    		if (t.kind != '(') error ("'(' expected");
	    		double d = expression();
	    		t = ts.get();
	    		if (t.kind != ',') error ("',' between Fs expected");
	    		double p = expression();
	    		t = ts.get();
	    		return pow(d,p);
	    		if (t.kind != ')') error ("')' expected");
			}
	    case number:
	        return t.value;
	    case name:
	    	return get_value(t.name);
	    default:
	        error("primary expected");
    }
}

//deal with !
double second()
{
	double left = primary();
	Token t = ts.get();
	
	while (true) {
		switch (t.kind) {
			case '!':
				{
					int x = left;
					if (x==0) left = 1;
					else 
					{
						x = factorial(x);
						left = x;
					}
					t = ts.get();
					break;
				}
			default:
				ts.putback(t);
				return left;				
		}
	}
}

//deal with *, /, and %
double term()
{
    double left = second();
    Token t = ts.get();

    while(true) {
        switch (t.kind) {
	        case '*':
	            left *= second();
	            t = ts.get();
	            break;
	        case '/':
	            {    
	                double d = second();
	                if (d == 0) error("divide by zero");
	                left /= d; 
	                t = ts.get();
	                break;
	            }
	        case '%':
	        	{
	        		double d = second();
	        		if (d==0) error ("divide by zero");
	        		left = fmod(left,d);
	        		t = ts.get();
	        		break;
				}
	        default: 
	            ts.putback(t);     // put t back into the token stream
	            return left;
        }
    }
}

//deal with + and
double expression()
{
    double left = term();      // read and evaluate a Term
    Token t = ts.get();        // get the next token from token stream

    while(true) {    
        switch(t.kind) {
	        case '+':
	        	previousplus = true;
	            left += term();    // evaluate Term and add
	            t = ts.get();
	            break;
	        case '-':
	        	previousminus = true;
	            left -= term();    // evaluate Term and subtract
	            t = ts.get();
	            break;
	        default: 
	            ts.putback(t);     // put t back into the token stream
	            return left;       // finally: no more + or -: return the answer
        }
    }
}
// neteja tot fins trobar un ;
void clean_up_mess()
{
	ts.ignore(print);
}

// deteca variable existent
bool is_declared(string var){
	for (const Variable& v : var_table)
		if (v.name == var) return true;
	return false;
}

//push_back nova variable amb valor
void define_name (string var, double val, char t){
	if (is_declared(var)){
		cout << "Do you really want to give another value to "
				<< var << "? (y/n) ";
		char ans;
		do {
			cin >> ans;
		} while (ans!='y' && ans!='n');
		if (ans=='y'){
			for (int i=0; i<var_table.size(); ++i) {
				if (var_table[i].type == 'c') {
					cout << "You can't change a constant's value\n";
					return;
				} else if (var_table[i].type == 'v') {
					if (var_table[i].name == var) {
						var_table[i].value = val;
						return;
					}
				} else error ("Unknown value for type");
			}
		} else if (ans=='n') return; //don't want to change it's value
	}
	else var_table.push_back(Variable(var,val, t));
	return;
}

//declare a variable called "name" with the initial value "expression"
double declaration(char x)
{
	Token t = ts.get();
	if (t.kind != name) error ("name expected in declaration");
	string var_name = t.name;
	
	Token t2 = ts.get();
	if (t2.kind != '=') error ("'=' missing in declaration of ", var_name);
	
	double d = expression();
	define_name (var_name, d, x);
	return d;
}

// statement: handle with declaration() and expression().
double statement(){
	Token t = ts.get();
	switch(t.kind){
		case hashy:
			return declaration('v');
		case const_create:
			return declaration('c');
		default:
			ts.putback(t);
			return expression();
	}
}

// calculate
void calculate() {
	// predefine names:
	define_name("pi",3.1415926535, 'c');
	define_name("e",2.7182818284, 'c');
	define_name("k", 1000, 'c');
	
	while (cin) {
	    	try {
		        Token t = ts.get();
		        while (t.kind == print) t=ts.get();
		        if (t.kind == quit) return;
		    	ts.putback(t);
		        cout << result << statement() << '\n';
			} catch (exception& e) {
				cerr << e.what() << '\n';
				clean_up_mess();
			} 
	    }
}
 
 
 void missatge();
    
//------------------------------------------------------------------------------

int main()
try
{
	missatge();
    calculate();
	keep_window_open();
	return 0;
}
catch (exception& e) {
    cerr << "error: " << e.what() << '\n'; 
	//keep_window_open();
	keep_window_open("~~");
	return 1;
}
catch (...) {
    cerr << "Oops: unknown exception!\n"; 
	keep_window_open();
    return 2;
}

//------------------------------------------------------------------------------

// factorial function
int factorial (int n){
	for (int i = (n-1); i > 1; --i){
		n *= i;
	}
	return n;
}

// missatge inicial
void missatge (){
	for (int i = 0; i < 47; i++){
		cout << "=";
	}
	cout << " CALCULADORA by Nil Quera ";
	for (int i = 0; i < 47; i++){
		cout << "=";
	}
	cout << "\n\n";
	cout << "Operacions principals:         suma (+)    resta (-)    producte (*)    divisio (/)\n\n"
		<< "Operacions especials:         factorial: valor!      arrel quadrada: sqrt(valor)    potència: pow(valor, exponent)\n\n";
	
	for (int i = 0; i < 120; i++){
		cout << "-";
	}
	
	cout << "\n\nConstants disponibles: pi, e, k\n\n"
		<< "Per introduir una nova variable: #nom_variable = valor_variable;\nPer exemple:  #x = 5;\nTip: per fer-la servir, introduir-la alla on posaries un numero.\n\n";
	
	for (int i = 0; i < 120; i++){
		cout << "=";
	}
	cout << "\n\n";
	
}
