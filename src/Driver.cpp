/*
 * Driver.cpp
 *
 *  Created on: Jun 11, 2022
 *      Author: 91991
 */
#include "user.h"

unordered_map<string,User*> mapFromNameToUserObject;

	void putListToVector(string list, vector<string>& vec){
		string temp="";
		for(unsigned int i=0;i<list.size();i++){
			if(list[i]==' '){
				vec.push_back(temp);
				temp="";
			}
			else
				temp+=list[i];
		}
		vec.push_back(temp);
	}
	void addNewUser(){
		string name;
		cout<<"Enter the name of the new user: ";
		cin>>name;
		if(mapFromNameToUserObject.find(name)!=mapFromNameToUserObject.end()){
			cout<<"User already exists!";
			return;
		}
		User* user = new User(name);
		mapFromNameToUserObject[name]=user;
		cout<<"User added!\n\n";
	}
	int getTotalUnits(vector<string> ratios){
			int sum=0;
			for(auto x:ratios){
				sum+=stoi(x);
			}
			return sum;
		}
	//Expenses are added in a way that the first person is always the user - so it is ignored
	void addNewExpense(){
		string listOfUsers="";
		string ratioString="";
		vector<string> usersToBeSplitWith;
		vector<string> ratios;
		int amount;
		string expenseName;
		string tag;
		string name;
		cout<<"Enter the name of the user that incurred the expense: ";
		cin>>name;
		if(mapFromNameToUserObject.find(name)==mapFromNameToUserObject.end()){
			cout<<"User does not exist!";
			return;
		}
		cout<<"Enter the list of Users for the bill to be split with: ";
		string s;
		while(true){
			cin>>s;
			usersToBeSplitWith.push_back(s);
			if(cin.peek()=='\n')
				break;
		}
		cout<<"Enter the ratio in which bill is to be split: ";
		while(true){
			cin>>s;
			ratios.push_back(s);
			if(cin.peek()=='\n')
				break;
		}
		cout<<"Enter the amount: ";
		cin>>amount;
		cout<<"Enter the tag of the expense: ";
		cin>>tag;
		mapFromNameToUserObject[name]->addOwedExpense(usersToBeSplitWith, ratios, amount, tag);
		int totalUnits = getTotalUnits(ratios);
		int perUnitAmount=amount/totalUnits;
		for(unsigned int i=1;i<ratios.size();i++){
			if(mapFromNameToUserObject.find(usersToBeSplitWith[i])==mapFromNameToUserObject.end()){
				cout<<"User "<<usersToBeSplitWith[i]<<" does not exist!";
				return;
			}
			else
				mapFromNameToUserObject[usersToBeSplitWith[i]]->addOwesExpense(name,stoi(ratios[i]),perUnitAmount,tag);
		}
		cout<<"Expense added!\n\n";
	}
	void simplifyForUserAndDisplay(){
		string name;
		cout<<"Enter the name of the user: ";
		cin>>name;
		if(mapFromNameToUserObject.find(name)==mapFromNameToUserObject.end()){
			cout<<"User does not exist!";
			return;
		}
		mapFromNameToUserObject[name]->simplify();
	}
	void simplifyForAll(){
		for(auto itr=mapFromNameToUserObject.begin();itr!=mapFromNameToUserObject.end();itr++)
			itr->second->simplify();
	}
	void showExpenditureByUserAndTag(){
		string name;
		cout<<"Enter the name of the user: ";
		cin>>name;
		if(mapFromNameToUserObject.find(name)==mapFromNameToUserObject.end()){
			cout<<"User does not exist!";
			return;
		}
		cout<<"Enter the name of the tag: ";
		string tag;
		cin>>tag;
		mapFromNameToUserObject[name]->expenditureByTag(tag);
	}
	void showNetBalance(){
		string name;
		cout<<"Enter the name of the user: ";
		cin>>name;
		if(mapFromNameToUserObject.find(name)==mapFromNameToUserObject.end()){
			cout<<"User does not exist!";
			return;
		}
		mapFromNameToUserObject[name]->getBalanceDetails();
	}
	void settleBalance(){
		string sender;
		cout<<"\nEnter the name of the first User: ";
		cin>>sender;
		if(mapFromNameToUserObject.find(sender)==mapFromNameToUserObject.end()){
			cout<<"User "<<sender<<" does not exist!";
			return;
		}
		string receiver;
		cout<<"\nEnter the name of the second User: ";
		cin>>sender;
		if(mapFromNameToUserObject.find(sender)==mapFromNameToUserObject.end()){
			cout<<"User "<<receiver<<" does not exist!";
			return;
		}
		int senderToReceiverAmount = mapFromNameToUserObject[sender]->simplifyTransactions(receiver);
		if(senderToReceiverAmount<0)
			cout<<"\n"<<sender<<" owes "<<(-1*senderToReceiverAmount)<<" to "<<receiver;
		else
			cout<<"\n"<<receiver<<" owes "<<senderToReceiverAmount<<" to "<<sender;
		string optionToSettle;
		cout<<"\nDo you want to settle the balance between "<<sender<<" and "<<receiver<<"? (Y/N)";
		cin>>optionToSettle;
		if(optionToSettle=="Y"){
			if(senderToReceiverAmount<0){
				mapFromNameToUserObject[sender]->makePaymentToUser(receiver, senderToReceiverAmount);
				mapFromNameToUserObject[receiver]->receivePaymentFromUser(sender, senderToReceiverAmount);
			}
			else{
				mapFromNameToUserObject[receiver]->makePaymentToUser(sender, senderToReceiverAmount);
				mapFromNameToUserObject[sender]->receivePaymentFromUser(receiver, senderToReceiverAmount);
			}
			cout<<"\nBalance settled!";
		}
	}
int main(){
		int choice;
		cout<<"Hello! Welcome to the Bill Splitting Application!\n";
		while(true){
			cout<<"Please choose one of the following options:\n";
			cout<<"1. Add a new User\n";
			cout<<"2. Add a new expense\n";
			cout<<"3. Simplify expenses and look at the netted balance sheet of a User\n";
			cout<<"4. Simplify the expenses for all Users and look at respective netted balance sheets\n";
			cout<<"5. Show the expenditure by tag for a User\n";
			cout<<"6. Show total owed/owes and balance of a User\n";
			cout<<"7. Show and settle balance between two Users\n";
			cout<<"8. Exit the program\n";
			cout<<"Please enter a choice: ";
			cin>>choice;
			switch(choice){
			case 1: addNewUser(); break;
			case 2: addNewExpense(); break;
			case 3: simplifyForUserAndDisplay(); break;
			case 4: simplifyForAll(); break;
			case 5: showExpenditureByUserAndTag(); break;
			case 6: showNetBalance(); break;
			case 7: settleBalance(); break;
			case 8: exit(0);
			default: cout<<"You seem to have entered an invalid option. Please enter again.\n\n"; break;
			}
		}
		return 1;
	}
