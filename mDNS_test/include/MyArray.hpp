//自己的通用数组类
#pragma once
#include <Arduino.h>

template<class T>
class MyArray
{
private:
    T * pAddress; //指针指向堆区开辟的真实数组
    int m_Capacity; //数组的容量
    int m_Size; //数组的大小
public:
    //有参构造 参数 容量
    MyArray(int capacity){
        // Serial.println("Myarray有参构造调用");
        this->m_Capacity = capacity;
        this->m_Size = 0;
        this->pAddress = new T[this->m_Capacity];
    }; 
    //拷贝构造 为了防止浅拷贝问题
    MyArray(const MyArray& arr){
        // Serial.println("Myarray拷贝构造调用");
        // 以下为编译器提供的浅拷贝构造
        this->m_Capacity = arr.m_Capacity;
        this->m_Size = arr.m_Size;
        // this->pAddress = arr.pAddress; //此处浅拷贝有问题，需要重新写一个

        //深拷贝
        this->pAddress = new T[arr.m_Capacity];
        for (int i = 0; i < this->m_Size; i++){
            this->pAddress[i] = arr.pAddress[i];
        }
    }
    //operator= 也是为了防止浅拷贝问题
    MyArray& operator=(const MyArray & arr){
        // Serial.println("Myarray operator= 调用");
        //先判断原来堆区是否有数据，如果有先释放干净
        if (this->pAddress != NULL)
        {
            delete [] this->pAddress;
            this->pAddress = NULL;
            this->m_Capacity = 0;
            this->m_Size = 0;
        }
        //深拷贝
        this->m_Capacity = arr.m_Capacity;
        this->m_Size = arr.m_Size;
        this->pAddress = new T[arr.m_Capacity];
        for (int i = 0; i < this->m_Size; i++)
        {
            this->pAddress[i] = arr.pAddress[i];
        }
        return *this;
    }
    //尾插法
    void Push_Back(const T & val){
        //判断容量是否等于大小
        if (this->m_Capacity == this->m_Size)
            return;
        this->pAddress[this->m_Size] = val;
        this->m_Size++;//更新数组的大小
    }
    //尾删法
    void Pop_Back(){
        //让用户访问不到最后一个元素就可以，这是一种逻辑上的删除方法
        if (this->m_Size == 0)
            return;
        this->m_Size--;
    }
    //通过下标的方式访问元素 重载[]运算符 
    //如果想要作为左值 "arr[0] = 100" 还需要返回一个引用 T& 
    T& operator[](int index){
        return this->pAddress[index];
    }

    //返回数组容量
    int getCapacity(){
        return this->m_Capacity;
    }
    //返回数组大小
    int getSize(){
        return this->m_Size;
    }
    //析构
    ~MyArray(){
        if (this->pAddress != NULL){
            // Serial.println("Myarray析构调用");
            delete [] this->pAddress;
            this->pAddress = NULL;
        }
        
    };
};


