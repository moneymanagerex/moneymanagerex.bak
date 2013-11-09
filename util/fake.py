import sys
import random
import sqlite3

conn = sqlite3.connect(sys.argv[1])
curs = conn.cursor()

curs.execute("select * from ACCOUNTLIST_V1")
all_account = [account[0] for account in curs.fetchall()]

curs.execute("select * from PAYEE_V1")
all_payee = [payee[0] for payee in curs.fetchall()]

curs.execute("select c.CATEGID, ifnull(s.SUBCATEGID, -1) from CATEGORY_V1 as c left join SUBCATEGORY_V1 as s ON (c.CATEGID = s.CATEGID)")
all_category = [(c[0], c[1]) for c in curs.fetchall()]

all_type = ("Withdrawal", "Deposit", "Transfer")
all_status = ("V", "R", "N", "F", "D")

for x in range(1, 10000):
    account_id = all_account[random.randint(0, len(all_account) -1)]
    payee_id = all_payee[random.randint(0, len(all_payee) -1)]
    transcode = all_type[random.randint(0, len(all_type) - 2)]
    category = all_category[random.randint(0, len(all_category) -1)]
    amount = 1000 * random.random()
    type = all_status[random.randint(0, len(all_status) -1)]
    
    sql = '''INSERT INTO CHECKINGACCOUNT_V1(ACCOUNTID, TOACCOUNTID, PAYEEID, TRANSCODE, TRANSAMOUNT, STATUS, TRANSACTIONNUMBER, NOTES, CATEGID, SUBCATEGID, TRANSDATE, FOLLOWUPID, TOTRANSAMOUNT)
    VALUES(%d, -1, %d, "%s", %f, "%s", "0", "", %d, %d, "%s", -1, %f)''' % (account_id, payee_id, transcode, amount, type, category[0], category[1], "2013-10-11", amount)
    curs.execute(sql)
    
conn.commit()
