//- Symbolic names
const char number = '8';
const char quit = 'q'; // t.kind==quit means that t is a quit Token
const char print = ';'; // t.kind==print means that t is a print Token
const char help = 'h';
const string prompt = "> ";
const string result = "= "; // used to indicate that what follows is a result

//classe
class Token_stream {
public: 
    Token_stream();   // make a Token_stream that reads from cin
    Token get();      // get a Token (get() is defined elsewhere)
    void putback(Token t);    // put a Token back
    void ignore (char c);
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

//constructor
Token_stream::Token_stream() :full(false), buffer(0)    // no Token in buffer
{
}

//putback: per posar tokens al buffer
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // copy t to buffer
    full = true;      // buffer is now full
}


const char name = 'a';          //name token
const char hashy = '#';           //declaration token
const char square = 'S';
const string sqrtkey = "sqrt"; 
const char pw = 'P';

const char const_create = '@'; //per crear constants 


//get: cin un token
Token Token_stream::get()
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full=false;
        return buffer;
    }

    char ch;
    cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)
    switch (ch) {
	    case print:    // for "print"
	    case '(': 
		case ')': 
		case '{': 
		case '}':
		case '+': 
		case '-': 
		case '*': 
		case '/': 
		case '%':
		case '!':
		case '=':
		case ',':
		case hashy:
	        return Token(ch);        // let each character represent itself
	    case '.':
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
	        {    
	            cin.putback(ch);         // put digit back into the input stream
	            double val;
	            cin >> val;              // read a floating-point number
	            return Token(number,val);   // let '8' represent "a number"
	        }
	    default:
	    	if (isalpha(ch)) {
	    		string s;
	    		s += ch;
	    		while (cin.get(ch)&&(isalpha(ch) || isdigit(ch) || ch=='_')) s+=ch;
	    		cin.putback(ch);
	    		if (s == sqrtkey) return Token(square);
	    		if (s == "pow") return Token(pw);
	    		if (s == "const") return Token(const_create, s);
	    		if (s == "quit") return Token(quit);
	    		if (s=="help"||s=="HELP"||s=="Help") return Token(help); //helpmessage: quan s'escriu "help"
	    		return Token(name,s); //(.kind name , .name s)
			}
	        error("Bad token");
    }
}

//ignore: per errors (ignora fins trobar un ;)
void Token_stream::ignore(char c) //c represents the kind of Token
{
	//first look in buffer:
	if (full && c==buffer.kind){
		full = false;
		return;
	}
	full = false;
	//now search input:
	char ch = 0;
	while (cin >> ch) if (ch == c) return;
}
