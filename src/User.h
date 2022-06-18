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
	User(string name);
	void getBalanceDetails();
	void balanceOut();
	int getTotalUnits(vector<string> ratios);
	//Expense when some other spends and this user is involved
	void addOwesExpense(string name, int multiplier, int perUnitAmount, string tag);
	//Expense when this user spends
	void addOwedExpense(vector<string> usersToBeSplitWith,vector<string> ratios,int amount,string tag);
	int simplifyTransactions(string name);
	void simplify();
	void expenditureByTag(string tag);
	void makePaymentToUser(string name, int amount);
	void receivePaymentFromUser(string name, int amount);
	virtual ~User(){
	}
};






#endif /* USER_H_ */
