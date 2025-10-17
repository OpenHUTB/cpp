// hello.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<memory>

class Myclass
{
public:
	Myclass()
	{
		std::cout << "调用构造函数" << std::endl;
	}

	~Myclass()
	{
		std::cout << "调用析构函数" << std::endl;
	}

	void Say()
	{
		std::cout << "Hello World" << std::endl;
	}
};


int main()
{
	// 会调用构造函数，此时ptr1指针指向的是一个堆上创建的 MyClass 对象
	std::shared_ptr<Myclass> ptr1 = std::make_shared<Myclass>();

	// 然后在局部函数{} 中又加入了一个指针ptr2 也是指向ptr1所指向的对象，
	// 此时就不会再调用构造函数，而且此时引用计数是2，因为有两个指针都指向这个堆上创建的Myclass对象
	{
		std::shared_ptr<Myclass> ptr2 = ptr1;
		ptr2->Say();
		std::cout << "引用计数:" << ptr2.use_count() << std::endl;
	}
	// 在局部函数中{}结束后ptr2自动销毁，但不会调用析构函数，因为ptr2销毁了，引用计数-1，还剩下 1 个

	std::cout << "引用计数:" << ptr1.use_count() << std::endl;

	// 当ptr1销毁后，引用计数为0，此对象才会自动销毁


	// 注意事项：

	return 0;
}

/*
* 工作原理：
shared_ptr 的底层通过一个“控制块”（control block）来记录引用计数。
每当有一个新的 shared_ptr 拷贝或赋值同一块资源，引用计数就会加一；
当某个 shared_ptr 销毁时，引用计数减一；当计数归零，资源自动释放。

这就像几个人（指针）共同拥有一把钥匙（资源），最后一个人离开时，把钥匙扔掉，锁也解除了        

参考：https://blog.csdn.net/2401_87117051/article/details/147782922
*/

