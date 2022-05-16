#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace std;
using namespace eosio;


namespace Account {
  CONTRACT bankuser : public contract {
  public:
    using contract::contract;
    bankuser(name receiver, name code, datastream<const char*> ds);

    //we need to learn more about on_notify attribute
    //[[eosio::on_notify("bankadmin::create")]]
    //will now b called from inline action
    //Error the user account must be created first.
    ACTION initial(name accName, asset initialDeposit);
    
    ACTION assetnotify(name accName, asset amount, asset bankCharge, string msg);
    ACTION remove(name accName);
    ACTION deposit(name accName, name bankName,asset amount);
    ACTION withdraw(name accName, name bankName, asset amount);
    ACTION transfer(name bankName, name from, name to, asset quantity, string description);
    
  private:
    TABLE account {
      name accName;
      asset balance;
      auto primary_key() const { return accName.value; }
    };
    using account_table = multi_index<name("accounts"), account>;
    account_table balance;
    void depositPrivate(name accName, name bankName, asset amount){
            action(
             permission_level{get_self(), "active"_n},
             bankName,
             "deposit"_n,
             make_tuple(accName, amount)
          ).send();
    }

    void withdrawPrivate(name accName, name bankName,asset amount){
           action(
             permission_level{get_self(), "active"_n},
             bankName,
             "withdraw"_n,
             make_tuple(accName, amount)
          ).send();
    }

    void transferPrivate(name bankName, name from, name to, asset quantity, string description){
            action(
             permission_level{get_self(), "active"_n},
             bankName,
             "transfer"_n,
             make_tuple(from, to, quantity, description)
          ).send();
    }
  };
}

