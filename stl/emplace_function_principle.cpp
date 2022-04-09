//=====================================================================
//
// emplace_function_principle.cpp - 
// 理解stl的emplace的方法。手动实现emplace方法
// Created by wwq on 2022/04/09
// Last Modified: 2022/04/09 17:20:05
//
//=====================================================================
#include <iostream>

//#include <vector>

// 手动实现emplace函数的功能
// 内存分配器
template<typename T>
class MyAllocator
{
public:
	// allocate deallocate方法是开辟内存和释放内存的。
	
	T* allocate(size_t size)
	{
		return (T*)malloc(size * sizeof(T));
	}
	//construct desctruct是在指定内存上去构造一个对象 析构只是析构对象，不释放内存
	template<typename... Types>
	void construct(T* ptr, Types&&... args)
	{
		//arg只是一个参数，而且是Test。需要知道T的构造是哪个。
		new (ptr) T(std::forward<Types>(args)...);

	}
};

template<typename T, typename Alloc = MyAllocator<T>>
class vector
{
public:
	vector():vec_(nullptr),size_(0), idx_(0){}

	//预留内存空间
	void reserve(size_t size)
	{
		vec_ = allocator_.allocate(size);
		size_ = size;
	}

	//通过引用折叠实现这两种方法
	
#if 1
	template<typename Type>
	void push_back(Type&& val)
	{
		allocator_.construct(vec_ + idx_, std::forward<Type>(val));
		idx_++;
	}
#else
	// push_back
	void push_back(const T& val)
	{
		allocator_.construct(vec_ + idx_ , val);
		idx_++;
	}
	void push_back(const T&& val)
	{
		//右值引用
		allocator_.construct(vec_ + idx_, std::move(val));
		idx_++;
	}
#endif
	//Types后面有两个引用表示：引用折叠
	// Types&&...
	// Test&+&& = Test& 左值引用
	// Test && = 右值 根据实参的类型可以自动识别，这是引用折叠的特性。目的就是接口统一
	// 变量本身是左值。
	template<typename... Types>
	void emplace_back(Types&&... args)
	{
		//不管是左值引用变量还是右值引用变量。传递过程重要保持args类型。
		//采用std::forward()完美转发。
		allocator_.construct(vec_ + idx_, std::forward<Types>(args)...);
	}
private:
	T* vec_; //指向元素
	int size_;//长度
	int idx_;//元素格式
	Alloc allocator_;//内存分配器
};


// c++11对 STL容器 push/insert外提供了emplace方法
// std::move 移动， 右值引用&&
// std::forword 完美转发
class Test
{
public:
	Test(int a){std::cout << "Test(int)" << std::endl;}
	Test(int a, int b) {std::cout << "Test(int ,int)" << std::endl;}
	// 传统的赋值，就是左值
	Test(const Test&){std::cout << "Test(const Test&)" << std::endl;}
	// 直接做资源的转移，提高效率
	Test(const Test&&){std::cout << "Test(const Test&&)" << std::endl;}

	~Test(){std::cout << "~Test()" << std::endl;}
};

// map实例化 map<int, std::string> m; m.insert(make_pair(1, "Lili"));
// 构造一个临时变量，map底层存储的也是pair，在底层实现了一个资源转移
// 调用了一个带有右值引用的拷贝构造函数。
// m.emplace(1, "Lili"); 在map底层直接调用构造函数，没有额外的方法产生。
// emplace只需要传入对象所需要的构造的参数.
int main()
{
	// 对标准的stl库的vector进行测试
	// 分配固定大小，防止不够扩容了
	Test t1(10);
	/* std::vector<Test> v; */
	vector<Test> v;
	v.reserve(100);
	std::cout << "============begin==================" << std::endl;
	v.push_back(t1);
	v.emplace_back(t1);
	std::cout << "============end  ==================" << std::endl;

	//临时对象，移动构造少了一次析构
	std::cout << "============begin==================" << std::endl;
	v.push_back(Test(20));
	v.emplace_back(Test(20));
	std::cout << "============end  ==================" << std::endl;
	// 区别只产生一次构造, 不用构造析构临时变量，直接在底层构造去对象;
	std::cout << "============begin==================" << std::endl;
	v.emplace_back(20);
	v.emplace_back(30, 40);
	std::cout << "============end  ==================" << std::endl;

	return 0; 
}
