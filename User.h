/*
 * User.h
 *
 *  Created on: Jun 11, 2022
 *      Author: 91991
 */

#ifndef USER_H_
#define USER_H_
#include<string>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<unordered_map>
using namespace std;

class User {
	string name;
	int balance;
	int total_owed;
	int total_owes;
	unordered_map<string,vector<string>> transactions;
	unordered_map<string,int> tagCount;

public:
	User(string name){
		this->name=name;
		this->balance=0;
		this->total_owed=0;
		this->total_owes=0;
	}
	void getBalanceDetails(){
		cout<<"Total owes for "<<name<<" is "<<total_owes<<"\n";
		cout<<"Total owed for "<<name<<" is "<<total_owed<<"\n";
		cout<<"Net balance for "<<name<<" is "<<balance<<"\n";
	}
	void balanceOut(){
		balance=total_owes-total_owed;
	}
	int getTotalUnits(vector<string> ratios){
		int sum=0;
		for(auto x:ratios){
			sum+=stoi(x);
		}
		return sum;
	}
	//Expense when some other spends and this user is involved
	void addOwesExpense(string name, int multiplier, int perUnitAmount, string tag){
		int owesToUser = multiplier*perUnitAmount;
		transactions[name].push_back(to_string(-1*(owesToUser)));
		total_owes+=owesToUser;
		balanceOut();
		if(tagCount.find(tag)!= tagCount.end())
			tagCount[tag]+=owesToUser;
		else
			tagCount[tag]=owesToUser;
	}
	//Expense when this user spends
	void addOwedExpense(vector<string> usersToBeSplitWith,vector<string> ratios,int amount,string tag){
		int totalUnits = getTotalUnits(ratios);
		cout<<"Total units are: "<<totalUnits<<"\n";
		int perUnitAmount=amount/totalUnits;
		cout<<"Amount is "<<amount<<" and per unit amount is "<<perUnitAmount<<"\n";
		for(unsigned int i=1;i<ratios.size();i++){
			int owedByThis = perUnitAmount*stoi(ratios[i]);
			total_owed+=owedByThis;
			string owedByThisUser= to_string(owedByThis);
			try{
			transactions[usersToBeSplitWith[i]].push_back(owedByThisUser);
			}
			catch(...){
				cout<<"Exception with map!/n/n";
			}
		}
		balanceOut();
		if(tagCount.find(tag)!= tagCount.end())
			tagCount[tag]+=perUnitAmount*stoi(ratios[0]);
		else
			tagCount[tag]=perUnitAmount*stoi(ratios[0]);
		cout<<"Updated tag amount for "<<tag<<" is "<<tagCount[tag]<<"\n";
	}
	int simplifyTransactions(vector<string>& allTransactions){
		int finalAmount=0;
		for(auto x:allTransactions)
			finalAmount+=stoi(x);
		return finalAmount;
	}
	void simplify(){
		cout<<"\nStarting the netting of transactions\n";
		for(auto x:transactions){
			cout<<"Transactions with "<<x.first<<"\n";
			int answer = simplifyTransactions(x.second);
			cout<<"For user "<<x.first<<", the net amount owed/owes is "<<answer<<"\n";
		}
	}
	void expenditureByTag(string tag){
		if(tagCount.find(tag)!= tagCount.end())
			cout<<"The user has spent "<<tagCount[tag]<<" on "<<tag<<" so far!";
		else
			cout<<"The user does not have transaction involved with this tag!\n";
	}
	virtual ~User(){
	}
};






#endif /* USER_H_ */
