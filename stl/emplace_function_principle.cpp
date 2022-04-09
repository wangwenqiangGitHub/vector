#include <iostream>

#include <vector>

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
	std::vector<Test> v;
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
