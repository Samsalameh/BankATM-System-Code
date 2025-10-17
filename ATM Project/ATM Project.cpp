//-------------------------------------------Project-ATM-----------
//-----------------------------------------------------------------------------------------
#include<iostream>
#include <stdio.h>
#include<iomanip>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;

enum enWithdrawAmounts {
    e20 = 1, e50 = 2, e100 = 3,
    e200 = 4, e400 = 5, e600 = 6, e800 = 7, e1000 = 8, eExit = 9
};
enum enATMMainMenueOptions {
    eATMQuickWithdraw = 1, eATMNormalWithdraw = 2, eATMDeposit = 3,
    eCheckBalance = 4, eLogOut = 5
};

const string ClientsFileName = "Clients.txt";

struct sClient
{
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance;
    bool MarkForDelete = false;
};

sClient CurrentClient;
void ShowATMMainMenue();
void GoBackToMainMenue();

vector<string> SplitString(string S1, string Delim)
{
    vector<string> vString;
    short pos = 0;
    string sWord; // define a string variable  

    // use find() function to get the position of the delimiters  
    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos); // store the word   
        if (sWord != "")
        {
            vString.push_back(sWord);
        }

        S1.erase(0, pos + Delim.length());  /* erase() until positon and move to next word. */
    }

    if (S1 != "")
    {
        vString.push_back(S1); // it adds last word of the string.
    }

    return vString;

}
sClient ConvertLinetoRecord(string Line, string Seperator = "//")
{
    sClient Client;
    vector<string> vClientData;
    vClientData = SplitString(Line, Seperator);

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];
    Client.AccountBalance = stod(vClientData[4]);//cast string to double
    return Client;
}
string ConvertRecordToLine(sClient Client, string Seperator = "//")
{

    string stClientRecord = "";
    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);
    return stClientRecord;
}

vector <sClient> LoadClientDataFromFile(string FileName)
{
    vector <sClient> vClient;
    fstream MyFile;
    MyFile.open(FileName, ios::in); //read Mode

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord(Line);
            vClient.push_back(Client);
        }
        MyFile.close();
    }
    return vClient;
}

bool FindClientByAccountNumberAndPinCode(string UserName, string PinCode, sClient& Client)
{
    vector <sClient> vClient = LoadClientDataFromFile(ClientsFileName);
    for (sClient U : vClient)
    {
        if (U.AccountNumber == UserName && U.PinCode == PinCode)
        {
            Client = U;
            return true;
        }
    }
    return false;
}

bool LoadClientInfo(string UserName, string PinCode)
{
    if (FindClientByAccountNumberAndPinCode(UserName, PinCode, CurrentClient))
        return true;
    else
        return false;
}

void PrintClientRecordLine(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccountBalance;
}

int ReadClientNormalAmount()
{
    int Amount = 0;
    do
    {
        cout << "Enter an amount multiple of 5's ? " << endl;
        cin >> Amount;
    } while ((Amount % 5) != 0);
    return Amount;
}
int ReadClientAddAmount()
{
    int Amount = 0;
    cout << "Enter a positive Deposit Amount? ";
    cin >> Amount;
    return Amount;
}

void LogIn()
{
    bool LoginFailed = false;
    string AccountNumber, PinCode;
    do
    {
        system("cls");
        cout << "\n-----------------------------------\n";
        cout << "\tLogin Screen";
        cout << "\n-----------------------------------\n";

        if (LoginFailed)
        {
            cout << "Invailed AccountNumber/PinCode!\n";
        }
        cout << "Enter AccountNumber ? ";
        cin >> AccountNumber;
        cout << "Enter PinCode ?";
        cin >> PinCode;
        LoginFailed = !LoadClientInfo(AccountNumber, PinCode);
    } while (LoginFailed);
    ShowATMMainMenue();

}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient> vClients, sClient& Client)
{
    for (sClient C : vClients)
    {

        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }

    }
    return false;
}

sClient ChangeClientBalance(sClient& Client, int Amount, string AccountNumber, enATMMainMenueOptions OperationKind)
{

    switch (OperationKind)
    {
    case enATMMainMenueOptions::eATMDeposit:
    {

        Client.AccountBalance += Amount;
        cout << "Done Successfullym. New Balance is :" << Client.AccountBalance << endl;
        return Client;
        break;
    }
    case enATMMainMenueOptions::eATMQuickWithdraw || enATMMainMenueOptions::eATMNormalWithdraw:
    {
        if (Client.AccountBalance < Amount * -1)
        {
            cout << "\n !! You do not have enough money !!  your balance is " << Client.AccountBalance << endl;
            return Client;
        }
        else
        {
            Client.AccountBalance += Amount;
            return Client;
        }
        break;
    }
    }

    return Client;
}

vector <sClient> SaveCleintsDataToFile(string FileName, vector <sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);//overwrite

    string DataLine;

    if (MyFile.is_open())
    {
        for (sClient& C : vClients)
        {

            if (C.MarkForDelete == false)
            {
                //we only write records that are not marked for delete.  
                DataLine = ConvertRecordToLine(C);
                MyFile << DataLine << endl;
            }

        }

        MyFile.close();
    }

    return vClients;
}

bool UpdateBalanceClientByAccountNumber(string AccountNumber, int QuickAmount, vector <sClient>& vClients, enATMMainMenueOptions OperationKind)
{
    sClient Client;
    char Answer = 'n';
    int Amount = 0;
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        if (OperationKind == enATMMainMenueOptions::eATMDeposit)
        {
            cout << "\n\nAre you sure you want to add money? y/n ? ";
        }
        else
        {
            cout << "\n\nAre you sure you want to withdraw money? y/n ? ";
        }
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sClient& C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {

                    if (OperationKind == enATMMainMenueOptions::eATMDeposit)
                    {
                        C = ChangeClientBalance(Client, QuickAmount, AccountNumber, OperationKind);
                        CurrentClient = C;
                        break;
                    }
                    else if (OperationKind == enATMMainMenueOptions::eATMNormalWithdraw)
                    {
                        cout << "\n\nEnter The amount you want to withdraw ";
                        cin >> Amount;
                        C = ChangeClientBalance(Client, Amount * (-1), AccountNumber, OperationKind);
                        CurrentClient = C;
                        break;
                    }
                    else if (OperationKind == enATMMainMenueOptions::eATMQuickWithdraw)
                    {
                        //cout << "\n\nEnter The amount you want to withdraw ";
                        //cin >> Amount;
                        C = ChangeClientBalance(Client, QuickAmount * (-1), AccountNumber, OperationKind);
                        CurrentClient = C;
                        break;
                    }

                }
            }

            SaveCleintsDataToFile(ClientsFileName, vClients);
            return true;
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

void ShowCheckBalanceScreen()
{

    cout << "\n===================================\n";
    cout << "\tCheck Balance Screen";
    cout << "\n===================================\n";

    cout << "Your Balance is " << CurrentClient.AccountBalance << endl;

}

void GoBackToMainMenue()
{

    cout << "\nPress any Key to return to Main Menue...";
    system("pause>0");
    ShowATMMainMenue();
}

short ReadATMMainMenueOption()
{
    cout << "Choose what do you want to do? [1 to 5]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}
short ReadWithdrawOption()
{
    cout << "Choose what do you want to withdraw? [1 to 9]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void WithdrawOperaion(int Amount)
{
    vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
    UpdateBalanceClientByAccountNumber(CurrentClient.AccountNumber, Amount, vClients, enATMMainMenueOptions::eATMQuickWithdraw);
    cout << "\n\n Done Successfully. New Balance is : " << CurrentClient.AccountBalance;
}
void DepositOperaion(int Amount)
{
    vector <sClient> vClients = LoadClientDataFromFile(ClientsFileName);
    UpdateBalanceClientByAccountNumber(CurrentClient.AccountNumber, Amount, vClients, enATMMainMenueOptions::eATMDeposit);
}

void PerformQuickWithdrawOption(enWithdrawAmounts Amount)
{
    switch (Amount)
    {
    case enWithdrawAmounts::e20:
        WithdrawOperaion(20);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e50:
        WithdrawOperaion(50);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e100:
        WithdrawOperaion(100);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e200:
        WithdrawOperaion(200);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e400:
        WithdrawOperaion(400);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e600:
        WithdrawOperaion(600);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e800:
        WithdrawOperaion(800);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::e1000:
        WithdrawOperaion(1000);
        GoBackToMainMenue();
        break;
    case enWithdrawAmounts::eExit:
        GoBackToMainMenue();
        break;

    }

}

void ShowQuickWithdraw()
{
    system("cls");
    cout << "======================================================" << endl;
    cout << right << setw(16) << "Quick Withdraw Manue" << endl;
    cout << "======================================================" << endl;
    cout << setw(6) << left << "[1] 20" << endl;
    cout << setw(6) << left << "[2] 50" << endl;
    cout << setw(6) << left << "[3] 100" << endl;
    cout << setw(6) << left << "[4] 200" << endl;
    cout << setw(6) << left << "[5] 400" << endl;
    cout << setw(6) << left << "[6] 600" << endl;
    cout << setw(6) << left << "[7] 800" << endl;
    cout << setw(6) << left << "[8] 1000" << endl;
    cout << setw(6) << left << "[9] Logout." << endl;
    cout << "======================================================" << endl;
    cout << "Your Balance is " << CurrentClient.AccountBalance << endl;
    PerformQuickWithdrawOption((enWithdrawAmounts)ReadWithdrawOption());
}
void PerformATMMainMenueOption(enATMMainMenueOptions ATMMainMenueOption)
{
    int Amount = 0;
    switch (ATMMainMenueOption)
    {

    case enATMMainMenueOptions::eATMQuickWithdraw:

        system("cls");
        ShowQuickWithdraw();
        GoBackToMainMenue();
        break;

    case enATMMainMenueOptions::eATMNormalWithdraw:
        system("cls");
        Amount = ReadClientNormalAmount();
        WithdrawOperaion(Amount);
        GoBackToMainMenue();
        break;

    case enATMMainMenueOptions::eATMDeposit:
        system("cls");
        Amount = ReadClientAddAmount();
        DepositOperaion(Amount);
        GoBackToMainMenue();
        break;

    case enATMMainMenueOptions::eCheckBalance:
        system("cls");
        ShowCheckBalanceScreen();
        GoBackToMainMenue();
        break;

    case enATMMainMenueOptions::eLogOut:

        system("cls");
        LogIn();
        break;
    }
}

void ShowATMMainMenue()
{
    system("cls");
    cout << "======================================================" << endl;
    cout << right << setw(16) << "ATM Main Manue Screen" << endl;
    cout << "======================================================" << endl;
    cout << setw(6) << left << "[1] Quick Withdraw." << endl;
    cout << setw(6) << left << "[2] Normal Withdraw." << endl;
    cout << setw(6) << left << "[3] Deposit." << endl;
    cout << setw(6) << left << "[4] Check Balance." << endl;
    cout << setw(6) << left << "[5] Logout." << endl;
    cout << "======================================================" << endl;

    PerformATMMainMenueOption((enATMMainMenueOptions)ReadATMMainMenueOption());
}

int main()
{
    //****Uncomment the following just in the fist run. and then comment them*****
    //    fstream MyFile;     MyFile.open("Clients.txt", ios::out);//Write Mode
    //if (MyFile.is_open())    
    //{ 
    //    MyFile << "Q004//664//Obama//044777//3000\n"; 
    //    MyFile << "A001//4466//LoLo//7799955//4000\n";   
    //    MyFile << "Q009//6600//Vinex//0666644//4000\n";
    //    MyFile << "Q010//211//Spongpop//033054666//9000\n";
    //    MyFile << "G001//46444//Crap//04455888//100000\n";
    //    MyFile.close(); 
    //}
    LogIn();
    system("pause>0");
    return 0;
}