/*
 * User.cpp
 *
 *  Created on: Jun 14, 2022
 *      Author: 91991
 */
#include "User.h"

User::User(string name){
		this->name=name;
		this->balance=0;
		this->total_owed=0;
		this->total_owes=0;
	}

User::User(string name, double balance, double total_owed, double total_owes, unordered_map<string,int> tagCount){
	this->name=name;
	this->balance=balance;
	this->total_owed=total_owed;
	this->total_owes=total_owes;
	this->tagCount=tagCount;
}

void User::getBalanceDetails(){
	cout<<"Total owes for "<<name<<" is "<<total_owes<<"\n";
	cout<<"Total owed for "<<name<<" is "<<total_owed<<"\n";
	cout<<"Net balance for "<<name<<" is "<<balance<<"\n";
}

double User::getTotalOwes(){
	return this->total_owes;
}

double User::getTotalOwed(){
	return this->total_owed;
}

double User::getBalance(){
	return this->balance;
}

unordered_map<string,int> User::getTagMap(){
	return this->tagCount;
}

void User::balanceOut(){
	balance=total_owes-total_owed;
}
int User::getTotalUnits(vector<string> ratios){
	int sum=0;
	for(auto x:ratios){
		sum+=stoi(x);
	}
	return sum;
}
//Expense when some other spends and this user is involved
void User::addOwesExpense(string name, int multiplier, double perUnitAmount, string tag){
	double owesToUser = multiplier*perUnitAmount;
	transactions[name].push_back(to_string(-1*(owesToUser)));
	total_owes+=owesToUser;
	balanceOut();
	if(tagCount.find(tag)!= tagCount.end())
		tagCount[tag]+=owesToUser;
	else
		tagCount[tag]=owesToUser;
}
//Expense when this user spends
void User::addOwedExpense(vector<string> usersToBeSplitWith,vector<string> ratios,double amount,string tag){
	int totalUnits = getTotalUnits(ratios);
	cout<<"Total units are: "<<totalUnits<<"\n";
	double perUnitAmount=amount/totalUnits;
	cout<<"Amount is "<<amount<<" and per unit amount is "<<perUnitAmount<<"\n";
	for(unsigned int i=1;i<ratios.size();i++){
		double owedByThis = perUnitAmount*stoi(ratios[i]);
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

int User::simplifyTransactions(string name){
	double finalAmount=0;
	vector<string> transactionsWithThisUser = transactions[name];
	for(auto x:transactionsWithThisUser)
		finalAmount+=stoi(x);
	return finalAmount;
}
void User::simplify(){
	cout<<"\nStarting the netting of transactions for "<<name<<":\n";
	for(auto x:transactions){
		cout<<"Transactions with "<<x.first<<"\n";
		double answer = simplifyTransactions(x.first);
		cout<<"For user "<<x.first<<", the net amount owed/owes is "<<answer<<"\n";
	}
}
void User::expenditureByTag(string tag){
	if(tagCount.find(tag)!= tagCount.end())
		cout<<"The user has spent "<<tagCount[tag]<<" on "<<tag<<" so far!";
	else
		cout<<"The user does not have transaction involved with this tag!\n";
}
void User::makePaymentToUser(string name, double amount){
	transactions[name].push_back(to_string(amount));
	total_owes-=amount;
	balanceOut();
}
void User::receivePaymentFromUser(string name, double amount){
	transactions[name].push_back(to_string(-1*amount));
	total_owed-=amount;
	balanceOut();
}
