
## fifty: Simple Proof-of-Concept gambling DAPP created for EOSIO

fifty is simple to use, first you must have an account on the EOSIO testnet with the name *fifty*.  To do that you can enter the command:

`cleos create account eosio fifty <Owner_key_here> <Active_key_here>`

Once that's done just set the contract to the account *fifty*.  The first thing you must do after setting the contract to fifty is create
an account for *fifty* within the contract.  Use the following command:

`cleos push action fifty create '["fifty"]' -p fifty`

This will create an account for the contract owner *fifty*.  Since this is a proof-of-concept each account is reated with 100 *points* to represent
tokens, these are not actual tokens, just a representation. It would be possible to implement real tokens within the contract easily.

Once, the *fifty* account is made, a seperate account should be created using cleos. We'll call it **tester**:

`cleos create account eosio tester <Owner_key_here> <Active_key_here>`

We must also create an account for **tester** within the contract:

`cleos push action fifty create '["tester"]' -p tester`

Once both accounts are created you can run `cleos get table fifty fifty profile` Which should output something similar to:

```json
{
  "rows": [{
      "account": "tester",
      "wins": 0,
      "losses": 0,
      "numb_games": 0,
      "balance": 100
    },{
      "account": "fifty",
      "wins": 0,
      "losses": 0,
      "numb_games": 0,
      "balance": 100
    }
  ],
  "more": false
}

```

Now that all of the account creation is out of the way we can get to the fun part! Actually *gambling* with the app.  Using this feature
is as simple as the create action, we will call an action called play via the commandline using cleos:

**Format:**  `cleos push action fifty play '["<account_name>", <bet>]' -p <account_name>`

What this looks like with our test account **tester** is shown below:
`cleos push action fifty play '["tester", 10]' -p tester`

If you run the play action multiple times and then check the *profile* table again with cleos you will see that the stats for each account has changed.

Anyone wanting to contribute example DAPPS or improve upon any I've uploaded, please send a pull request or contact me on telegram at: [@bitfloww](https://t.me/bitfloww)
