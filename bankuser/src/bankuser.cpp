#include <bankuser.hpp>


namespace Account {
   bankuser::bankuser(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds),
   balance(get_self(), get_self().value){}
   /*
   We don't need to provide authentication because we are verifying the 
   user from the bankadmin conract.
   */
   ACTION bankuser::assetnotify(name accName, asset amount, asset bankCharge, string msg){
          require_recipient(get_self());
   }

   //called as inline action from bankadmin.
   ACTION bankuser::initial(name accName, asset initialDeposit){
        print("Am in initial");
        auto it = balance.find(accName.value);
        if(it == balance.end()){
            balance.emplace(get_self(), [&](auto &bal){
               bal.accName = accName;
               bal.balance = initialDeposit;
            });
        }
   }

   //called as inline action from bankadmin
   ACTION bankuser::remove(name accName){
       auto it = balance.find(accName.value);
       if(it != balance.end()){
          balance.erase(it);
       }
   }
   
   ACTION bankuser::deposit(name accName, name bankName, asset amount){
          depositPrivate(accName, bankName, amount);
          auto it = balance.find(accName.value);
              balance.modify(it, get_self(), [&](auto &bal){
              bal.balance += amount;
          });
   }

   ACTION bankuser::withdraw(name accName, name bankName, asset amount){
          withdrawPrivate(accName, bankName, amount);
            auto it = balance.find(accName.value);
              balance.modify(it, get_self(), [&](auto &bal){
              bal.balance -= amount;
          });
   }
    ACTION bankuser::transfer(name bankName, name from, name to, asset quantity, string description){
         transferPrivate(bankName, from, to, quantity, description);
           auto it = balance.find(from.value);
              balance.modify(it, get_self(), [&](auto &bal){
              bal.balance -= quantity;
          });
    }
}