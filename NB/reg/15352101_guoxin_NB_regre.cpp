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
double a=0.01;
int emo_num = 0; 
int vol_num = 1; 
const int max_sen = 1000;
const int max_vol = 2100;
const int max_emo = 10;
vector <string> trainset[max_sen]; 
string volcab[max_vol];
string emocab[max_emo];
double train_emotag[max_sen][max_emo];
double valid_emotag[max_sen][max_emo];
double result_emotag[max_sen][max_emo];
map<string,int> vol_base; 
double TFmatrix[max_sen][max_vol];
double OHvalid[max_sen][max_vol];
double sum[max_sen];
void set_emotag(string sentence,int pos,int sen_num,double tag[max_sen][max_emo])
{
	string poss;
	stringstream p;
	//cout<<p.str()<<endl;
	int counter=0;
	while(counter<6)
	{
		int start = pos+1;
		int end=sentence.find(',',pos+1);
		if(end==-1)
		end=sentence.size();
		
		poss=sentence.substr(start,end-start);
		
		p<<poss;
		p>>tag[sen_num][counter];
		p.clear();
		//cout<<train_emotag[train_sen_num][counter]<<" ";
		
		pos= sentence.find(',',pos+1);
		counter++;
	} 
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
	pos=sentence.find(',',0);
	//建立记录感情的表格，1、记录比例 2、进行map 
	while(pos!=-1)
	{
		int start = pos+1;
		int end=sentence.find(',',pos+1);
		if(end==-1)
		end=sentence.size();
		
		emocab[emo_num++]=sentence.substr(start,end-start);
		//cout<<sentence.substr(start,end-start)<<endl;
		
		pos= sentence.find(',',pos+1);
	}
		
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
		//构建感情训练集 
		set_emotag(sentence,pos,train_sen_num,train_emotag);
		//cout<<endl;
		
		str.clear();
		train_sen_num++;
	}
	//cout<<train_sen_num<<" "<<vol_num; //623 2087
	fin.close();
	
	//convert_map()，为了加速查找 
	for(int i=1; i<vol_num;i++) 
    { 
        vol_base.insert(make_pair(volcab[i],i)); 
    } 	
}

void createTF()
{
	for(int sen=0;sen<train_sen_num;sen++)
		for(int vol=0;vol<vol_num-1;vol++)
			TFmatrix[sen][vol]=a;
			
	for(int row=0;row<train_sen_num;row++)
	{
		for(int j=0;j<trainset[row].size();j++)//每个单词进行累计 
		{
			string vol=trainset[row][j];
			int index=vol_base.find(vol)->second-1;//找到其对应词汇表中的下标 
			TFmatrix[row][index]++;	//计算出现的次数		
		}
		double d=0;
		for(int j=0;j<vol_num-1;j++)
			if(TFmatrix[row][j]>1)
				d++; 
		
		for(int j=0;j<vol_num-1;j++)//计算出现概率 
		{
			TFmatrix[row][j]/=(a*(d)+trainset[row].size()); //拉普拉斯平滑 
		}

	}
	
//	ofstream fout("TFmatrix.txt");
//	
//	for (int row=0;row<train_sen_num;row++)
//	{
//		for(int j=0;j<vol_num-1;j++)
//		{
//			fout<<TFmatrix[row][j]<<" ";
//		}	
//		fout<<endl;				
//	}
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
	getline(fin,sentence);
	int pos,end;
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		end=sentence.find(",",pos+1);
		//str<<sentence.substr(0,pos);
		str<<sentence.substr(pos+1,end-pos-1);
		while(str>>vol)
		{
			//cout<<vol<<" ";
			int index=vol_base.find(vol)->second-1;
			if(index>=0)
			OHvalid[valid_sen_num][index]++; 
		}
		//set_emotag(sentence,pos,valid_sen_num,valid_emotag);
		str.clear();
		valid_sen_num++;
	}
//	for(int row=0;row<valid_sen_num;row++)
//	{
//		for(int j=0;j<vol_num-1;j++)
//		{
//			if(OHvalid[row][j]>0)
//				cout<<volcab[j+1]<<" ";
//			
//		}
//		cout<<endl;
//	}
	//311 cout<<valid_sen_num<<endl;
} 

void norm()
{
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		for(int emo_cnt=0;emo_cnt<emo_num;emo_cnt++)
			result_emotag[senNO][emo_cnt]=result_emotag[senNO][emo_cnt]/sum[senNO];
	}
};

void NB_Reg()
{
	//对于每一个validation中的句子 
	//ofstream fout ("first.txt");
	memset(result_emotag,false,sizeof(result_emotag));
	memset(sum,false,sizeof(sum));
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		for(int base=0;base<train_sen_num;base++) 
		{
			double p=1;
			for(int volNO=0;volNO<vol_num-1;volNO++)
			{	
				if(OHvalid[senNO][volNO]>0) 
				{
					p*=pow(TFmatrix[base][volNO],OHvalid[senNO][volNO]);//计算要检验的单词下 
				}				
			}
			
			for(int emo_cnt=0;emo_cnt<emo_num;emo_cnt++)
			{	
				result_emotag[senNO][emo_cnt]+=p*train_emotag[base][emo_cnt];//对应各情感乘上比例 
			}	
		}
		
		for(int emo_cnt=0;emo_cnt<emo_num;emo_cnt++)
		{
			sum[senNO]+=result_emotag[senNO][emo_cnt];//
		} 
	}
	norm();	
	
	ofstream fout ("result_NB_reg0.txt");
	
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{		
		//for(int emo_cnt=0;emo_cnt<emo_num;emo_cnt++)
		fout<<result_emotag[senNO][0]<<endl;
		
		//fout<<endl; 
	} 
	
}

int main()
{
	createtrainSet();
	createTF();
	createvalidOH();
	NB_Reg();
}
