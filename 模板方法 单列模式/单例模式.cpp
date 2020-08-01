#include <iostream>
using namespace std;




class CPerson
{
private:
	static bool bflag;    //  程序中是否存在对象
private:
	CPerson()
	{
	
	}
	CPerson(const CPerson& ps)
	{
	
	}
	~CPerson()
	{
		
	}
public:
	static CPerson* GetObject()  //  获取对象的接口
	{
		if(bflag == false)
		{
			CPerson* pPerson = new CPerson;
			bflag = true;
			return pPerson;
		}
		else
		{
			return NULL;
		}
	}
	static void DestoryObject(CPerson* pp)
	{
		bflag = false;
		delete pp;
		pp = 0;
	}
};
bool CPerson::bflag = false;



int main()
{
	CPerson* pp = CPerson::GetObject();


	system("pause");
	return 0;
}