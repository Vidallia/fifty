#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/crypto.h>
using namespace eosio;
using eosio::key256;

class fifty : public eosio::contract {
public:
    const double PAYMENT_MODIFIER = 1.05;

    explicit fifty(account_name self)
    :eosio::contract(self)
    {}

    // @abi action
    void create(const account_name& account) {
        require_auth(account);
        profile_table profile(_self,_self);

        auto iter = profile.find(account);
        /* Assert that this account does not already exist */
        eosio_assert(iter == profile.end(), "Account already exists.");
                                           /*Payer*/
        auto new_user_iter = profile.emplace(_self, [&](auto& prof) {
            prof.account    = account;
            prof.wins       = 0;
            prof.losses     = 0;
            prof.numb_games = 0;
            prof.avg        = 0.0;
            prof.balance    = 100;
        });

        print("Profile created with the account name: ", name{account} );
    }


    // @abi action
    void play(const account_name& account, const uint32_t bet) {
        require_auth(account);
        eosio_assert(bet > 0, "Bet must be greater than zero.");
        profile_table profile(_self,_self);

        auto fifty_acc = profile.find(_self);
        auto iter = profile.find(account);

        eosio_assert(iter != profile.end(), "Account does not exist.");
        eosio_assert(fifty_acc != profile.end(), "Contract account does not exist.");
        eosio_assert(iter->balance >= bet,  "Account has insufficient funds for this bet.");
        eosio_assert(fifty_acc->balance >= (bet * PAYMENT_MODIFIER), "Contract has insufficient funds to play with a bet that size.");

        /* Contract account, calculating a sha256 checksum */
        checksum256 fifty_res;
        sha256( (char *)&fifty_acc->account, sizeof(profile)*2, &fifty_res);
        /* Player account, calculating a sha256 checksum */
        checksum256 acc_res;
        sha256( (char *)&iter->account, sizeof(profile)*2, &acc_res);

        int winner = ((fifty_res.hash[0] + fifty_res.hash[1]) < (acc_res.hash[0] + acc_res.hash[1])) ? 1 : 0;

        if (winner) {
            pay_and_modify(*iter, *fifty_acc, bet);
            print("Congrats! You won ", name{account});
        }
        else {
            pay_and_modify(*fifty_acc, *iter, bet);
            print("You lost :( Try again you just might get that sweet WIN!");
        }
    }

private:
    // @abi table profile i64
    struct profile {
        account_name account;
        uint32_t     wins;
        uint32_t     losses;
        uint32_t     numb_games;
        float        avg;
        uint32_t     balance = 100;

        uint64_t primary_key()const { return account; }
        EOSLIB_SERIALIZE(profile, (account)(wins)(losses)(numb_games)(avg)(balance))
    };

    typedef eosio::multi_index<N(profile), profile> profile_table;


    void pay_and_modify(const profile& winner, const profile& loser, const uint32_t bet) {
        profile_table profile(_self, _self);

        auto winner_accnt = profile.find(winner.account);
        profile.modify(winner_accnt, _self, [&](auto& acc) {
            acc.wins += 1;
            acc.numb_games += 1;
            acc.balance += (acc.account != _self) ? (bet * PAYMENT_MODIFIER) : bet;
            acc.avg = (float)(acc.wins / acc.numb_games);
        });

        auto loser_accnt = profile.find(loser.account);
        profile.modify(loser_accnt, _self, [&](auto& acc) {
            acc.losses += 1;
            acc.numb_games += 1;
            acc.balance -= (acc.account == _self) ? (bet * PAYMENT_MODIFIER) : bet;
            acc.avg += (float)(acc.wins / acc.numb_games);
        });

        if (loser_accnt->balance == 0 && loser_accnt->account != _self) {
            profile.erase(loser_accnt);
            print("You'r account has been deleted in order to save space because your balance has hit zero.");
            print("You are more than welcome to make a new account & continue playing!");
        }

    }

};
EOSIO_ABI(fifty, (create)(play))