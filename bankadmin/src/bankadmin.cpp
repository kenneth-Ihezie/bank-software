#include <bankadmin.hpp>


namespace Bank{

    //The constructor initialize currency which is a token created i created using eosio.token
    bankadmin::bankadmin(name receiver, name code, datastream<const char*> ds):
    contract(receiver, code, ds), currency("RX", 4), bankuser(get_self(), 
    get_self().value), banktable(get_self(), get_self().value){};

     ACTION bankadmin::notify(name accName, string msg){
            require_auth(get_self());
            require_recipient(get_self());
     }
    /*
    Create the account for the user
    */
    ACTION bankadmin::create(name accName, asset initialDeposit){
           //we will get autentication from the account the contract is deployed into.
           require_auth(get_self());
           //check for valid conditions
           check(initialDeposit.symbol != currency, "Invaild currency");
           auto it_user = bankuser.find(accName.value);
           //auto it_admin = banktable.find(bankName.value);
           if(it_user == bankuser.end()){
                bankuser.emplace(get_self(), [&](auto &user){
                user.accountName = accName;
                user.timeOfAccountOpen = getCurrentTime();
                user.accountBalance = initialDeposit;
            });
            send_notification(accName, "Successfully created account");
            send_initialdeposit(accName, initialDeposit);
        } else {
           bankuser.modify(it_user, get_self(), [&](auto &user){
           //if user want to change account name
                user.accountName = accName;
           });
           send_notification(accName, "Successfully modified account");
        }
    }

    /*
    Will be called as an inline action from bankuser contract.
    */
    ACTION bankadmin::deposit(name accName, asset amount){
          //we are manually requesting auth from the bankuser as in our use case.
          require_auth(accName);
          //check for valid conditions
          check(amount.symbol != currency, "Invalid currency");
          //init tables
          //bank_users bankUser(get_self(), get_self().value);
          auto it = bankuser.find(accName.value);
          //check user validity of account
         check(it != bankuser.end(), "Invalid account");
          bankuser.modify(it, get_self(), [&](auto &user){
               user.accountBalance += amount;
        });
        asset bankDepositCharge(0, symbol(currency));
        send_notification_user(accName, amount, bankDepositCharge, "Successfully deposited");
    }

    /*
    Will be called as an inline action from bankuser contract.
    */
    ACTION bankadmin::withdraw(name accName, asset amount){
          //we are manually requesting auth from the banuser as in our use case.
          require_auth(accName);
          //check for valid condition
          check(amount.symbol != currency, "Invalid currency");
          //init table
          //bank_users bankUser(get_self(), get_self().value);
          auto it = bankuser.find(accName.value);
          //check if user account is valid
          check(it != bankuser.end(), "Invalid account");
          //assigning assets manually.
          asset bankCharge(4, symbol(currency));
          bankuser.modify(it, get_self(), [&](auto &user){
                //I used c style cast.
                double charge = (double) amount.amount + bankCharge.amount;
                //check if user has suffient funds
                check(charge < user.accountBalance.amount, "Insuffient fund");
                //else
                double balance = (double) user.accountBalance.amount - charge;
                user.accountBalance.amount = balance;
          });
        send_notification_user(accName, amount, bankCharge, "Withdrawal successful");
    }

    ACTION bankadmin::transfer(name from, name to, asset amount, string description){
          //we are manually requesting auth from the banuser as in our use case.
          require_auth(from);
           //init table
           //bank_users bankuser(get_self(), get_self().value);
           auto it_from = bankuser.find(from.value);
           auto it_to = bankuser.find(to.value);
           //check if "to" bank account exits in the table
           check(it_from != bankuser.end(), "Account does not exist");
           check(it_to != bankuser.end(), "receiver Account does not exist");
           //sub balance from sender account
           bankuser.modify(it_from, get_self(), [&](auto &user){
                    //assigning assets manually.
                asset bankCharge(4, symbol(currency));
                //I used c style cast.
                double charge = (double) amount.amount + bankCharge.amount;
                //check if user has suffient funds
                check(charge < user.accountBalance.amount, "Insuffient fund");
                //else
                double balance = (double) user.accountBalance.amount - charge;
                user.accountBalance.amount = balance;
           });
           bankuser.modify(it_to, get_self(), [&](auto &user){
                   user.accountBalance += amount;
           });
    }

    ACTION bankadmin::remove(name accName){
           require_auth(get_self());
           auto it = bankuser.find(accName.value);
           check(it != bankuser.end(), "Account does not exist");
           bankuser.erase(it);
           send_notification(accName, "Account removed successfully");
           //remove account from bankuser
           remove_account(accName);
    }
}
