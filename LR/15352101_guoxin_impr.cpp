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
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
using namespace std;
int sen_num=0;
int train_sen_num = 0;
int valid_sen_num = 0;
int test_sen_num = 0;
int item_num = 0; 
int t=0;
const int max_sen = 10000;
const int max_item = 50;
double n = 0.1;

double set_x[max_sen][max_item];
double train_x[max_sen][max_item];
double valid_x[max_sen][max_item];
double test_x[max_sen][max_item];

double set_y[max_sen];
double train_y[max_sen];
double valid_y[max_sen];
double temp_y[max_sen];
double result_y[max_sen];
double test_y[max_sen];

double w[max_item];
double acc,rec,pre,f1;
double tp,tn,fp,fn; 
void createSet(string s,double set[max_sen][max_item],double tag[max_sen])
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
			
			set[sen_num][++item_num]=x;
			start=end+1;
			str.clear();
		}
		if(s!="test.csv")
		{
			str<<sentence.substr(start);
			str>>x;
			tag[sen_num]=x;
		}
		set[sen_num][0]=1;
		str.clear();
		sen_num++;
	}
	//4000 65cout<<sen_num<<" "<< item_num <<endl; 
	fin.close();
}

void createtrainANDvalid()
{
	train_sen_num=0;
	valid_sen_num=0;
	for(int i=0;i<sen_num;i++)
	{
		if(i%2==1)//2000 6000 
		{
			for(int j=0;j<=item_num;j++)
			{
				train_x[train_sen_num][j]=set_x[i][j];	
				
			}
			train_y[train_sen_num]=set_y[i];
			train_sen_num++; 
		}
		else
		{
			for(int j=0;j<=item_num;j++)
			{
				valid_x[valid_sen_num][j]=set_x[i][j];	
			}
			valid_y[valid_sen_num]=set_y[i];
			valid_sen_num++;
		}		
	}	
}

int sign(double a)
{
	if(a<=0.5) return 0;
	else return 1;
};
void findw()
{
	for(int i=0;i<=item_num;i++)
		w[i]=1;
	int iter=0;
	int senNO; 
	srand((int)time(0)+iter);
	while(iter<t)//迭代多次 
	{
		senNO=rand()%7999;//随机选取一个 
		//cout<<senNO<<endl;
		double index=0;
		for(int i=0;i<=item_num;i++)
		{
				index+=w[i]*train_x[senNO][i];//得到预测值 
		}	
		temp_y[senNO]=1.0/(1.0+exp(-index));//转化为概率函数 
		
		if((temp_y[senNO]<0.9&&train_y[senNO]==1)||(temp_y[senNO]>0.1&&train_y[senNO]==0) )//如果预测效果不好 
		{
			double corr=0;
			for(int i=0;i<=item_num;i++)
			{	
				corr=(temp_y[senNO]-train_y[senNO])*train_x[senNO][i];//计算每一维的梯度 
				
				w[i]=w[i]-n*corr;//进行修正 
			}
			iter++;
		}			
	}
}
void judge(int sen_num,double base_y[max_sen],double set[max_sen][max_item])
{
	tp=0;tn=0;fp=0;fn=0;
	for(int i=0;i<sen_num;i++)
	{
		double y=0;
		for(int j=0;j<=item_num;j++)
		{
			y+=w[j]*set[i][j];
		}
		y=1/(1+exp(-y));
		//cout<<y<<endl; 
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
	
	//ofstream fout("acc.txt");
	//fout<<acc<<endl; 
	cout<<"ACC:"<<acc<<" REC:"<<rec<<" PRE:"<<pre<<" F1:"<<f1<<endl;
}
void test_result()
{
	ofstream fout("result.txt");
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
		if(test_y[i]==0)
			negcnt++;
		else if(test_y[i]==1)
			poscnt++;
	}
	cout<<"the result is "<<"neg:"<<negcnt<<" pos:"<<poscnt;
} 
ofstream fout1("impr训练正确率.txt");
ofstream fout2("impr验证正确率.txt");
int main()
{
	createSet("train.csv",set_x,set_y);
	//createSet("test.csv",test_x,test_y);
	createtrainANDvalid();
	double max=0;
	int tm;
	for(t=500;t<2000;t+=20)//1529 
	{
		findw();
		judge(train_sen_num,train_y,train_x);
		fout1<<acc<<endl;
		judge(valid_sen_num,valid_y,valid_x);
		fout2<<acc<<endl;
		cout<<endl;
		if(acc>max)
		{
			max=acc;
			tm=t;
		}
				
	}
	cout<<tm<<" "<<max<<endl;	
	//
	//test_result();
}
