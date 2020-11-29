#include<iostream>
#include<cstdlib>
#include<cassert>
#include<fstream>
#include<string>
#include<locale.h>

using namespace std;

const int LEN = 1000;
const long double  EPS = 0.000000001;

bool eq(long double a, long double b)
{
	return ((-EPS < a - b) && (a - b < EPS));
}

struct char_list
{
	char op;
	char_list *next;
	char_list *prev;

	char_list(char op_) : op(op_), next(NULL), prev(NULL) {};
};

struct num_list
{
	long double val;
	num_list *next;
	num_list *prev;

	num_list(long double x) : val(x), next(NULL), prev(NULL) {};
};

long double calc(long double x, long double y, char c)
{
	if (eq(x, 0) && c == '/')
    {
		cout << "Division by zero\n";
		assert(false);
	}

	switch (c)
	{
	case '+':
		return y + x;
	case '-':
		return y - x;
	case '*':
		return y * x;
	case '/':
		return y / x;
	}
}


char_list* push(char_list* stack, char arg)
{
    if (stack == NULL)
    {
        stack = new char_list(arg);
        return stack;
    }
    char_list *new_node = new char_list(arg);
    stack->next = new_node;
    new_node->prev = stack;
    return new_node;
}

num_list* push(num_list* stack, long double arg)
{
    if (stack == NULL)
    {
        stack = new num_list(arg);
        return stack;
    }
    num_list *new_node = new num_list(arg);
    stack->next = new_node;
    new_node->prev = stack;
    return new_node;
}

num_list* pop(num_list* args, long double *rv)
{
	if (args == NULL)
    {
		cout << "Incorrect input\n";
		assert(false);
	}
	*rv = args->val;
	num_list* tmp = args;
	args = args->prev;
	delete(tmp);
	return args;
}

char_list* pop(char_list* stack, char rv[], int *pos, num_list** args)
{
	if (stack == NULL)
    {
		cout << "Incorrect input\n";
		assert(false);
	}
	if (rv)
    {
		long double x, y;
		*args = pop(*args, &x);
		*args = pop(*args, &y);
		long double res = calc(x, y, stack->op);
		*args = push(*args, res);
		rv[(*pos)++] = stack->op;
		rv[(*pos)++] = ' ';
	}
	char_list* tmp = stack;
	stack = stack->prev;
	delete(tmp);
	return stack;
}

bool check_dig(char c)
{
	return (48 <= c && c <= 57 || c == 46 || c == 'e');
}

bool check_op(char c)
{
	return (c == 40 || c == 42 || c == 43 || c == 45 || c == 47);
}

int main()
{
	string s;
	char t[2 * LEN] = {};
	int prior[] = { 1, 0, 3, 2, 0, 2, 0, 3 };

	ifstream file("test.txt");
	getline(file, s);
	file.close();
	cout << s << endl;

	char_list *stack = NULL;
	num_list  *args = NULL;
	int cur = 0;
	int pos = 0;
	char buff[100];
	int b_end = 0;

	while (s[cur] != '=')
    {
		if (check_dig(s[cur]))
		{
			while (check_dig(s[cur]))
				t[pos++] = buff[b_end++] = s[cur++];

			t[pos++] = ' ';
			continue;
		}
		if (check_op(s[cur]))
        {
			int p = s[cur] == '(' ? 4 : prior[s[cur] - 40];

			if ((s[cur] == '+' || s[cur] == '-') && (cur == 0 || s[cur - 1] == '(' || s[cur - 1] == 'e'))
            {
				if (s[cur++] == '-') t[pos++] = buff[b_end++] = s[cur - 1];
				continue;
			}
			if (b_end > 0)
            {
				buff[b_end] = 0;
				long double tmp = atof(buff);
				args = push(args, tmp);
				b_end = 0;
			}
			while (stack && p <= prior[stack->op - 40])
				stack = pop(stack, t, &pos, &args);

			stack = push(stack, s[cur]);
		}
		else
        {
			if (s[cur] == ')')
			{
				if (b_end > 0)
                {
					buff[b_end] = 0;
					long double tmp = atof(buff);
					args = push(args, tmp);
					b_end = 0;
				}
				while (stack && stack->op != '(')
					stack = pop(stack, t, &pos, &args);

				if (stack == NULL)
                {
					cout << "Incorrect input\n";
					assert(false);
				}

				stack = pop(stack, NULL, NULL, &args);
			}
			else
            {
				cout << "Incorrect input\n";
				assert(false);
			}
		}
		cur++;
	}

	if (b_end > 0)
    {
		buff[b_end] = 0;
		long double tmp = atof(buff);
		args = push(args, tmp);
		b_end = 0;
	}
	while (stack)
		stack = pop(stack, t, &pos, &args);

	t[pos] = '=';
	cout << t << endl;
	cout << args->val << endl;
	return 0;
}
