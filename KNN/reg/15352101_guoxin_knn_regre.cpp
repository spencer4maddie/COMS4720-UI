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
const int k=21;
vector <string> trainset[max_sen]; 
string volcab[max_vol];
string emocab[max_emo];
double train_emotag[max_sen][max_emo];
double valid_emotag[max_sen][max_emo];
double result_emotag[max_sen][max_emo];
map<string,int> vol_base; 
double sum[max_sen];
bool OHmatrix[max_sen][max_vol];
bool OHvalid[max_sen][max_vol];
struct dis_emo
{
	double dis;
	int train_NO;
};
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
			OHvalid[valid_sen_num][index]=true; 
		}
		//set_emotag(sentence,pos,valid_sen_num,valid_emotag);
		str.clear();
		valid_sen_num++;
	}
//	for(int row=0;row<valid_sen_num;row++)
//	{
//		for(int j=0;j<vol_num-1;j++)
//		{
//			if(OHvalid[row][j]==true)
//				cout<<j<<" ";
//			
//		}
//		cout<<endl;
//	}
	//311 cout<<valid_sen_num<<endl;
} 
double calcu(int emo_num,int k,dis_emo dis_set[])
{
	double p=0;
	if(dis_set[0].dis==-1)
	{
		return  train_emotag[dis_set[0].train_NO][emo_num];
	}
	for(int i=0;i<k;i++)
	{
		int train=dis_set[i].train_NO;
		double train_poss=train_emotag[train][emo_num];
		double dis_wgh=(1/dis_set[i].dis-1/dis_set[k-1].dis)/(1/dis_set[0].dis-1/dis_set[k-1].dis); 
		p+=train_poss*dis_wgh;
	}
	//cout<<p<<" "; 
	return p;
};
bool cmp(dis_emo a,dis_emo b)
{
	return a.dis<b.dis;
};
void norm()
{
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		for(int emo_cnt=0;emo_cnt<6;emo_cnt++)
			result_emotag[senNO][emo_cnt]=result_emotag[senNO][emo_cnt]/sum[senNO];
	}
};

void KNN_Reg()
{
	//对于每一个validation中的句子 
	//ofstream fout ("dis.txt");
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{
		//计算距离，一个词一个词算 
		dis_emo *dis_set = new dis_emo[train_sen_num];
		for(int base=0;base<train_sen_num;base++)
		{
			double dis=0;
			for(int volNO=0;volNO<vol_num-1;volNO++)
			{	
				dis+=(OHmatrix[base][volNO]-OHvalid[senNO][volNO])*(OHmatrix[base][volNO]-OHvalid[senNO][volNO]);
				
			}
			if(dis==0)//当找到了十分吻合的训练集，则直接搬用这个训练集的结果 
			{
				for(int i=0;i<train_sen_num;i++)
				{
					dis_set[i].dis=0;
				}
				dis_set[base].dis=-1;
				dis_set[base].train_NO= base;
				break;
			}
			dis_set[base].dis=sqrt(dis);//dis 
			//fout<<sqrt(dis)<<" ";
			dis_set[base].train_NO= base;				
		}
		//fout<<endl;
		sort(dis_set,dis_set+train_sen_num,cmp);//排序 
		
		sum[senNO]=0;
		for(int emo_cnt=0;emo_cnt<6;emo_cnt++)
		{
			result_emotag[senNO][emo_cnt]=calcu(emo_cnt,k,dis_set);//对于前k个的进行比例的计算 
			sum[senNO]+=result_emotag[senNO][emo_cnt];
		}
		//cout<<endl;
		
	}
	norm();	
	
	ofstream fout ("result_knn_reg2.txt");
	
	for(int senNO=0;senNO<valid_sen_num;senNO++)
	{	
		//for(int emo_cnt=0;emo_cnt<6;emo_cnt++)
			fout<<result_emotag[senNO][2]<<endl;

	} 
	
}

int main()
{
	createtrainSet();
	createOneHot();
	createvalidOH();
	KNN_Reg();
}
