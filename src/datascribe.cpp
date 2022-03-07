#include <datascribe.hpp>

#include "./core/varscribe.cpp"
#include "./core/msgscribe.cpp"



EOSIO_DISPATCH(datascribe, (message)(regvar)(delvar)(update)(clearbytime)(clearlast))