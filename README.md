# puppyDB
PuppyDB - an in-memory database that does fast GET,SET,BEGIN, NUMEQUALTO operations. 
Implemented in C++.

The database should accept the following commands:

SET name value – Set the variable name to the value value. Neither variable names nor values will contain spaces.
GET name – Print out the value of the variable name, or NULL if that variable is not set.
Both SET and GET accept only integer values for now. That will change in my next update.
UNSET name – Unset the variable name, making it just like that variable was never set.
NUMEQUALTO value – Print out the number of variables that are currently set to value. If no variables equal that value, print 0.
END – Exit the program. Your program will always receive this as its last command.

PuppyDB does support the following transactions commands:

BEGIN – Open a new transaction block. Transaction blocks can be nested; a BEGIN can be issued inside of an existing block.
ROLLBACK – Undo all of the commands issued in the most recent transaction block, and close the block. Print nothing if successful, or print NO TRANSACTION if no transaction is in progress.
COMMIT – Close all open transaction blocks, permanently applying the changes made in them. Print nothing if successful, or print NO TRANSACTION if no transaction is in progress.

The database is lighting fast and perform O(1) - GET,SET,BEGIN and NUMEQUALTO.
Very useful for small in-memory data operations.
