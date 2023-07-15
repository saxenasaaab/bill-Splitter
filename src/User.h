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
	double balance;
	double total_owed;
	double total_owes;
	unordered_map<string,vector<string>> transactions;
	unordered_map<string,int> tagCount;

public:
	User(string name);
	User(string name, double balance, double total_owed, double total_owes, unordered_map<string,int> tagCount);
	void getBalanceDetails();
	double getBalance();
	double getTotalOwed();
	double getTotalOwes();
	unordered_map<string,int> getTagMap();
	void balanceOut();
	int getTotalUnits(vector<string> ratios);
	//Expense when some other spends and this user is involved
	void addOwesExpense(string name, int multiplier, double perUnitAmount, string tag);
	//Expense when this user spends
	void addOwedExpense(vector<string> usersToBeSplitWith,vector<string> ratios,double amount,string tag);
	int simplifyTransactions(string name);
	void simplify();
	void expenditureByTag(string tag);
	void makePaymentToUser(string name, double amount);
	void receivePaymentFromUser(string name, double amount);
	virtual ~User(){
	}
};






#endif /* USER_H_ */
