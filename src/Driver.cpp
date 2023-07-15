/*
 * Driver.cpp
 *
 *  Created on: Jun 11, 2022
 *      Author: 91991
 */
#include "user.h"
#include "json.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;
using json = nlohmann::json;
string databaseFileName = "database/sampledatabase.json";
unordered_map<string,User*> mapFromNameToUserObject;
const char* temporaryFileName="FXExchangeAPIResponse.json";
const char* exchangeRatesAPIKeyFilePath="src/APIKey.txt";
string CURL = "curl";
string exchangeRatesAPIPath = "http://api.exchangeratesapi.io/v1/latest";
string accessKeyAdder = "?access_key=";

    string getAccessKeyFromFileSystem(){
        ifstream accessKeyFile(exchangeRatesAPIKeyFilePath);
        string accessKey;
        if(accessKeyFile.is_open()){
            getline(accessKeyFile,accessKey);
            accessKeyFile.close();
        }
        return accessKey;
    }

    json getAllRates(string command){
        string redirectCommand = std::string(command) + " > " + temporaryFileName;
        int result = system(redirectCommand.c_str());
        if (result == -1) {
            cerr<<"Error executing command."<<endl;
            json emptyResponse;
            return emptyResponse;
        }
        ifstream file(temporaryFileName);
        string output((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
        json dataFromString = json::parse(output);
        file.close();
        remove(temporaryFileName);
        return dataFromString;
    }
	double getCurrencyEquivalent(double amount, string baseCurrency, string quoteCurrency){
		string curlCommandInput = CURL+" "+exchangeRatesAPIPath+accessKeyAdder;
        string accesKey = getAccessKeyFromFileSystem();
        curlCommandInput+=accesKey;
		cout<<"CURL command is: "<<curlCommandInput<<endl;
        json response = getAllRates(curlCommandInput);
		cout<<"Response: "<<response<<endl;
        json rates = response.at("rates");
        double baseRate = rates.at(baseCurrency);
        cout<<"baseRate is: "<<baseRate<<endl;
        double quoteRate = rates.at(quoteCurrency);
        cout<<"quoteRate is: "<<quoteRate<<endl;
        double finalAmount = ((double) quoteRate/baseRate)*amount;
        cout<<"converted amount is: "<<finalAmount<<endl;
        return finalAmount;
	}
	void fromArrayToUserObject(json jsonObject){
		unordered_map<string,int> tagMap;
		string name=jsonObject.at("name");
		double balance=jsonObject.at("balance");
		double total_owed=jsonObject.at("total_owed");
		double total_owes=jsonObject.at("total_owes");
		json tagArray = jsonObject.at("tags");
		for(const auto& tag:tagArray){
			string tagName=tag.at("tagName");
			int tagAmount=tag.at("tagAmount");
			tagMap[tagName]=tagAmount;
		}
		User* loadedUser = new User(name,balance,total_owed,total_owes,tagMap);
		mapFromNameToUserObject[name]=loadedUser;
	}
	void writeToDatabase(json jsonDataToWrite){
		ofstream file(databaseFileName);
		if(file.is_open()){
			file<<jsonDataToWrite.dump();
			file.close();
		}
	}
	void populateTagArray(json& tagArray, unordered_map<string,int> tagMap){
		for(const auto& tagEntry: tagMap){
			json arrayEntry;
			arrayEntry["tagName"]=tagEntry.first;
			arrayEntry["tagAmount"]=tagEntry.second;
			tagArray.push_back(arrayEntry);
		}
	}
	json connectToDatabaseAndGetJson(){
		cout<<"Opening file\n";
		ifstream file(databaseFileName);
		cout<<"Beginning to parse in json\n";
		json databaseInJson=json::parse(file);
		//file>>databaseInJson;
		file.close();
		return databaseInJson;
	}
	/*
	This method loads all the user data from database (json file), to memory (map)
	This is okay for short fun apps, but not scalable.
	*/
	void loadUserInfoFromDatabase(){
		json databaseInJson=connectToDatabaseAndGetJson();
		int existingNumberOfUsers=databaseInJson.at("numberOfUsers");
		cout<<"There are currently "<<existingNumberOfUsers<<" number of users in database.\n";
		if(existingNumberOfUsers>0){
			json arrayOfUsers=databaseInJson.at("users");
			for(const auto& user:arrayOfUsers){
				fromArrayToUserObject(user);
			}
		}
	}
	/*
	This method updates the database after the session is closed.  
	Memory (map) -> database (json file)
	*/
	void updateDatabase(){
		json databaseToJson;//=connectToDatabaseAndGetJson();
		databaseToJson.clear();
		json usersArray=json::array();
		int numberOfUsers=0;
		
		for(const auto& userPair:mapFromNameToUserObject){
			numberOfUsers++;
			string userName=userPair.first;
			double balance=userPair.second->getBalance();
			double total_owed=userPair.second->getBalance();
			double total_owes=userPair.second->getBalance();
			unordered_map<string,int> tagMap = userPair.second->getTagMap();
			json tagArray = json::array();
			populateTagArray(tagArray,tagMap);
			json userJson;
			userJson["name"]=userName;
			userJson["balance"]=balance;
			userJson["total_owes"]=total_owes;
			userJson["total_owed"]=total_owed;
			userJson["tags"]=tagArray;
			usersArray.push_back(userJson);
		}
		databaseToJson["numberOfUsers"]=numberOfUsers;
		databaseToJson["users"]=usersArray;
		cout<<"There are now "<<numberOfUsers<<" number of users in database.\n";
		writeToDatabase(databaseToJson);
	}
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
		double amount;
		string currency;
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
		cout<<"Enter the currency (e.g. INR/USD/EUR) paid in: ";
		cin>>currency;
		/*
		If currency paid in is not INR, convert it to INR and then add to each user's data
		*/
		if(currency!="INR"){
			cout<<"Converting to INR...\n";
			double inrEquivalent = getCurrencyEquivalent(amount,currency,"INR");
			amount=inrEquivalent;
		}
		cout<<"Enter the tag of the expense: ";
		cin>>tag;
		mapFromNameToUserObject[name]->addOwedExpense(usersToBeSplitWith, ratios, amount, tag);
		int totalUnits = getTotalUnits(ratios);
		double perUnitAmount= amount/totalUnits;
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
	void updateDatabaseAndExit(){
		cout<<"Updating database now...\n";
		updateDatabase();
		cout<<"Exiting the program!\n";
		exit(0);
	}

int main(){

		int choice;
		cout<<"Hello! Welcome to the Bill Splitting Application!\n";
		cout<<"Retrieving user information from database...\n";
		loadUserInfoFromDatabase();
		cout<<"Finished loading information. Starting up the application...\n";
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
			case 8: updateDatabaseAndExit(); break;
			default: cout<<"You seem to have entered an invalid option. Please enter again.\n\n"; break;
			}
		}
		return 1;
	}
