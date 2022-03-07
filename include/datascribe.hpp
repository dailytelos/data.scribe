#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>
#include <eosio/action.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace std;
using namespace eosio;

CONTRACT datascribe : public contract {
  public:
    using contract::contract;

    //constants
    #include "./constants.hpp"

    //utility
    #include "./utility/utility.hpp"

    //notifications
    #include "./transfer.hpp"

    //core systems
    #include "./core/varscribe.hpp"
    #include "./core/msgscribe.hpp"

  private:

};
