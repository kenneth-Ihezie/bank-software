#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;

namespace Bank {
CONTRACT bankadmin : public contract {
  public:
    using contract::contract;
    //constructor of bankadmin class.
    bankadmin(name receiver, name code, datastream<const char*> ds);
   
    ACTION create(name accName, asset intialDeposit);
    ACTION remove(name accName);
    //internal inline action to send notification
    ACTION notify(name accName, string msg);
    //This below actions will be inlined action.
    ACTION deposit(name accName, asset amount);
    ACTION withdraw(name accName, asset amount);
    ACTION transfer(name from, name to, asset quantity, string description);
    /*
    ACTION activate(name accName, uint64_t bvn);
    ACTION deactivate(name accName, uint64_t bvn);
   */

  private:
    //defining the currency type the bank will accept
    symbol currency;
    name bankName;
    TABLE bankUsers {
      name accountName;
      uint32_t timeOfAccountOpen;
      asset accountBalance;
      auto primary_key() const { return accountName.value; }
    };
    
    /*
    Will implement this function after smart contract 201.
    */
    TABLE bankTable {
      name bankName;
      uint64_t totalCustomers;
      asset netWorth;
      auto primary_key() const { return bankName.value; }
    };
    using bank_users = multi_index<"bank"_n, bankUsers>;
    using bank_admin = multi_index<"admin"_n, bankTable>;
    //private variable for instatiating the table classes
    bank_users bankuser;
    bank_admin banktable;
    /*
     Get current time.
    */
    uint32_t getCurrentTime(){
         return current_time_point().sec_since_epoch();
     }

     /*
     Send notification when a new account is created
     */
     void send_notification(name accName, string msg){
          action(
             permission_level{get_self(), "active"_n},
             get_self(),
             "notify"_n,
             make_tuple(accName, name{accName}.to_string()+" "+msg)
          ).send();
     }

     /*
     Send a notification when asset is deposited, withdrawed or transfered to bankuser contract
     */
     void send_notification_user(name accName, asset amount, asset bankCharge, string msg){
          action(
             permission_level{get_self(), "active"_n},
             accName,
             "assetnotify"_n,
             make_tuple(accName, amount, bankCharge, name{accName}.to_string()+" "+msg+" "+asset{amount}.to_string()+" "+asset{bankCharge}.to_string())
          ).send();
     }

     void send_initialdeposit(name accName, asset amount){
          action(
              permission_level{get_self(), "active"_n},
              accName,
              "initial"_n,
              make_tuple(accName, amount)
          ).send();
     }

     void remove_account(name accName) {
         action(
           permission_level{get_self(), "active"_n },
           accName,
           "remove"_n,
           make_tuple(accName)
         ).send();
     }
     
   };
}

