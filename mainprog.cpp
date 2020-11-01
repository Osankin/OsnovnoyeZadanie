#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstdlib>

int bit_count(int input, bool value);
void generate_list(std::list<int>&l);
void print_list(std::list<int>&l);
void work_with_list(std::list<int>&l, bool value, int &counter_of_bits, int & counter_of_delete_elems);
std::mutex mtx;

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	setlocale(LC_ALL, "ru");
	
	int size = 100;

	int counter_of_zero_bits = 0;
	int counter_of_non_zero_bits = 0;

	int counter_of_delete_elem_1 = 0;
	int counter_of_delete_elem_2 = 0;

	std::list<int> l(size);
	generate_list(l);
	print_list(l);
	std::cout<<"\n";

	std::thread th1(work_with_list, std::ref(l), false, std::ref(counter_of_zero_bits), std::ref(counter_of_delete_elem_1));
	std::thread th2(work_with_list, std::ref(l), true, std::ref(counter_of_non_zero_bits), std::ref(counter_of_delete_elem_2));
	th1.join();
	th2.join();

	std::cout<<"Количество нулевых бит: "<<counter_of_zero_bits<<"\n";
	std::cout<<"Количество единичных  бит: "<<counter_of_non_zero_bits<<"\n";
	std::cout<<"Количество пройденных первым потоком элементов: "<<counter_of_delete_elem_1<<"\n";
	std::cout<<"Количество пройденных вторым потоком элементов: "<<counter_of_delete_elem_2<<"\n";
	return 0;

}

int bit_count(int input, bool value)
{
	int res = 0;
	static const unsigned shift_size = ((sizeof(int)*8)-1);
	//if (input < 0)
	//{
	//	res++;
	//	input &= ((1U<<shift_size)-1);
	//}
	while (input)
	{
		res += (input &1);
		input >>= 1;
	}
	switch (value)
	{
		case false:
			return (sizeof(int)*8)-res;
		case true:
			return res;
	}
}

void generate_list (std::list<int>&l)
{
	std::list<int>::iterator it{l.begin()};
	for (auto i = it; i != l.end(); ++i)
	{
		*i = rand();
	}
}

void print_list (std::list<int>&l)
{
	std::list<int>::iterator it{l.begin()};
	for (auto i = it; i != l.end(); ++i)
	{
		std::cout<<*i<<" ";
	}
}

void work_with_list(std::list<int>&l, bool value, int &counter_of_bits, int &counter_of_delete_elems)
{
	while (!l.empty())
	{
		int temp = 0;
		switch(value)
		{
			case false:
				temp += bit_count(*(l.begin()),value);
				mtx.lock();
				if (!l.empty())
				{
					l.pop_front();
					mtx.unlock();
					counter_of_delete_elems++;
					counter_of_bits += temp;
				}
				else
					mtx.unlock();
				break;
			case true:
				temp += bit_count(*(l.rbegin()),value);
				mtx.lock();
				if (!l.empty())
				{
					l.pop_back();
					mtx.unlock();
					counter_of_delete_elems++;
					counter_of_bits += temp;
				}
				else
					mtx.unlock();
				break;
		}
	}
}

