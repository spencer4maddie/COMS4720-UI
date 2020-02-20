#include<iostream>
#include<sstream>
#include<vector>
#include<fstream>
#include<map> 
#include<cstring>
#include<cmath>
#include<set>
#include<algorithm> 
#include<cmath>
using namespace std;
int train_sen_num = 0;
int valid_sen_num = 0;
int test_sen_num = 0;
int item_num = 0; 
int t=15405;
const int max_sen = 5000;
const int max_item = 100;

double train_x[max_sen][max_item];
double valid_x[max_sen][max_item];
double test_x[max_sen][max_item];

double train_y[max_sen];
double valid_y[max_sen];
double result_y[max_sen];
double test_y[max_sen];

double w[max_item];
double acc,rec,pre,f1;
double tp,tn,fp,fn; 
void createSet(string s,double set_x[max_sen][max_item],double set_y[max_sen],int &sen_num)
{
	ifstream fin(s);
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	
	stringstream str;
	string sentence;
	double x;
	int start=0,end;
	while(getline(fin,sentence))
	{
		item_num=0;
		start=0;
		while(sentence.find(",",start)!=-1)
		{	
			end=sentence.find(",",start);
			str<<sentence.substr(start,end-start);
			str>>x; 
			
			set_x[sen_num][++item_num]=x;
			start=end+1;
			str.clear();
		}
		if(s!="test.csv")
		{
			str<<sentence.substr(start);
			str>>x;
			set_y[sen_num]=x;
		}
		set_x[sen_num][0]=1;
		str.clear();
		sen_num++;
	}
	//4000 65cout<<sen_num<<" "<< item_num <<endl; 
	fin.close();
}

void createtrainSet()
{
	string s="train.csv";
	createSet(s,train_x,train_y,train_sen_num);	
}

void createValidSet()
{
	string s="val.csv";
	createSet(s,valid_x,valid_y,valid_sen_num);	
}
void createTestSet()
{
	string s="test.csv";
	createSet(s,test_x,result_y,test_sen_num);
}

int sign(double a)
{
	if(a<0) return -1;
	if(a==0) return 0;
	if(a>0) return 1;
};
void findw()
{
	for(int i=0;i<=item_num;i++)
		w[i]=1;
	
	int senNO=0;
	int con=0;
	int iter=0;
	while(iter<t && con<train_sen_num)//对于非线性的，迭代到t次后停止；对于线性的，找到最优解则停止。 
	{
		if(senNO==0)
			con=0;//连续计数，用于确定是否找到了线性解 ，每次从头开始考察句子时，要复位为0 
		double y=0;//计算第n个是否符合y 
		for(int i=0;i<=item_num;i++)
		{
			y+=w[i]*train_x[senNO][i];
		}
		y=sign(y);
		
		if(y!=train_y[senNO])//不符合，进行修正 
		{
			for(int i=0;i<=item_num;i++)
			{
				w[i]+=train_y[senNO]*train_x[senNO][i];
			}
			iter++;//迭代次数记录累加 
			con=0;//连续计数变为0 
		}
		
		else //符合 
		{
			con++;	//连续满足计数累加 
		}
		
		senNO++;//考察下一个句子 
		if(senNO==train_sen_num)//对下标进行修正 
		{
			senNO=0;
		}	
	}
}
void judge(int sen_num,double base_y[max_sen],double set_x[max_sen][max_item])
{
	tp=0;tn=0;fp=0;fn=0;
	for(int i=0;i<sen_num;i++)
	{
		double y=0;
		for(int j=0;j<=item_num;j++)
		{
			y+=w[j]*set_x[i][j];
		}
		result_y[i]=sign(y);
	}
	 
	for(int i=0;i<sen_num;i++)
	{
		if(base_y[i]==result_y[i])
		{
			if(base_y[i]==1)
				tp++;
			else
				tn++;
		}
		else if(base_y[i]!=result_y[i])
		{
			if(base_y[i]==1)
				fn++;
			else
				fp++;
		}
	}
	cout<<"迭代次数:"<<t<<endl; 
	cout<<"TP:"<<tp<<" ";
	cout<<"TN:"<<tn<<" ";
	cout<<"FP:"<<fp<<" ";
	cout<<"FN:"<<fn<<" ";
	cout<<endl; 
	acc=(tp+tn)/(tp+tn+fn+fp);
	rec=(tp)/(tp+fn);
	pre=(tp)/(tp+fp);
	f1=2*pre*rec/(pre+rec);
	
	cout<<"ACC:"<<acc<<" REC:"<<rec<<" PRE:"<<pre<<" F1:"<<f1<<endl<<endl;
}
void test_result()
{
	ofstream fout("result_init.txt");
	double negcnt=0,poscnt=0; 
	for(int i=0;i<test_sen_num;i++)
	{
		double y=0;
		for(int j=0;j<=item_num;j++)
		{
			y+=w[j]*test_x[i][j];
		}
		test_y[i]=sign(y);
		fout<<test_y[i]<<endl;
		if(test_y[i]==-1)
			negcnt++;
		else if(test_y[i]==1)
			poscnt++;
	}
	cout<<"the result is "<<"neg:"<<negcnt<<" pos:"<<poscnt;
} 
int main()
{
	createtrainSet();
	createValidSet();
	createTestSet(); 
	findw();
	judge(train_sen_num,train_y,train_x);
	judge(valid_sen_num,valid_y,valid_x);
	test_result();
}
