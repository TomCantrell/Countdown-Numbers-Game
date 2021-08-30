

// =============================================================================================
// **************************         Countdown numbers game        ****************************
// =============================================================================================

/*                   ---------------     Summary     ------------------
* The program written here allows a user to play the countdown numbers game and the program 
* attemps to solve the game within the predetermined time of 30 seconds.
* 
*	- How this code works -
* This program works by utilising Reverse Polish Notation (RPN) and large amount of vector/array
* manipulation as well as techniques to optimise the program in order to increase the chances of
* finding a solution to a given game. Firstly consider the following example of RPN notation:
*
*			5 3 + 2 -  is written as (5 + 3) - 2
*
* RPN is read left to right and when one comes across an operation (+,*,-,/) the operation is 
* executed on the preceding integers. Moreover this notation eliminates the requirement for 
* brackets to dictate the order of operation in a series of operations and integers (as shown
* above). 
* 
*	-----------	The Countdown Numbers Game and how solutions are found  ------------
* In the countdown numbers game one has six numbers randomly selected from two sets of tiles
* (small numbers and big numbers) and then attempts to reach a randomly selected target number
* within 30 seconds. This means that with six numbers and five operations for these numbers,
* there are 11 objects (integers and operations) we must use to search for a solution. There 
* are 54 distinct ways to obtain a list of 5 operations from 4 permitted operations (allowing
* for repeats obviously e.g. +-*+/). This means that we have to search through a VERY large
* number of permutations to potentially reach a solution. 11! * 54 ~= 2 billion different 
* permutations. However most of these permutations do not correspond to valid RPN, this means 
* we are required to either eliminate the invalid permutations or systematically write valid
* RPN permutations (an attempt of this is done here as cycling through 2 billion permutations
* is rather foolish). 
*
*		Things to note with RPN:
* [-] In any permutation of six numbers and five operations, the first two objects in the 
* list/vector must be numbers as it makes no sense for an operation to be first as it has 
* no preceding integers to act on.  
* [-] The final object in the list must also be an operation, this leaves eight objects in the 
* middle. ==> 8! = 40320 a much more computationally tractable number of permutations to deal 
* with. 
* [-] Reading from left to right if the number of operations ever exceeds the number of integers
* counted then this cannot be valid RPN as there are not enough integers for the number of 
* operations to act on. This is used to discard invalid RPN.
* 
*		{ A, B, C, D, E, F, op, op, op, op, op } 
*	    |Fixed||--------------------------||Fixed|
*			   |  Permute |   Permute     | 
* 
* In order to construct a list of all the permutations we shall use to search for a solution we
* begin by making a list of the different ways one can choose 4 objects from the 8 inner objects
* underlined above. These are then tested to see if they correspond to valid RPN (op,op,C,D) say 
* would be invalid but (op,C,op,D) would be valid. This is then done on the remaining 4 objects
* in the 8 inner objects, with invalid notations being eliminated. This gives 24192 valid ways of 
* permuting the inner 8 objects that satisfy RPN rules. Furthermore there are guaranteed repeats 
* in the operations used as there are 5 operations and only 4 at the disposal of the user, this 
* in conjunction with the 15 ways one can have two fixed numbers at the beginning gives 
* approximately 1.5 million different combinations. Totalling approximately 75 million permutations
* where before we had 2 billion permutations. 
* 
*			--------- Optimisations and useful techniques ---------
* [-] The rules of countdown aid in narrowing down the search of a solution to a given countdown
* numbers game. If the running total becomes negative or a fraction we can bail on this solution
* instantly to continue our search. 
* [-] As there are 15 ways one can have two fixed numbers at the beginning we can make use of 
* multithreading to improve performance in the search of a solution. As the number of permutations 
* we search through is always a multiple of 15, we can write 5 threads at the beginning (repeated 3
* times to make up the 15). If the solution is found the solution is returned to the user.
* 
* 
* 
* 
-------------=========================================================================------------
-------------|								Enjoy the game!					         |------------
-------------=========================================================================------------
*/
#include<iostream>
#include<cmath>
#include<vector>
#include<iomanip>
#include<ctime>
#include<chrono>
#include<fstream>
#include<sstream>
#include<cassert>
#include<string>
#include<cstdlib>
#include<typeinfo>
#include<algorithm>
#include<random>
#include<stack>
#include<thread>
#include<iterator>
#ifndef MVECTOR_H // the 'include guard'
#define MVECTOR_H // see C++ Primer Sec. 2.9.2

// Class that represents a mathematical vector
class MVector
{
public:
	// constructors
	MVector() {}
	explicit MVector(int n) : v(n) {}
	MVector(int n, double x) : v(n, x) {}
	MVector(std::initializer_list<double> l) : v(l) {}
	// access element (lvalue) // not decalred as const allowing for modification(see example sheet 5, q5.6)
	double& operator[](int index)
	{
		// Assert Error
		if (index <= v.size() - 1 && index >= 0) { return v[index]; }
		else { assert(index >= v.size(), "not accessible"); }// std::cout << "not accessible" << std::endl;  exit(1);
	}
	// access element (rvalue) (see example sheet 5, q5.7)
	double operator[](int index) const {
		//Assert Errors
		if (index <= v.size() - 1 && index >= 0) { return v[index]; }
		else { assert("not accessible"); }// std::cout << "not accessible" << std::endl;  exit(1);
	}
	int size() const { return v.size(); } // number of elements

	void resize(int x) { v.resize(x); }
	void push_back(double x) { v.push_back(x); }

	// Member function to swap elements in MVector
	void swap(int i, int j)
	{
		double tmp = v[i];
		v[i] = v[j];
		v[j] = tmp;
	}
	// Creating inital data
	void initialise_random(double xmin, double xmax)
	{
		size_t s = v.size();
		for (size_t i = 0; i < s; i++)
		{
			v[i] = xmin + (xmax - xmin) * rand() / static_cast<double>(RAND_MAX);
		}
	}
private:
	std::vector<double> v;
};
#endif
// =================================================================================================================================================
// Display vector elements
void display_Mvector(MVector v)
{
	for (int j = 0; j < v.size(); j++)
	{
		std::cout << v[j] << " ";
	}
	std::cout << std::endl;
}
void display_stdvector(std::vector<int> v)
{
	for (int j = 0; j < v.size(); j++)
	{
		std::cout << v[j] << " ";
	}
	std::cout << std::endl;
}
void Display_stdvector_str(std::vector<std::string> v)
{
	for (int j = 0; j < v.size(); j++)
	{
		std::cout << v[j] << " ";
	}
	std::cout << "\n" << std::endl;
}
// =========================================================================================================================================================

// A function that takes a vector of numbers and returns a list of possible operations on the numbers in RPN notation
std::vector<std::string> RPN(MVector v)
{
	MVector v_temp = v;
	std::vector<std::string> RPNs, ops = { "+","*","/","-" };
	for (int j = 0; j < 4; j++)
	{
		RPNs.push_back(std::to_string(v[0]) + " " + std::to_string(v[1]) + ops[j]);
		if (j > 1) // (for non-commutative operations)
		{
			RPNs.push_back(std::to_string(v[1]) + " " + std::to_string(v[0]) + ops[j]);
		}
	}
	return RPNs;
} 
using namespace std;
void makeCombn(vector<vector<int>>& ans, vector<int>& tmp, int n, int left, int k)
{
	if (k == 0)
	{
		ans.push_back(tmp);
		return;
	}
	for (int i = left; i <= n; i++)
	{
		tmp.push_back(i);
		makeCombn(ans, tmp, n, i + 1, k - 1);
		tmp.pop_back();
	}
}

vector<vector<int>> makeCombi(int n, int k)
{
	vector<vector<int>> ans;
	vector<int> tmp;
	makeCombn(ans, tmp, n, 1, k);
	return ans;
}

std::string combn_solv(std::vector<std::string> NUMBERS_ops, int Target)
{
	std::vector<std::string> tmmp=NUMBERS_ops, str=NUMBERS_ops, str_tmp;
	int tot_curr = 0, i = 0, no_ops = 0, soln = 0;
	std::string str_curr, return_str;
	while (no_ops < 5)
	{
		int op_exe = 0;
		if (NUMBERS_ops[i] == "+")
		{
			tot_curr = std::stoi(NUMBERS_ops[i - 1]) + std::stoi(NUMBERS_ops[i - 2]);
			str_curr = "(" + str[i - 1] + " + " + str[i - 2] + ")";
			op_exe = 1;
		}		
		else if (NUMBERS_ops[i] == "*")
		{
			tot_curr = std::stoi(NUMBERS_ops[i - 1]) * std::stoi(NUMBERS_ops[i - 2]);
			str_curr = "(" + str[i - 1] + " * " + str[i - 2] + ")";
			op_exe = 1;
		}
		else if (NUMBERS_ops[i] == "-")
		{
			tot_curr = std::stoi(NUMBERS_ops[i - 2]) - std::stoi(NUMBERS_ops[i - 1]);
			// Check that tot_curr does not become negative
			if (tot_curr < 0) { return "0"; }
			str_curr = "(" + str[i - 2] + " - " + str[i - 1] + ")";
			op_exe = 1;
		}
		else if (NUMBERS_ops[i] == "/")
		{
			tot_curr = std::stoi(NUMBERS_ops[i - 2]) / std::stoi(NUMBERS_ops[i - 1]);
			// Check that tot_curr is not a fraction
			if (tot_curr * std::stoi(NUMBERS_ops[i - 1]) != std::stoi(NUMBERS_ops[i - 2])){ return "0"; }
			str_curr = "(" + str[i - 2] + " / " + str[i - 1] + ")";
			op_exe = 1;
		}
		i++;


		// if vector is of length 3 then just work out the result and check
		//if (NUMBERS_ops.size() < 3)
		//{
			//std::cout << tot_curr << std::endl;
			//break;
		//}
		//std::cout << i << std::endl;
		//Display_stdvector_str(NUMBERS_ops);
		// if operation was executed (need to change vector)
		if (op_exe == 1)
		{
			
			no_ops++;
			std::vector<std::string> tmp, str_tmp;
			for (int j = 0; j < i - 3; j++)
			{
				tmp.push_back(NUMBERS_ops[j]);
				str_tmp.push_back(str[j]);
				soln = j + 1;
			}
			tmp.push_back(std::to_string(tot_curr));
			str_tmp.push_back(str_curr);
			for (int j = i; j < NUMBERS_ops.size(); j++)
			{
				tmp.push_back(NUMBERS_ops[j]);
				str_tmp.push_back(str[j]);
			}
			
			NUMBERS_ops = tmp;
			str = str_tmp;
			i = 0;
		}
		//std::cout << NUMBERS_ops.size() << std::endl;
		//Display_stdvector_str(NUMBERS_ops);cout << soln << endl;
		// Check if the running total is equal to the target
		if (tot_curr == Target)
		{
			for (int p = 0; p < NUMBERS_ops.size(); p++)
			{
				if (NUMBERS_ops[p] == "+") { continue; }
				else if (NUMBERS_ops[p] == "*") { continue; }
				else if (NUMBERS_ops[p] == "-") { continue; }
				else if (NUMBERS_ops[p] == "/") { continue; }
				else if (tot_curr == std::stoi(NUMBERS_ops[p])) { soln=p; }
			}
			if (NUMBERS_ops.size() == 1) {  soln = 0; } //cout << NUMBERS_ops.size() << endl;
			return_str = NUMBERS_ops[soln] + " = " + str[soln]; //cout << return_str << endl;
			return return_str;
		}
	}
	soln = 0;
	return_str = NUMBERS_ops[soln] + " = " + str[soln];
	return return_str;
}

// A function that takes a string of operations and returns a vector of vectors. 
// - Vectors are comprised of distinct permutations of the operations (in accordance with how the vectors will be permuted)
std::vector<std::vector<std::string>> makeCombn_operations(std::string v)
{
	std::vector<std::string> ops = { "+","*","-","/" }, vec(5);
	for (int i = 0; i < v.length(); i++)
	{
		if (v.substr(i, 1) == "+") { vec[i] = "+"; }
		else if (v.substr(i, 1) == "*") { vec[i] = "*"; }
		else if (v.substr(i, 1) == "-") { vec[i] = "-"; }
		else if (v.substr(i,1)=="/") { vec[i] = "/"; }
	}
	// Now make the combinations
	std::vector<std::vector<std::string>> Return_vec;
	std::vector<std::vector<int>> OPs_combn = makeCombi(5, 4); //std::vector<std::vector<std::string>> OPs_combinations;
	for (int i = 0; i < OPs_combn.size(); i++)
	{
		std::vector<std::string> tmp;
		// -------------------------------------------------------
		std::vector<int> indexs = { 0,1,2,3,4 };
		for (int k = 0; k < OPs_combn[i].size(); k++)
		{
			for (int p = 0; p < indexs.size(); p++)
			{
				if (OPs_combn[i][k] == indexs[p] + 1)
				{
					indexs.erase(indexs.begin() + p);
				}
			}
		}//display_stdvector(indexs);
		for (int j = 0; j < OPs_combn[i].size(); j++)
		{
			tmp.push_back(vec[OPs_combn[i][j] - 1]);
		}
		tmp.push_back(vec[indexs[0]]); //Display_stdvector_str(tmp);
		// Test tmp to see if it is a repeat of any other vectors already added
		bool bl = false; // This boolean variable will turn true if we have a repeat
		if (Return_vec.empty() == false)
		{
			// Check the tmp vector created 
			int Add = 0, Sub = 0, Mul = 0, Div = 0;
			for (int j = 0; j < tmp.size() - 1; j++)
			{
				if (tmp[j] == "+") { Add++; }
				else if (tmp[j] == "-") { Sub++; }
				else if (tmp[j] == "*") { Mul++; }
				else { Div++; }
			}
			// Now loop over OPs_combinations to see if there are repeats of the tmp vector just created			
			for (int k = 0; k < Return_vec.size(); k++)
			{
				int Add_ = 0, Sub_ = 0, Mul_ = 0, Div_ = 0;
				for (int j = 0; j < Return_vec[k].size() - 1; j++)
				{
					if (Return_vec[k][j] == "+") { Add_++; }
					else if (Return_vec[k][j] == "-") { Sub_++; }
					else if (Return_vec[k][j] == "*") { Mul_++; }
					else { Div_++; }
				}
				// Now checking if we have a repeat					
				if (Add_ == Add)
				{
					if (Sub_ == Sub)
					{
						if (Mul_ == Mul)
						{
							if (Div_ == Div)
							{// Found a repeat ==> don't add it
								bl = true;
							}
						}
					}
				}
			}
		}
		if (bl == false) { Return_vec.push_back(tmp); }
	}
	return Return_vec;
}

// ------------------------------------------------------------------------------------------------
// This functions serves to create the solution when it is found by the program
std::string Ans(std::vector<std::string> v, std::vector<int> Permutations_i, int T)
{					// Numbers and operations		one of the permutations		Target
	std::string str;
	std::vector<string> v2 = { v[0],v[1] }; 
	for (int i = 0; i < Permutations_i.size(); i++)
	{
		v2.push_back(v[Permutations_i[i]]);
	}
	//Display_stdvector_str(v2);
	str = combn_solv(v2, T);
	//cout << str;
	return str;
}
// ------------------------------------------------------------------------------------------------
// A function that runs through combinations
std::string eval_(std::vector<std::string>& v, std::vector<std::vector<int>> &Permutations, int &T)
{					// Numbers and operations			all 24192 permutations			Target
	// -------------------------------------
	for (int i = 0; i < Permutations.size(); i++)
	{
		// while loop to evaluate the vector of numbers and operations		
		// Variable initialisation
		std::vector<int> v6 = { std::stoi(v[0]),std::stoi(v[1]) }; 
		int no_ops = 0, m = 0, tot_curr = 0, ind_ = 1;
		while (no_ops < 5)
		{
			if (v[Permutations[i][m]] == "+")
			{
				tot_curr = v6[ind_] + v6[ind_ - 1]; // possible optimization, do we need the tot_curr vector?
				v6[ind_ - 1] = tot_curr;
				v6.erase(v6.begin() + ind_);
				no_ops++; ind_ = v6.size() - 1;
			}
			else if (v[Permutations[i][m]] == "*")
			{
				if (v6[ind_] == 1 || v6[ind_ - 1] == 1) { break; } // No benefit to multiplying by 1
				else { tot_curr = v6[ind_ - 1] * v6[ind_]; }
				v6[ind_ - 1] = tot_curr;
				v6.erase(v6.begin() + ind_);
				no_ops++; ind_ = v6.size() - 1;
			}
			else if (v[Permutations[i][m]] == "-")
			{
				// Check
				if (v6[ind_ - 1] - v6[ind_] <= 0) { break; }
				else if (v6[ind_ - 1] - v6[ind_] == v6[ind_]) { break; } // a-b=b (e.g. 10-5=5) worthless calculation
				else { tot_curr = v6[ind_ - 1] - v6[ind_]; }
				v6[ind_ - 1] = tot_curr;
				v6.erase(v6.begin() + ind_);
				no_ops++; ind_ = v6.size() - 1;
			}
			else if (v[Permutations[i][m]] == "/")
			{
				// Check cout << "here" << endl;
				if ((v6[ind_ - 1] / v6[ind_]) * v6[ind_] != v6[ind_ - 1]) { break; }
				else if (v6[ind_ - 1] / v6[ind_] == v6[ind_]) { break; } // Worthless computation when a/b=b (25/5=5)
				else if (v6[ind_] == 1) { break; } // No benefit in dividing by 1
				else { tot_curr = v6[ind_ - 1] / v6[ind_]; }
				v6[ind_ - 1] = tot_curr;
				v6.erase(v6.begin() + ind_);
				no_ops++; ind_ = v6.size() - 1;
			}
			else { v6.push_back(std::stoi(v[Permutations[i][m]])); ind_++; }
			if (tot_curr == T) { return Ans(v, Permutations[i], T); }
			m++; //display_stdvector(v6);
		}
	}
	// -------------------------------------std::cout << "here\n";
	return "0";
}

int main() 
{
	// -----------==================================----------------
	// =======================    Game    ==========================
	// -----------==================================----------------
	std::string line(20, '---');
	std::cout << "\n\n\n        ================================================================================" << std::endl;
	std::cout << "        "+line+line+line+line << std::endl;
	std::cout << "        " + line <<" Welcome to the Countdown Numbers Game! "<< line << std::endl;
	std::cout << "        " + line+line+line+line << std::endl;
	std::cout << "        ================================================================================" << "\n\n" << std::endl;
	// 
	// ----------  This section of the code calculates all the permutations of five operations  --------------
	std::vector<std::string> ops = { "+","*","/","-" };
	std::vector<std::string> ops_perm;
	ops_perm.resize(1024);
	for (int i = 0, pos_1 = 0, pos_2 = 0, pos_3 = 0, pos_4 = 0; i < pow(4, 5); i++)
	{
		if (i % 4 == 0)
		{
			pos_1 = i / 4;
			if (i % 16 == 0) { pos_2++; }
			if (i % 64 == 0) { pos_3++; }
			if (i % 256 == 0) { pos_4++; }
		}
		ops_perm[i] = ops[pos_4 - 1] + ops[pos_3 - 1 - (pos_4 - 1) * 4] + ops[pos_2 - 1 - (pos_3 - 1) * 4] + ops[pos_1 - (pos_2 - 1) * 4] + ops[i - pos_1 * 4];
	}
	// Obtain a vector of distinct set of operations
	std::vector<std::string> opers;
	for (int pos = 0; pos < ops_perm.size(); pos++)
	{
		std::string str = ops_perm[pos];
		//std::cout << str << "\n" << std::endl;
		int plus = 0, div = 0, times = 0, minus = 0;
		for (int i = 0; i < str.size(); i++)
		{
			std::string op = str.substr(i, 1);
			//std::cout << op << std::endl;
			if (op == "+") { plus++; }
			else if (op == "/") { div++; }
			else if (op == "*") { times++; }
			else if (op == "-") { minus++; }
		}
		for (int j = pos + 1; j < ops_perm.size(); j++)
		{
			int plus_ = 0, div_ = 0, times_ = 0, minus_ = 0;
			std::string str_ = ops_perm[j];
			for (int p = 0; p < str_.size(); p++)
			{
				std::string op_ = str_.substr(p, 1);
				if (op_ == "+") { plus_++; }
				else if (op_ == "/") { div_++; }
				else if (op_ == "*") { times_++; }
				else if (op_ == "-") { minus_++; }
			}
			if (plus_ == plus && div_ == div && times_ == times && minus_ == minus)
			{
				//std::cout << j << std::endl;
				//ops_perm.erase(ops_perm.begin() + j);
				ops_perm[j] = "0";
			}
		}
	}
	//std::cout << ops_perm.size() << std::endl;
	for (int m = 0; m < ops_perm.size(); m++)
	{
		if (ops_perm[m] != "0")
		{
			opers.push_back(ops_perm[m]);
		}
	}
	// 
	// Press enter or specified key to start game
	MVector LargeNumbers = { 25, 50, 75, 100 };
	MVector SmallNumbers = { 1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10 };
	std::cout << "        ================================================================================" << std::endl;
	std::cout << "           Large Numbers:                           ";
	display_Mvector(LargeNumbers);
	std::cout << "\n           Small Numbers:           ";
	display_Mvector(SmallNumbers);
	std::cout << "        ================================================================================" << "\n"<< std::endl;
	// Interaction w/ user defining how many from each set
	std::vector<int> NUMBERS;
	int small, large, k=1, p=1, tot=6;
	std::cout << "        How many numbers large numbers would you like?\n";
	std::cout << "        Please enter a number between 0 and 4, or if you\n        would like to enter your own numbers type -1: ";
	std::cin >> large;
	if (large != -1)
	{
		while (k == 1)
		{
			if (large > 4 || large < 0)
			{
				std::cout << "     \nPlease enter how many numbers you would like from the large number set: ";
				std::cin >> large;
			}
			else if (large <= 4 && large >= 0)
			{
				k = 0;
				small = tot - large;
			}
		}
	}
	else
	{
		int num_tot = 6;
		while (num_tot > 0)
		{
			int num_input;
			std::cout << "        Please input your numbers (" << num_tot << ") :  ";
			std::cin >> num_input;
			NUMBERS.push_back(num_input);
			num_tot--;
		}
	}
	//std::cout << opers.size() << std::endl;
	// --------------------   Numbers are randomly chosen here  -------------------------
	int j = 0; std::vector<int> LargeNumbers_temp = {25,50,75,100};
	if (large != -1)
	{
		while (j < large)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(0, LargeNumbers_temp.size() - 1); int a = distr(gen);
			NUMBERS.push_back(LargeNumbers_temp[a]);
			LargeNumbers_temp.erase(LargeNumbers_temp.begin() + a);
			j++;
		}
		int i = 0; std::vector<int> SmallNumbers_temp = { 1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10 };
		while (i < small)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(0, SmallNumbers_temp.size() - 1); int b = distr(gen);
			NUMBERS.push_back(SmallNumbers_temp[b]);
			SmallNumbers_temp.erase(SmallNumbers_temp.begin() + b);
			i++;
		}
	}
	// FOR TESTING PURPOSES ONLY
	//NUMBERS = { 25,8,10,7,1,5 };
	// ==========================================================================================
	// --------------------------  Generating VALID RPN notation  -------------------------------
	// ==========================================================================================
	std::vector<std::string> N_ops = { "N","N", "N", "N", "N", "N", "op","op","op","op","op" };
	// Code that chooses distinct set of 4 from 8 in this case
	vector<vector<int>> permns_tmp = makeCombi(8, 4), permns;
	// Checking
	std::vector<int> ind;
	for (int p = 0; p < permns_tmp.size(); p++)
	{
		std::vector<int> vec_tmp = permns_tmp[p];
		std::vector<std::string> v1;
		for (int i = 0; i < vec_tmp.size(); i++)
		{
			v1.push_back(N_ops[vec_tmp[i] + 1]);
		}
		// Check validity of RPN
		int N = 2, op = 0;
		for (int j = 0; j < v1.size(); j++)
		{
			if (v1[j] == "N") { N++; }
			else { op++; }
			if (N == op)
			{
				ind.push_back(p); //Display_stdvector_str(v1);//permns_tmp.erase(permns_tmp.begin() + p);
			}
		}
	}//std::cout << "\n\n" << permns_tmp.size() << std::endl;
	for (int t = ind.size() - 1; t > 0; t--)
	{
		permns_tmp.erase(permns_tmp.begin() + ind[t]);
	}//std::cout << "\n\n" << permns_tmp.size() << std::endl;
	// Add the permutations of each of the permutations
	std::vector<int> v_tmp = { 0,1,2,3 }; int u = 0;
	for (int j = 0; j < permns_tmp.size(); j++)
	{
		std::vector<int> tmp = permns_tmp[j];
		// Create permutations for each vector
		do
		{
			bool b = true;
			std::vector<int> tmpp;
			for (int e : v_tmp) { tmpp.push_back(tmp[e]); }
			// Check the validity of the permutation
			std::vector<std::string> v1;
			for (int i = 0; i < tmpp.size(); i++)
			{
				v1.push_back(N_ops[tmpp[i] + 1]);
			}
			//Display_stdvector_str(v1);
			int N = 2, op = 0;
			for (int j = 0; j < v1.size(); j++)
			{
				if (v1[j] == "N") { N++; }
				else { op++; }
				if (N == op)
				{
					u++; b = false;//Display_stdvector_str(v1);//break;
				}//else {  }
			}
			if (b == true)
			{
				permns.push_back(tmpp);	//Display_stdvector_str(v1);
			}
		} while (std::next_permutation(v_tmp.begin(), v_tmp.end()));
	}
	// -----------------------------------------------------------------------------------------
	//cout << permns.size() << endl;
	//cout << u << endl;
	// Creating the permutations of 8 
	std::vector<std::vector<int>> Permutations;
	for (int p = 0; p < permns.size(); p++)
	{
		// Calculate the valid permutations for eight 
		std::vector<int> v2 = permns[p], pos = { 1,2,3,4,5,6,7,8 }, pos_remaining; int N_ = 2, op_ = 0;
		for (int j = 0; j < v2.size(); j++)
		{
			if (N_ops[v2[j] + 1] == "N") { N_++; }
			else { op_++; }
			for (int k = 0; k < pos.size(); k++)
			{
				if (pos[k] == v2[j]) { pos[k] = -1; }
			}
		}
		for (int i = 0; i < pos.size(); i++)
		{
			if (pos[i] != -1) { pos_remaining.push_back(pos[i]); }
		}
		// -------------------------------------------------------
		// Run a do while loop to permute the 4 remaining positions
		do
		{
			std::vector<int> v3 = v2;
			for (int e : v_tmp)
			{
				v3.push_back(pos_remaining[e]);
			}
			// Initialise and check
			int No = N_, Op = op_; bool B = true;
			for (int k = 0; k < v3.size(); k++)
			{
				if (N_ops[v3[k] + 1] == "N") { No++; }
				else { Op++; }
				if (No == Op) { B = false; }
			}
			if (B == true) { v3.push_back(9); Permutations.push_back(v3); }
		} while (next_permutation(v_tmp.begin(), v_tmp.end()));
	}
	// cout << Permutations.size() << endl;
	//
	for (int i = 0; i < Permutations.size(); i++)
	{
		for (int j = 0; j < Permutations[i].size(); j++)
		{
			Permutations[i][j]++;
		}
	}
	// ---------------------------------------------------------------------------------------------------------
	// Code that calculates permutations of numbers and operations
	std::vector<std::vector<int>> Numbers_combn = makeCombi(NUMBERS.size(), 2);
	// Note: does not account for repeats, 15 vectors produced
	// To be run at the beginning of the game, does not need to be run during finding solutions
	for (int p = 0; p < Numbers_combn.size(); p++)
	{
		// Combinations already made above
		//std::vector<int> n_tmp = NUMBERS;
		std::vector<int> positions = { 0,1,2,3,4,5 };
		for (int k = 0; k < Numbers_combn[p].size(); k++)
		{
			// Replace with number values
			Numbers_combn[p][k] = Numbers_combn[p][k]-1;
			for (int j = 0; j < positions.size(); j++)
			{
				if (positions[j] == Numbers_combn[p][k]) { positions.erase(positions.begin() + j); }
			}
			/*
			Numbers_combn[p][k] = NUMBERS[Numbers_combn[p][k] - 1];
			for (int j = 0; j < n_tmp.size(); j++)
			{
				if (Numbers_combn[p][k] == n_tmp[j]) { n_tmp.erase(n_tmp.begin() + j); }
			}
			*/
		}	
		Numbers_combn[p][0] = NUMBERS[Numbers_combn[p][0]];
		Numbers_combn[p][1] = NUMBERS[Numbers_combn[p][1]];
		for (int y = 0; y < positions.size(); y++)
		{
			Numbers_combn[p].push_back(NUMBERS[positions[y]]);
		}
		//display_stdvector(Numbers_combn[p]);
	}
	// Turn to strings ready for computation
	std::vector<std::vector<std::string>> Numbers_combn_str(Numbers_combn.size());
	for (int i = 0; i < Numbers_combn.size(); i++)
	{
		std::vector<string> tmp(Numbers_combn[i].size());
		for (int k = 0; k < Numbers_combn[i].size(); k++)
		{
			tmp[k] = std::to_string(Numbers_combn[i][k]);
		}
		Numbers_combn_str[i] = tmp;
		//Display_stdvector_str(tmp);
	}
	/* Now we display the numbers to the user we randomly generated from the sets of large numbers and small numbers
	* and randomly generate a number that will be our target. 
	*/
	// If the user has entered -1 to set their own numbers, here we give them the opportunity to set their own target.
	int T = 0;
	if (large == -1)
	{
		while (T < 101 || T > 999)
		{
			std::cout << "\n        Please enter a target number: ";
			std::cin >> T;
		}
	}
	std::cout << "\n\n        Your Numbers are :\n" << std::endl;
	std::cout << "         ================================================================================" << std::endl;
	std::cout << "        ";
	for (int p = 0; p < NUMBERS.size(); p++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "     |    " << NUMBERS[p];
	}
	std::cout << "     |    \n";
	//std::cout << "          \n";
	std::cout << "         ================================================================================\n" << std::endl;
	std::cout << "         And your target number is      ";
	std::vector<int> vec = { 1,2,3 }; int cnt = 0;
	for (int k = 0; k < 40; k++)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(75)); // 200ms 
		int var = k / 2;
		if (2 * var == k) { std::cout << "\b\b\b"; }
		else if (vec[cnt] == 1) { std::cout << "///"; cnt++; }
		else if (vec[cnt] == 2) { std::cout << "---"; cnt++; }
		else if (vec[cnt] == 3) { std::cout << "\\\\\\"; cnt = 0; }

	}
	std::cout << " \b\b";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "\n" << std::endl;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(101,999);
	int Target; 	
	if (large == -1) { Target = T; }
	else { Target = distr(gen); }
	// --- Testing ---
	//Target = 988;
	// ---------------
	std::cout << "               TARGET :                        " << Target << "\n\n";
	// Begin countdown from 30 and call countdown solver fn
	// =========================================================
	// IMPORTANT data: 
	/*		- Vector of Permutations - Permutations
			- Vector of distinct operations - opers
			- Vector of distinct Numbers - Numbers_combn
	*/
	// =========================================================
	/*
	for (int l = 0; l < opers.size(); l++)
	{
		//Display_stdvector_str(NUMBERS_opers[l]);
		cout << opers[l] << endl;
	}
	*/
	//std::cout << "=============\n";
	//std::cout << opers.size() << std::endl;
	/* -----------------------------------------------------------------------------------------
	This section of code rearranges the list of permutations of 5 operations so that 
	the sets of operations which are most likely to make a solution are used first in a search.
		-	We only add sets of permutations to the list if * is one of the operations
		-	We only add sets of permutations if they have a good variety of operations
		   --> dicated by the size of makeCombn_operations vector; large vector ==> lots of variety
		-	
	*/
	std::vector<std::string> opers_tmp;
	for (int k = 0; k < 5; k++)
	{
		for (int i = 0; i < opers.size(); i++)	
		{
			if (makeCombn_operations(opers[i]).size() == 5 - k) 
			{ 
				int addn = 0, subn = 0, muln = 0, divn = 0;
				for (int j = 0; j < makeCombn_operations(opers[i]).size(); j++)
				{
					
					for (int l = 0; l < makeCombn_operations(opers[i])[j].size(); l++)
					{
						// Prescribing conditions desirable for a set of operations
						if (makeCombn_operations(opers[i])[j][l] == "*")
						{
							muln++;
							//opers_tmp.push_back(opers[i]);
						}
						else if (makeCombn_operations(opers[i])[j][l] == "+") { addn++; }
						else if (makeCombn_operations(opers[i])[j][l] == "-") { subn++; }
						else { divn++; }
					}				
					// To add or not to add to the search space
					if (muln >= 1 && muln <= 3 && divn <= 2 && addn <= 3 && subn <= 2)
					{
						opers_tmp.push_back(opers[i]);
					}
				}

			}
		}
	}
	opers = opers_tmp;
	// This creates 13 strings ...

	/*
	std::cout << "=============\n";
	for (int l = 0; l < opers.size(); l++)
	{
		//Display_stdvector_str(NUMBERS_opers[l]);
		cout << opers[l] << endl;
	}*/
	//std::cout << opers.size() << std::endl;
	/*
										|
										|
										|
				NEED TO CREATE WHAT WE WILL ITERATE OVER -->HERE<--
				AND GET READY TO CALCULATE AS FAST AS POSSIBLE!!!!
										|
										|
										|
		- Create a big loop to loop over all possible combinations, with the ability to break out of all of it.
		- Test on a given set of operations (e.g. "+-+*-+") first, but still create all data required to do full search. 
		- Make optimizations where possible.
		- 
	*/
	// This is pretty fast!
	// Could it be faster??
	double combns = 0;
	std::string strng = "0"; std::cout << "                                           ";
	auto start1 = std::chrono::steady_clock::now();
	int t = 0;
	for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{ 30 }; now = std::chrono::steady_clock::now())
	{
		// Initialising data
		
		std::vector<std::vector<std::string>> Ops_perm = makeCombn_operations(opers[t]), Num_vec;
		// Creates the vectors we will iterate over; this will be a multiple of 15
		for (int i = 0; i < Numbers_combn_str.size(); i++)
		{
			for (int j = 0; j < Ops_perm.size(); j++)
			{
				//Display_stdvector_str(Ops_perm[j]);
				std::vector<std::string> tempry = Numbers_combn_str[i];
				for (int k = 0; k < Ops_perm[j].size(); k++)
				{
					tempry.push_back(Ops_perm[j][k]);
					//cout << Ops_perm[j][k] << endl;
				}
				Num_vec.push_back(tempry);	
			}
		}
		/*
		auto start2 = std::chrono::steady_clock::now();
		std::string strng_ = eval_(Num_vec[0], Permutations, T);
		auto end2 = std::chrono::steady_clock::now(); auto time2_diff = end2 - start2;
		std::cout << "\n                                Time taken: " << std::chrono::duration<double, std::milli>(time2_diff).count() << " milliseconds\n\n\n\n" << std::endl;
		*/
		/*
		for (int l = 0; l < Num_vec.size(); l++)
		{
			Display_stdvector_str(Num_vec[l]);
		}*/
		// Testing for solutions
		//cout << opers[p] << endl;
		std::string thd1 = "0", thd2 = "0", thd3 = "0", thd4 = "0", thd5 = "0";
		//std::string thd6 = "0", thd7 = "0", thd8 = "0", thd9 = "0", thd10 = "0", thd11 = "0", thd12 = "0", thd13 = "0", thd14 = "0", thd15 = "0";
		int r = 0; 
		for (int p = 0; p < Num_vec.size() / 5; p++)
		{
			if (r == 0) { std::cout << "\\ Working \\ "; r++; }
			else if (r == 1) { std::cout << "/ Working / "; r++; }
			else if (r == 2) { std::cout << "- Working - "; r = 0; }
			std::thread t1{ [&]() { thd1 = eval_(Num_vec[5 * p], Permutations, Target); } };
			std::thread t2{ [&]() { thd2 = eval_(Num_vec[5 * p + 1], Permutations, Target); } };
			std::thread t3{ [&]() { thd3 = eval_(Num_vec[5 * p + 2], Permutations, Target);  } };
			std::thread t4{ [&]() {	thd4 = eval_(Num_vec[5 * p + 3], Permutations, Target); } };
			std::thread t5{ [&]() {	thd5 = eval_(Num_vec[5 * p + 4], Permutations, Target); } };
			// ------------------------------------------------------
			t1.join();
			t2.join();
			t3.join();
			t4.join();
			//Display_stdvector_str(Num_vec[5 * p + 4]);
			t5.join();	//cout << Ops_perm.size() << endl;
			//t6.join(); t7.join(); t8.join(); t9.join(); t10.join(); t11.join(); t12.join(); t13.join(); t14.join(); t15.join();
			if (thd1 != "0") { strng = thd1; break; }
			if (thd2 != "0") { strng = thd2; break; }
			if (thd3 != "0") { strng = thd3; break; }
			if (thd4 != "0") { strng = thd4; break; }
			if (thd5 != "0") { strng = thd5; break; }
			std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b";
		}
		combns = combns + 24192.0 *Num_vec.size();
		if (strng != "0") { break; }
		//if (p > 5) { break; }
		t++;
	}
	auto end1 = std::chrono::steady_clock::now(); auto time1_diff = end1 - start1;
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\n" << std::endl;
	if (strng != "0") 
	{
		std::cout << "\n                                 ------- SOLUTION FOUND! -------     \n\n";
		std::cout << "                    ---------------------------------------------------------   \n\n";
		std::cout << "                           " + strng + "    \n\n";
		std::cout << "                    ---------------------------------------------------------   \n\n\n";
	}
	else {
		std::cout << "                             A solution could not be found in time. :(\n\n";
		std::cout << "                            It's possible the solution may not exist...\n";
	}
	std::cout << "                         -------=================================-------     \n";
	std::cout << "                         |    No. of combinations tried:  "<< combns << "  |" << std::endl;
	std::cout << "                         -------=================================-------     \n\n";
	
	std::cout << "\n                                Time taken: " << std::chrono::duration<double, std::milli>(time1_diff).count() << " milliseconds\n\n\n\n" << std::endl;

	// WHY does it take so long to check a vector where the solution cannot be found? Is it doing something unnecessary??
	return 0;
}