#include <Arduino.h>
#include <MyArray.hpp>
#define LOG(str,...) Serial.printf(str,##__VA_ARGS__);Serial.println()
void printIntArr(MyArray<int> & arr){
  for (int i = 0; i < arr.getSize(); i++)
  {
    Serial.println(arr[i]); 
  }
  
}
void test01(){
  Serial.println("void test01");
  MyArray<int> arr1(10);
  for (int i = 0; i < 5; i++)
  { 
    arr1.Push_Back(i);
  }
  printIntArr(arr1);
  LOG("arr1的容量为%d", arr1.getCapacity());
  LOG("arr1的大小为%d", arr1.getSize());

  MyArray<int> arr2(11);
  for (int i = 0; i < 5; i++)
  { 
    arr2.Push_Back(i+100);
  }
  arr2.Pop_Back();
  printIntArr(arr2);
  LOG("arr1的容量为%d\n", arr2.getCapacity());
  LOG("arr1的大小为%d\n", arr2.getSize());
}

class Person{
public:
  Person(){};
  Person(String name, int age){
    this->m_name =name;
    this->m_age = age;
  }
  String m_name;
  int m_age;
};
void printPersonArr(MyArray<Person> & arr){
  for (int i = 0; i < arr.getSize(); i++)
  {   
    LOG("姓名：%s  年龄：%d", arr[i].m_name, arr[i].m_age);
  }
  
}
void test02(){
  MyArray<Person> personarr(10);
  Person p1("sunwukong", 999);
  Person p2("zhubajie", 888);
  Person p3("shaheshang", 777);
  Person p4("tangseng", 30);
  personarr.Push_Back(p1);
  personarr.Push_Back(p2);
  personarr.Push_Back(p3);
  personarr.Push_Back(p4);
  printPersonArr(personarr);
  LOG("good day");
  LOG("log1 good day: %d", 365);
  LOG("是否换行检测");
}

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("开始");
  // test01();
  test02();


}

void loop(){}