//#include <iostream>
//using namespace std;
//
//class CPerson
//{
//public:
//	virtual void EatStyle()=0;
//public:
//	void Eat(/* CPerson* this = &aa */ )
//	{
//		cout << "来碗饭" << endl;
//		this->EatStyle();
//		cout << "放到嘴里" << endl;
//		cout << "咬两下" << endl;
//		cout << "咽下去" << endl;
//		cout << "出啦" << endl;
//	}
//};
//
//class CChina : public CPerson
//{
//public:
//	void EatStyle()
//	{
//		cout << "用筷子" << endl;
//	}
//};
//
//class CJapan : public CPerson
//{
//public:
//	void EatStyle()
//	{
//		cout << "用脚" << endl;
//	}
//};
//
//class CUsa : public CPerson
//{
//public:
//	void EatStyle()
//	{
//		cout << "把刀" << endl;
//	}
//};
//
//
//class AAAA :public CPerson
//{
//public:
//	void EatStyle()
//	{
//		cout << "asdasd" << endl;
//	}
//};
//
//
//int main()
//{
//
//	CChina cn;
//	cn.Eat();
//	cout << "=========================" << endl;
//
//	CJapan jp;
//	jp.Eat();
//	cout << "=========================" << endl;
//
//	CUsa us;
//	us.Eat();
//	cout << "=========================" << endl;
//
//	AAAA aa;
//	aa.Eat(/*  &aa  */);
//
//
//	system("pause");
//	return 0;
//}