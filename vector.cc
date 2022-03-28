#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <stddef.h>
// iterator begin() end() 使用指针实现
// allocator
// vector是连续的容器，动态数组
// date()返回首地址，size()是长度，size的元素是已经初始化的，capacity()是申请的一共分配的空间

namespace my{
template<typename ValueT>
class vector
{
public:
	//储存的元素类型，map<KeyT,ValueT>::value_type ==> pair<const KeyT,ValueT>
	using value_type		= ValueT;
	using reference			= ValueT&;
	using const_reference	= const ValueT&;

	//todo 指针没有越界的检查，需要优化的地方
	using iterator			= ValueT*;
	using const_iterator	= const ValueT*;

	using size_type			= ::size_t;
	//为两个迭代器相减定义一个有符号类型
	using difference_type   = ::ptrdiff_t;

private:
	ValueT* m_data;
	size_type m_size;
	size_type m_capacity;
public:
	// constexpr 
	// const并不能代表“常量”，它仅仅是对变量的一个修饰，告诉编译器这个变量只能被初始化，且不能被直接修改（实际上可以通过堆栈溢出等方式修改）。而这个变量的值，可以在运行时也可以在编译时指定。
	// constexpr可以用来修饰变量、函数、构造函数。一旦以上任何元素被constexpr修饰，那么等于说是告诉编译器 “请大胆地将我看成编译时就能得出常量值的表达式去优化我”
	
	// noexcept noexcept修饰的函数抛出了异常，编译器可以选择直接调用std::terminate()函数来终止程序的运行，这比基于异常机制的throw()在效率上会高一些
	constexpr
	vector() noexcept
		: m_data(), m_size(), m_capacity()
		{ }
	// 作用把已经构造的函数全部销毁掉，分配的内存释放掉；
	//delte参数是一个指针，做了两件事，一件是把指针指向的对象销毁，还会释放指针所指向的内存；我们的内存是vector自己管理的；
	//只想销毁对象，不能释放内存
	~vector()
	{
		do_destruct();
	}
	// 复制构造
	vector(const vector& rhs)
	{
		this->m_data = static_cast<ValueT*>(::operator new(
					rhs.m_capacity * sizeof(ValueT)));
		this->m_size = 0;
		this->m_capacity = rhs.m_capacity;
		try{
		for(::size_t k = 0; k < rhs.m_size; ++k)
		{
			::new(&this->m_data[k]) ValueT(rhs.m_data[k]);
			this->m_size +=1;
		}
		}
		catch(...){
			this->throw_erro(this->m_data, m_size);
			/* for(::size_t k =0; k < this->m_size; ++k) */
			/* 	this->m_data[k].~ValueT(); */
			/* ::operator delete(this->m_data); */
			/* throw; */
		}

	}
	//移动构造
	vector(vector&& rhs) noexcept 
	{
		this->m_data = rhs.m_data;
		this->m_size = rhs.m_size;
		this->m_capacity = rhs.m_capacity;
		rhs.m_data = nullptr;
		rhs.m_size = 0;
		rhs.m_capacity = 0;
	}

	//运算符重载
	vector&
	operator= (const vector& rhs); //TODO

	vector&
	operator=(vector&& rhs) noexcept; // TODO

private:
	void do_destruct() noexcept
	{
		this->clear();
		::operator delete(this->m_data);
	}
	void throw_erro(ValueT* data, ::size_t size)
	{
		for(::size_t k =0; k < size; ++k)
			this->m_data[k].~ValueT();
		::operator delete(data);
		throw;
	}
public:
	//const 和非 const
	iterator begin()noexcept
	{
		return this->m_data;
	}

	const_iterator begin() const noexcept
	{
		return this->m_data;
	}

	iterator end() noexcept
	{
		return this->m_data + this->m_size;
	}
	const_iterator end() const noexcept
	{
		return this->m_data + this->m_size;
	}

	value_type* data() noexcept
	{
		return this->m_data;
	}

	const value_type* data() const noexcept
	{
		return this->m_data;
	}

	size_type size() const noexcept
	{
		return this->m_size;
	}
	size_type capacity() const noexcept
	{
		return this->m_capacity;
	}
	
	bool empty() const noexcept
	{
		return this->m_size == 0;
	}

	void clear() noexcept
	{
		for(::size_t k = 0; k < this->m_size; ++k)
			this->m_data[k].~ValueT();
		this->m_size = 0;
	}
	void pop_back() noexcept
	{
		assert(!this->empty());
		::size_t k = this->m_size - 1;
		this->m_data[k].~ValueT();
		this->m_size = k;
	}

	void push_back(const ValueT& value)
	{
		// printf("<%s %d>\n",__func__, __LINE__);
		this->emplace_back(value);
	}
	void push_back(ValueT&& value)
	{
		// printf("<%s %d>\n",__func__, __LINE__);
		this->emplace_back(::std::move(value));
	}

	template<typename... ArgsT>
	reference emplace_back(ArgsT&&... args)
	{
		if(this->m_size < this->m_capacity)
		{
			// printf("<%s %d>\n",__func__, __LINE__);
			//data cannot be null
			::size_t k=this->m_size;
			::new(&this->m_data[k]) ValueT(::std::forward<ArgsT>(args)...);
			this->m_size += 1;
			return this->m_data[k];
		}

		::size_t new_cacpacity = this->m_size + 1;
		new_cacpacity |= this->m_size / 2; // *1.5

		auto new_data = static_cast<ValueT*>(::operator new(
					new_cacpacity * sizeof(ValueT)));

		::size_t new_size = 0;

		//把原有的元素，移动构造到新的内存中
		try{
			for(::size_t k = 0; k < this->m_size; ++k)
				::new(&new_data[k]) ValueT(::std::move(this->m_data[k])),
				new_size +=1;

			::new(&new_data[new_size]) ValueT(std::forward<ArgsT>(args)...),
				new_size += 1;
		}
		catch(...){
			this->throw_erro(new_data, new_size);
			/* for(::size_t k =0; k < new_size; ++k) */
			/* 	this->m_data[k].~ValueT(); */
			/* ::operator delete(new_data); */
			/* throw; */
		}
		this->do_destruct();
		this->m_data = new_data;
		this->m_size =new_size;
		this->m_capacity = new_cacpacity;
		return  new_data[new_size];
	}
};

} //namespace my

void testStdVector()
{
	std::vector<std::string> vec;
	vec.push_back("helloi vector");
	vec.emplace_back();
	vec.emplace_back("hello", 4);

	for(const auto& str : vec)
		std::cout << str << std::endl;
}
int main()
{
	//testStdVector();
	::my::vector<::std::string> vec;
	vec.push_back("helloworld");
	vec.emplace_back("hello", 4);
	vec.emplace_back();
	for(const auto& str : vec)
		std::cout << str << std::endl;
	return 0;
}
