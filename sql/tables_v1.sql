-- Describe ACCOUNTLIST_V1
CREATE TABLE ACCOUNTLIST_V1(
ACCOUNTID integer primary key
, ACCOUNTNAME TEXT COLLATE NOCASE NOT NULL UNIQUE
, ACCOUNTTYPE TEXT NOT NULL 
, ACCOUNTNUM TEXT
, STATUS TEXT NOT NULL
, NOTES TEXT 
, HELDAT TEXT 
, WEBSITE TEXT 
, CONTACTINFO TEXT
, ACCESSINFO TEXT 
, INITIALBAL numeric 
, FAVORITEACCT TEXT NOT NULL
, CURRENCYID integer NOT NULL
);

-- Describe ASSETS_V1
CREATE TABLE ASSETS_V1(
ASSETID integer primary key
, STARTDATE TEXT NOT NULL 
, ASSETNAME TEXT COLLATE NOCASE NOT NULL
, VALUE numeric
, VALUECHANGE TEXT
, NOTES TEXT
, VALUECHANGERATE numeric
, ASSETTYPE TEXT
);


-- Describe BILLSDEPOSITS_V1
CREATE TABLE BILLSDEPOSITS_V1(
BDID integer primary key
, ACCOUNTID integer NOT NULL
, TOACCOUNTID integer
, PAYEEID integer NOT NULL
, TRANSCODE TEXT NOT NULL
, TRANSAMOUNT numeric NOT NULL
, STATUS TEXT
, TRANSACTIONNUMBER TEXT
, NOTES TEXT
, CATEGID integer
, SUBCATEGID integer
, TRANSDATE TEXT
, FOLLOWUPID integer
, TOTRANSAMOUNT numeric
, REPEATS integer
, NEXTOCCURRENCEDATE TEXT
, NUMOCCURRENCES integer 
);
CREATE INDEX idx_account on BILLSDEPOSITS_V1 (ACCOUNTID, TOACCOUNTID);

-- Describe BUDGETSPLITTRANSACTIONS_V1
CREATE TABLE BUDGETSPLITTRANSACTIONS_V1(
SPLITTRANSID integer primary key
, TRANSID integer NOT NULL
, CATEGID integer
, SUBCATEGID integer
, SPLITTRANSAMOUNT numeric
);

-- Describe BUDGETTABLE_V1
CREATE TABLE BUDGETTABLE_V1(
BUDGETENTRYID integer primary key
, BUDGETYEARID integer
, CATEGID integer
, SUBCATEGID integer
, PERIOD TEXT NOT NULL
, AMOUNT numeric NOT NULL
);

-- Describe BUDGETYEAR_V1
CREATE TABLE BUDGETYEAR_V1(
BUDGETYEARID integer primary key
, BUDGETYEARNAME TEXT NOT NULL UNIQUE
);

-- Describe CATEGORY_V1
CREATE TABLE CATEGORY_V1(
CATEGID integer primary key
, CATEGNAME TEXT COLLATE NOCASE NOT NULL UNIQUE
);


-- Describe CHECKINGACCOUNT_V1
CREATE TABLE CHECKINGACCOUNT_V1(
TRANSID integer primary key
, ACCOUNTID integer NOT NULL
, TOACCOUNTID integer
, PAYEEID integer NOT NULL
, TRANSCODE TEXT NOT NULL
, TRANSAMOUNT numeric NOT NULL
, STATUS TEXT
, TRANSACTIONNUMBER TEXT
, NOTES TEXT
, CATEGID integer
, SUBCATEGID integer
, TRANSDATE TEXT
, FOLLOWUPID integer
, TOTRANSAMOUNT numeric
);
CREATE INDEX idx_account on CHECKINGACCOUNT_V1 (ACCOUNTID, TOACCOUNTID);


-- Describe CURRENCYFORMATS_V1
CREATE TABLE CURRENCYFORMATS_V1(
CURRENCYID integer primary key
, CURRENCYNAME TEXT COLLATE NOCASE COLLATE NOCASE NOT NULL UNIQUE
, PFX_SYMBOL TEXT
, SFX_SYMBOL TEXT
, DECIMAL_POINT TEXT
, GROUP_SEPARATOR TEXT
, UNIT_NAME TEXT COLLATE NOCASE
, CENT_NAME TEXT COLLATE NOCASE
, SCALE integer
, BASECONVRATE numeric
, CURRENCY_SYMBOL TEXT
);


-- Describe INFOTABLE_V1
CREATE TABLE INFOTABLE_V1(
INFOID integer not null primary key
, INFONAME TEXT COLLATE NOCASE NOT NULL UNIQUE
, INFOVALUE TEXT NOT NULL
);


-- Describe PAYEE_V1
CREATE TABLE PAYEE_V1(
PAYEEID integer primary key
, PAYEENAME TEXT COLLATE NOCASE NOT NULL UNIQUE
, CATEGID integer
, SUBCATEGID integer
);


-- Describe SPLITTRANSACTIONS_V1
CREATE TABLE SPLITTRANSACTIONS_V1(
SPLITTRANSID integer primary key
, TRANSID integer NOT NULL
, CATEGID integer
, SUBCATEGID integer
, SPLITTRANSAMOUNT numeric
);


-- Describe STOCK_V1
CREATE TABLE STOCK_V1(
STOCKID integer primary key
, HELDAT integer 
, PURCHASEDATE TEXT NOT NULL
, STOCKNAME TEXT COLLATE NOCASE NOT NULL UNIQUE
, SYMBOL TEXT
, NUMSHARES numeric
, PURCHASEPRICE numeric NOT NULL
, NOTES TEXT
, CURRENTPRICE numeric NOT NULL
, VALUE numeric
, COMMISSION numeric
);


-- Describe SUBCATEGORY_V1
CREATE TABLE SUBCATEGORY_V1(
SUBCATEGID integer primary key
, SUBCATEGNAME TEXT COLLATE NOCASE NOT NULL
, CATEGID integer NOT NULL
, UNIQUE(CATEGID, SUBCATEGNAME)
);

-- Describe SETTING_V1
create table SETTING_V1(
SETTINGID integer not null primary key
, SETTINGNAME TEXT COLLATE NOCASE NOT NULL UNIQUE
, SETTINGVALUE TEXT
);
