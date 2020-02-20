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
int emo_num = 0; 
int vol_num = 1; 
const int max_sen = 1000;
const int max_vol = 2100;
const int max_emo = 10;
const int k=9;
vector <string> trainset[max_sen]; 
string volcab[max_vol];
string emocab[max_emo];
string train_emotag[max_sen];
string valid_emotag[max_sen];
string result_emotag[max_sen];
map<string,int> vol_base; 
map<string,int> emo_base;
bool OHmatrix[max_sen][max_vol];
bool OHvalid[max_sen][max_vol];
int emo_app[max_emo]; 
struct dis_emo
{
	double dis;
	string emo;
};
void createtrainSet()
{
	ifstream fin("train_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	
	stringstream str;
	string sentence,vol;
	int pos;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		//cout<<pos<<endl;
		str<<sentence.substr(0,pos);
		while(str>>vol)
		{
			//cout<<vol<<" ";
			
			//检查是否已经出现在词汇表中
			bool flag=true;
			for(int i=1;i<vol_num;i++)
			{
				if(volcab[i]==vol)
				{
					flag=false;
					break;
				}
			} 
			if(flag)
			{ 
				volcab[vol_num++]=vol;
			}	
			 
			trainset[train_sen_num].push_back(vol);
		}
		vol=sentence.substr(pos+1);
		//cout<<vol<<" ";
		//cout<<endl;
		train_emotag[train_sen_num]=vol;
		//构建感情词汇表
		bool flag=true;
		for(int i=0;i<emo_num;i++)
		{
			if(emocab[i]==vol)
			{
				flag=false;
				break;
			}
		} 
			if(flag)
			{ 
				emocab[emo_num++]=vol;
			}
		// 
		str.clear();
		train_sen_num++;
	}
	//623 2087 cout<<train_sen_num<<" "<<vol_num; 
	fin.close();
	
	//convert_map()，为了加速查找 
	for(int i=1; i<vol_num;i++) 
    { 
        vol_base.insert(make_pair(volcab[i],i)); 
    } 
    for(int i=0; i<emo_num;i++) 
    { 
        emo_base.insert(make_pair(emocab[i],i)); 
    } 
}

void createOneHot()
{
	memset(OHmatrix,false,sizeof(OHmatrix));
	string vol;
	for(int row=0;row<train_sen_num;row++)//遍历文本 
	{
		for(int j=0;j<trainset[row].size();j++)//遍历文本中的每个单词 
		{
			vol=trainset[row][j];
			int index=vol_base.find(vol)->second-1;//找到其在词汇库中对应的下标 
			if(index>=0)
			OHmatrix[row][index]=true;//在矩阵中记录 
		}
	}
}

void createvalidOH()
{
	ifstream fin("test_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence,vol;
	memset(OHvalid,false,sizeof(OHvalid));
	int pos,end;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		end=sentence.find(",",pos+1);
		//cout<<pos<<endl;
		//str<<sentence.substr(0,pos);
		str<<sentence.substr(pos+1,end-pos-1);
		while(str>>vol)
		{
			//cout<<vol<<" ";
			int index=vol_base.find(vol)->second-1;
			if(index>=0)
			OHvalid[valid_sen_num][index]=true; 
		}

		//vol=sentence.substr(pos+1);
		//valid_emotag[valid_sen_num]=vol;
	
		str.clear();
		valid_sen_num++;
	}
		for(int row=0;row<valid_sen_num;row++)
		{
			for(int j=0;j<vol_num-1;j++)
			{
				if(OHvalid[row][j]==true)
					cout<<j<<" ";
			
			}
		cout<<endl;
		}
	//311 cout<<valid_sen_num<<endl;
} 
bool cmp(dis_emo a,dis_emo b)
{
	return a.dis>b.dis;
};
void count (string emo)
{
	int index =  emo_base.find(emo)->second;
	emo_app[index]++;
};
void KNN_Cls()
{
	//对于每一个validation中的句子 
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		//计算距离，一个词一个词算 
		dis_emo *dis_set = new dis_emo[train_sen_num];
		for(int base=0;base<train_sen_num;base++)
		{
			double dis=0;
			double a=0,b=0;
			for(int volNO=0;volNO<vol_num-1;volNO++)
			{	
				dis+=OHmatrix[base][volNO]*OHvalid[senNO][volNO];
				if(OHmatrix[base][volNO]==1)
					a++;
				if(OHmatrix[base][volNO]==1)
					b++;
				//dis+=abs(OHmatrix[base][volNO]-OHvalid[senNO][volNO]);//计算lp距离 
			}
			dis_set[base].dis=dis/(sqrt(a)*sqrt(b));
			//dis_set[base].dis=dis;
			dis_set[base].emo= train_emotag[base];
			
		}
		sort(dis_set,dis_set+train_sen_num,cmp);//距离排序 
		
		memset(emo_app,0,sizeof(emo_app));//用于记录每个情感出现的次数 
		for(int i=0;i<k;i++)
		{
			count(dis_set[i].emo);//数情感出现的次数 
		}
		int likeli = emo_app[0];
		string result = emocab[0];
		for(int i=1;i<emo_num;i++)
		{
			//cout<<emocab[i]<<": "<<emo_app[i]<<"  ";
			if(emo_app[i]>likeli)//选择出现最多次，或者相同次数下，出现最早的情感 
			{
				likeli=emo_app[i];
				result = emocab[i];
			}			
		}
		//cout<<endl;
		result_emotag[senNO]= result; 
	}
	ofstream fout("result_knn_cls.txt");
	double counter = 0;
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		fout<<result_emotag[senNO]<<endl;
		//cout<<valid_emotag[senNO]<<" "<<result_emotag[senNO]<<endl;
		//if(valid_emotag[senNO]==result_emotag[senNO])
		//counter++;
	} 
	//cout<<"correct rate is "<<counter/valid_sen_num;
}

int main()
{
	createtrainSet();
	createOneHot();
	createvalidOH();
	KNN_Cls();
}
