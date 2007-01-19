///////////////////////////////////////////////////////////////////////////////
// Name:        wxsqlite3.h
// Purpose:     wxWidgets wrapper around the SQLite3 embedded database library.
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-07-14
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file wxsqlite3.h Interface of the wxSQLite3 class

#ifndef _WX_SQLITE3_H_
#define _WX_SQLITE3_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "wxsqlite3.h"
#endif

#include <wx/datetime.h>

#include "wx/wxsqlite3def.h"

#define WXSQLITE_ERROR 1000

#define WXSQLITE_INTEGER  1
#define WXSQLITE_FLOAT    2
#define WXSQLITE_TEXT     3
#define WXSQLITE_BLOB     4
#define WXSQLITE_NULL     5

enum wxSQLite3TransactionType
{
  WXSQLITE_TRANSACTION_DEFAULT,
  WXSQLITE_TRANSACTION_DEFERRED,
  WXSQLITE_TRANSACTION_IMMEDIATE,
  WXSQLITE_TRANSACTION_EXCLUSIVE
};

/// SQL exception
class WXDLLIMPEXP_SQLITE3 wxSQLite3Exception
{
public:
  /// Constructor
  wxSQLite3Exception(int errorCode, const wxString& errMsg);

  /// Copy constructor
  wxSQLite3Exception(const wxSQLite3Exception&  e);

  /// Destructor
  virtual ~wxSQLite3Exception();

  /// Get error code associated with the exception
  const int GetErrorCode() { return m_errorCode; }

  /// Get error message associated with the exception
  const wxString GetMessage() { return m_errorMessage; }

  /// Convert error code to error message
  static const wxString ErrorCodeAsString(int errorCode);

private:
  int      m_errorCode;     ///< SQLite3 error code associated with this exception
  wxString m_errorMessage;  ///< SQLite3 error message associated with this exception
};

/// SQL statment buffer for use with SQLite3's printf method
class WXDLLIMPEXP_SQLITE3 wxSQLite3StatementBuffer
{
public:
  /// Constructor
  wxSQLite3StatementBuffer();

  /// Destructor
  ~wxSQLite3StatementBuffer();

  /// Format a SQL statement using SQLite3's printf method
  /**
  * This method is a variant of the "sprintf()" from the standard C library.
  * All of the usual printf formatting options apply. In addition,
  * there is a "%q" option. %q works like %s in that it substitutes
  * a null-terminated string from the argument list. But %q also
  * doubles every '\'' character. %q is designed for use inside a
  * string literal. By doubling each '\'' character it escapes that
  * character and allows it to be inserted into the string.
  *
  * For example, so some string variable contains text as follows:
  *
  * char *zText = "It's a happy day!";
  *
  * One can use this text in an SQL statement as follows:
  *
  * wxSQLite3StatementBuffer stmtBuffer;
  * stmtBuffer.Format("INSERT INTO table VALUES('%q')", zText);
  *
  * Because the %q format string is used, the '\'' character in
  * zText is escaped and the SQL generated is as follows:
  *
  * INSERT INTO table1 VALUES('It''s a happy day!')
  *
  * \param format SQL statement string with formatting options
  * \param ... list of statement parameters
  * \return const char pointer to the resulting statement buffer
  */
  const char* Format(const char* format, ...);

  /// Dereference the internal buffer
  /**
  * \return const char pointer to the resulting statement buffer
  */
  operator const char*() const { return m_buffer; }

  /// Clear the internal buffer
  void Clear();

private:
  char* m_buffer;  ///< Internal buffer
};

/// Context for user defined scalar or aggregate functions
/**
* A function context gives user defined scalar or aggregate functions
* access to function arguments and function results. The "Execute" method
* resp. the "Aggregate" and "Finalize" methods receive the current 
* function context as an argument.
*/
class WXDLLIMPEXP_SQLITE3 wxSQLite3FunctionContext
{
public:
  /// Get the number of function arguments
  /**
  * \return the number of arguments the function was called with
  */
  int GetArgCount();

  /// Get the type of a function argument
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \return argument type as one of the values WXSQLITE_INTEGER, WXSQLITE_FLOAT, WXSQLITE_TEXT, WXSQLITE_BLOB, or WXSQLITE_NULL
  */
  int GetArgType(int argIndex);

  /// Check whether a function argument is a NULL value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \return TRUE if the argument is NULL or the argIndex is out of bounds, FALSE otherwise
  */
  bool IsNull(int argIndex);

  /// Get a function argument as an integer value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \param nullValue value to be returned in case the argument is NULL
  * \return argument value
  */
  int GetInt(int argIndex, int nullValue = 0);

  /// Get a function argument as an 64-bit integer value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \param nullValue value to be returned in case the argument is NULL
  * \return argument value
  */
  wxLongLong GetInt64(int argIndex, wxLongLong nullValue = 0);

  /// Get a function argument as a double value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \param nullValue value to be returned in case the argument is NULL
  * \return argument value
  */
  double GetDouble(int argIndex, double nullValue = 0);

  /// Get a function argument as a string value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \param nullValue value to be returned in case the argument is NULL
  * \return argument value
  */
  wxString GetString(int argIndex, const wxString& nullValue = wxEmptyString);

  /// Get a function argument as a BLOB value
  /**
  * \param argIndex index of the function argument. Indices start with 0.
  * \param[out] len length of the blob argument in bytes
  * \return argument value
  */
  const unsigned char* GetBlob(int argIndex, int& len);

  /// Set the function result as an integer value
  /**
  * \param function result value
  */
  void SetResult(int value);

  /// Set the function result as an 64-bit integer value
  /**
  * \param function result value
  */
  void SetResult(wxLongLong value);

  /// Set the function result as a double value
  /**
  * \param function result value
  */
  void SetResult(double value);

  /// Set the function result as a string value
  /**
  * \param function result value
  */
  void SetResult(const wxString& value);

  /// Set the function result as a BLOB value
  /**
  * \param function result value
  * \param length of the result blob in bytes
  */
  void SetResult(unsigned char* value, int len);

  /// Set the function result as a NULL value
  void SetResultNull();

  /// Set the function result as a exact copy of a function argument
  /**
  * \param argIndex index of the argument which should be copied as the result value
  */
  void SetResultArg(int argIndex);

  /// Set an error message as the function result
  /**
  * \param errmsg string containing an error message
  */
  void SetResultError(const wxString& errmsg);

  /// Get the number of aggregate steps
  /**
  * \return the number of aggregation steps. The current aggregation step counts so at least 1 is returned.
  */
  int GetAggregateCount();

  /// Get a pointer to an aggregate structure of specified length
  /**
  * Usually an aggregation functions needs temporary memory to collect
  * the information gathered from each invocation of the "Aggregate" method.
  * On the first invocation of this method the returned memory contains
  * binary zeros.
  * If this memory is used to store pointers to allocated objects,
  * it is important to free all allocated objects in the "Finalize" method.
  * 
  * \param len amount of memory needed in bytes
  * \return pointer to the allocated memory
  */
  void* GetAggregateStruct(int len);

  /// Execute a user defined scalar function (internal use only)
  static void ExecScalarFunction(void* ctx, int argc, void** argv);

  /// Execute an aggregate step of a user defined aggregate function (internal use only)
  static void ExecAggregateStep(void* ctx, int argc, void** argv);

  /// Execute the final step of a user defined aggregate function (internal use only)
  static void ExecAggregateFinalize(void* ctx);

  /// Execute the user defined authorizer function (internal use only)
  static int  ExecAuthorizer(void*, int type,
                             const char* arg1, const char* arg2,
                             const char* arg3, const char* arg4);

private:
  /// Constructor
  wxSQLite3FunctionContext(void* ctx, bool isAggregate, int argc = 0, void** argv = NULL);

  /// Copy constructor
  wxSQLite3FunctionContext(wxSQLite3FunctionContext& ctx);

  void*  m_ctx;          ///< SQLite3 context
  bool   m_isAggregate;  ///< Flag whether this is the context of an aggregate function
  int    m_argc;         ///< Number of arguments
  void** m_argv;         ///< Array of SQLite3 arguments
};


/// Interface for user defined scalar functions
/**
*/
class WXDLLIMPEXP_SQLITE3 wxSQLite3ScalarFunction
{
public:
  /// Virtual destructor
  virtual ~wxSQLite3ScalarFunction() {}
  /// Execute the scalar function
  /**
  * This method is invoked for each appearance of the scalar function in the SQL query.
  * \param ctx function context which can be used to access arguments and result value
  */
  virtual void Execute(wxSQLite3FunctionContext& ctx) = 0;
};


/// Interface for user defined aggregate functions
/**
*/
class WXDLLIMPEXP_SQLITE3 wxSQLite3AggregateFunction
{
public:
  /// Virtual destructor
  virtual ~wxSQLite3AggregateFunction() {}
  /// Execute the aggregate of the function
  /**
  * This method is invoked for each row of the result set of the query using the aggregate function.
  * \param ctx function context which can be used to access arguments and result value
  */
  virtual void Aggregate(wxSQLite3FunctionContext& ctx) = 0;

  /// Prepare the result of the aggregate function
  /**
  * This method is invoked after all rows of the result set of the query
  * using the aggregate function have been processed. Usually the final result
  * is calculated and returned in this method.
  * \param ctx function context which can be used to access arguments and result value
  */
  virtual void Finalize(wxSQLite3FunctionContext& ctx) = 0;
};


/// Interface for a user defined authorizer function
/**
*/
class WXDLLIMPEXP_SQLITE3 wxSQLite3Authorizer
{
public:
  /// Codes identifying the command for which authorization is requested
  enum wxAuthorizationCode
  {                                    // arg1 =          arg2 =
    SQLITE_CREATE_INDEX        =  1,   // Index Name      Table Name
    SQLITE_CREATE_TABLE        =  2,   // Table Name      NULL
    SQLITE_CREATE_TEMP_INDEX   =  3,   // Index Name      Table Name
    SQLITE_CREATE_TEMP_TABLE   =  4,   // Table Name      NULL
    SQLITE_CREATE_TEMP_TRIGGER =  5,   // Trigger Name    Table Name
    SQLITE_CREATE_TEMP_VIEW    =  6,   // View Name       NULL
    SQLITE_CREATE_TRIGGER      =  7,   // Trigger Name    Table Name
    SQLITE_CREATE_VIEW         =  8,   // View Name       NULL
    SQLITE_DELETE              =  9,   // Table Name      NULL
    SQLITE_DROP_INDEX          = 10,   // Index Name      Table Name
    SQLITE_DROP_TABLE          = 11,   // Table Name      NULL
    SQLITE_DROP_TEMP_INDEX     = 12,   // Index Name      Table Name
    SQLITE_DROP_TEMP_TABLE     = 13,   // Table Name      NULL
    SQLITE_DROP_TEMP_TRIGGER   = 14,   // Trigger Name    Table Name
    SQLITE_DROP_TEMP_VIEW      = 15,   // View Name       NULL
    SQLITE_DROP_TRIGGER        = 16,   // Trigger Name    Table Name
    SQLITE_DROP_VIEW           = 17,   // View Name       NULL
    SQLITE_INSERT              = 18,   // Table Name      NULL
    SQLITE_PRAGMA              = 19,   // Pragma Name     1st arg or NULL
    SQLITE_READ                = 20,   // Table Name      Column Name
    SQLITE_SELECT              = 21,   // NULL            NULL
    SQLITE_TRANSACTION         = 22,   // NULL            NULL
    SQLITE_UPDATE              = 23,   // Table Name      Column Name
    SQLITE_ATTACH              = 24,   // Filename        NULL
    SQLITE_DETACH              = 25    // Database Name   NULL
  };
  /// Return codes of the authorizer
  enum wxAuthorizationResult
  {
    SQLITE_OK     = 0,   // Allow access
    SQLITE_DENY   = 1,   // Abort the SQL statement with an error
    SQLITE_IGNORE = 2    // Don't allow access, but don't generate an error
  };
  /// Virtual destructor
  virtual ~wxSQLite3Authorizer() {}
  /// Execute the authorizer function
  /**
  * Please refer to the SQLite documentation for further information about the
  * meaning of the parameters.
  *
  * \param type wxAuthorizationCode. The value signifies what kind of operation is to be authorized.
  * \param arg1 first argument (value depends on "type")
  * \param arg2 second argument (value depends on "type")
  * \param arg3 third argument (name of database if applicable)
  * \param arg4 fourth argument (name of trigger or view if applicable)
  * \return a wxAuthorizationResult, i.e. SQLITE_OK, SQLITE_DENY or SQLITE_IGNORE
  */
  virtual int Authorize(int type, wxString& arg1, wxString& arg2, wxString& arg3, wxString& arg4) = 0;
};


/// Result set of a SQL query
class WXDLLIMPEXP_SQLITE3 wxSQLite3ResultSet
{
public:
  /// Constructor
  wxSQLite3ResultSet();

  /// Copy constructor
  wxSQLite3ResultSet(const wxSQLite3ResultSet& resultSet);

  /// Constructor for internal use
  wxSQLite3ResultSet(void* db, void* stmt,
                     bool eof, bool first = true, bool ownStmt = true);

  /// Assignment constructor
  wxSQLite3ResultSet& operator=(const wxSQLite3ResultSet& resultSet);

  /// Destructor
  /**
  */
  virtual ~wxSQLite3ResultSet();

  /// Get the number of columns in the result set
  /**
  * \return number of columns in result set
  */
  int GetColumnCount();

  /// Find the index of a column by name
  /**
  * \param columnName name of the column
  * \return index of the column. Indices start with 0.
  */
  int FindColumnIndex(const wxString& columnName);

  /// Get the name of a column
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return column name as string
  */
  wxString GetColumnName(int columnIndex);

  /// Get the declared type of a column
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return type string as specified in the table definition
  */
  wxString GetDeclaredColumnType(int columnIndex);

  /// Get the actual type of a column
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return column type as one of the values WXSQLITE_INTEGER, WXSQLITE_FLOAT, WXSQLITE_TEXT, WXSQLITE_BLOB, or WXSQLITE_NULL
  */
  int GetColumnType(int columnIndex);

  /// Get a column as a string using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column as string
  */
  wxString GetAsString(int columnIndex);

  /// Get a column as a string using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxString GetAsString(const wxString& columnName);

  /// Get a column as an integer using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  int GetInt(int columnIndex, int nullValue = 0);

  /// Get a column as an integer using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  int GetInt(const wxString& columnName, int nullValue = 0);

  /// Get a column as a 64-bit integer using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxLongLong GetInt64(int columnIndex, wxLongLong nullValue = 0);

  /// Get a column as a 64-bit integer using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxLongLong GetInt64(const wxString& columnName, wxLongLong nullValue = 0);

  /// Get a column as a double using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  double GetDouble(int columnIndex, double nullValue = 0.0);

  /// Get a column as a double using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  double GetDouble(const wxString& columnName, double nullValue = 0.0);

  /// Get a column as a string using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxString GetString(int columnIndex, const wxString& nullValue = wxEmptyString);

  /// Get a column as a string using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxString GetString(const wxString& columnName, const wxString& nullValue = wxEmptyString);

  /// Get a column as a BLOB using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param[out] len length of the blob in bytes
  * \return value of the column
  */
  const unsigned char* GetBlob(int columnIndex, int& len);

  /// Get a column as a BLOB using the column name
  /**
  * \param columnName name of the column
  * \param[out] len length of the blob in bytes
  * \return value of the column
  */
  const unsigned char* GetBlob(const wxString& columnName, int& len);

  /// Get a column as a date value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetDate(int columnIndex);

  /// Get a column as a date value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetDate(const wxString& columnName);

  /// Get a column as a time value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetTime(int columnIndex);

  /// Get a column as a time value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetTime(const wxString& columnName);

  /// Get a column as a date and time value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetDateTime(int columnIndex);

  /// Get a column as a date and time value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetDateTime(const wxString& columnName);

  /// Get a column as a timestamp value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetTimestamp(int columnIndex);

  /// Get a column as a timestamp value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetTimestamp(const wxString& columnName);

  /// Get a column as a boolean value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  bool GetBool(int columnIndex);

  /// Get a column as a boolean value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  bool GetBool(const wxString& columnName);

  /// Check whether a column has a NULL value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return TRUE if the value is NULL, FALSE otherwise
  */
  bool IsNull(int columnIndex);

  /// Check whether a column has a NULL value using the column name
  /**
  * \param columnName name of the column
  * \return TRUE if the value is NULL, FALSE otherwise
  */
  bool IsNull(const wxString& columnName);

  /// Check whether the result set 
  /**
  * \return TRUE if all rows of the result have been processed, FALSE otherwise
  */
  bool Eof();

  /// Retrieve next row of the result set
  /**
  * Advances the cursor to the next row.
  * On creation of the result set the cursor is positioned BEFORE the first row, i.e.
  * the first call to this method makes the first row available for processing.
  * \return TRUE while there are still rows to process, FALSE otherwise
  */
  bool NextRow();

  /// Finalize the result set
  /**
  */
  void Finalize();

private:
  /// Check the validity of the associated statement
  void CheckStmt();

  void* m_db;       ///< associated database
  void* m_stmt;     ///< associated statement
  bool  m_eof;      ///< Flag for end of result set
  bool  m_first;    ///< Flag for first row of the result set
  int   m_cols;     ///< Numver of columns in row set
  bool  m_ownStmt;  ///< Flag for ownership of the associated statement
};


/// Holds the complete result set of a SQL query
class WXDLLIMPEXP_SQLITE3 wxSQLite3Table
{
public:
  /// Constructor
  wxSQLite3Table();

  wxSQLite3Table(const wxSQLite3Table& table);

  wxSQLite3Table(char** results, int rows, int cols);

  virtual ~wxSQLite3Table();

  wxSQLite3Table& operator=(const wxSQLite3Table& table);

  /// Get the number of columns in the result set
  /**
  * \return the number of columns
  */
  int GetColumnCount();

  /// Get the number of rows in the result set
  /**
  * \return the number of rows
  */
  int GetRowCount();

  /// Find the index of a column by name
  /**
  * \param columnName name of the column
  * \return the index of the column
  */
  int FindColumnIndex(const wxString& columnName);

  /// Get the name of a column
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return name of the column
  */
  wxString GetColumnName(int columnIndex);

  /// Get a column as a string using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column as a string
  */
  wxString GetAsString(int columnIndex);

  /// Get a column as a string using the column name
  /**
  * \param columnName name of the column
  * \return value of the column as a string
  */
  wxString GetAsString(const wxString& columnName);

  /// Get a column as an integer using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  int GetInt(int columnIndex, int nullValue = 0);

  /// Get a column as an integer using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  int GetInt(const wxString& columnName, int nullValue = 0);

  /// Get a column as a 64-bit integer using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxLongLong GetInt64(int columnIndex, wxLongLong nullValue = 0);

  /// Get a column as an integer using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxLongLong GetInt64(const wxString& columnName, wxLongLong nullValue = 0);

  /// Get a column as a double using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  double GetDouble(int columnIndex, double nullValue = 0.0);

  /// Get a column as a double using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  double GetDouble(const wxString& columnName, double nullValue = 0.0);

  /// Get a column as a string using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxString GetString(int columnIndex, const wxString& nullValue = wxEmptyString);

  /// Get a column as a string using the column name
  /**
  * \param columnName name of the column
  * \param nullValue value to be returned in case the column is NULL
  * \return value of the column
  */
  wxString GetString(const wxString& columnName, const wxString& nullValue = wxEmptyString);

  /// Get a column as a date value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetDate(int columnIndex);

  /// Get a column as a date value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetDate(const wxString& columnName);

  /// Get a column as a time value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetTime(int columnIndex);

  /// Get a column as a time value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetTime(const wxString& columnName);

  /// Get a column as a date/time value using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  wxDateTime GetDateTime(int columnIndex);

  /// Get a column as a date/time value using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  wxDateTime GetDateTime(const wxString& columnName);

  /// Get a column as a boolean using the column index
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return value of the column
  */
  bool GetBool(int columnIndex);

  /// Get a column as a boolean using the column name
  /**
  * \param columnName name of the column
  * \return value of the column
  */
  bool GetBool(const wxString& columnName);

  /// Check whether the column selected by index is a NULL value
  /**
  * \param columnIndex index of the column. Indices start with 0.
  * \return TRUE if the value is NULL, FALSE otherwise
  */
  bool IsNull(int columnIndex);

  /// Check whether the column selected by name is a NULL value
  /**
  * \param columnName name of the column
  * \return TRUE if the value is NULL, FALSE otherwise
  */
  bool IsNull(const wxString& columnName);

  /// Set the current row
  /**
  * \param row index of the requested row. Indices start with 0.
  */
  void SetRow(int row);

  /// Finalize the result set
  /**
  */
  void Finalize();

private:
    /// Check for valid results
    void CheckResults();

    int m_cols;        ///< Number of columns
    int m_rows;        ///< Number of rows
    int m_currentRow;  ///< Index of the current row
    char** m_results;  ///< SQLite3 result buffer
};


/// Represents a prepared SQL statement
class WXDLLIMPEXP_SQLITE3 wxSQLite3Statement
{
public:
  /// Constructor
  /**
  */
  wxSQLite3Statement();

  /// Copy constructor
  /**
  */
  wxSQLite3Statement(const wxSQLite3Statement& statement);

  /// Assignement constructor
  /**
  */
  wxSQLite3Statement& operator=(const wxSQLite3Statement& statement);

  /// Constructor (internal use only)
  /**
  */
  wxSQLite3Statement(void* db, void* stmt);

  /// Destructor
  /**
  */
  virtual ~wxSQLite3Statement();

  /// Execute the database update represented by this statement
  /**
  * \return the number of database rows that were changed (or inserted or deleted)
  */
  int ExecuteUpdate();

  /// Execute the query represented by this statement
  /**
  * \return result set instance
  */
  wxSQLite3ResultSet ExecuteQuery();

  /// Get the number of statement parameters
  /**
  * \return the number of parameters in the prepared statement
  */
  int GetParamCount();

  /// Get the index of a parameter with a given name
  /**
  * \param paramName 
  * \return the index of the parameter with the given name. The name must match exactly.
  * If there is no parameter with the given name, return 0.
  */
  int GetParamIndex(const wxString& paramName);

  /// Get the name of a paramater at the given position
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \return the name of the paramIndex-th parameter in the precompiled statement.
  * Parameters of the form ":AAA" or "$VVV" have a name which is the string ":AAA" or "$VVV".
  * Parameters of the form "?" have no name.
  */
  wxString GetParamName(int paramIndex);

  /// Bind parameter to a string value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param stringValue value of the parameter
  */
  void Bind(int paramIndex, const wxString& stringValue);

  /// Bind parameter to a integer value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param intValue value of the parameter
  */
  void Bind(int paramIndex, int intValue);

  /// Bind parameter to a 64-bit integer value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param int64Value value of the parameter
  */
  void Bind(int paramIndex, wxLongLong int64Value);

  /// Bind parameter to a double value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param doubleValue value of the parameter
  */
  void Bind(int paramIndex, double doubleValue);

  /// Bind parameter to a utf-8 character string value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param charValue value of the parameter
  */
  void Bind(int paramIndex, const char* charValue);

  /// Bind parameter to a BLOB value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param blobValue value of the parameter
  * \param nlobLen length of the blob in bytes
  */
  void Bind(int paramIndex, const unsigned char* blobValue, int blobLen);

  /// Bind parameter to a date value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param date value of the parameter
  */
  void BindDate(int paramIndex, const wxDateTime& date);

  /// Bind parameter to a time value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param time value of the parameter
  */
  void BindTime(int paramIndex, const wxDateTime& time);

  /// Bind parameter to a date and time value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param datetime value of the parameter
  */
  void BindDateTime(int paramIndex, const wxDateTime& datetime);

  /// Bind parameter to a timestamp value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param timestamp value of the parameter
  */
  void BindTimestamp(int paramIndex, const wxDateTime& timestamp);

  /// Bind parameter to a boolean value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  * \param value value of the parameter
  */
  void BindBool(int paramIndex, bool value);

  /// Bind parameter to a NULL value
  /**
  * \param paramIndex index of the parameter. The first parameter has an index of 1.
  */
  void BindNull(int paramIndex);

  /// Clear all parameter bindings
  /**
  * Sets all the parameters in the prepared SQL statement back to NULL. 
  */
  void ClearBindings();

  /// Reset the prepared statement
  /**
  * Resets the statement back to it's initial state, ready to be re-executed.
  * Any SQL statement variables that had values bound to them retain their values.
  */
  void Reset();

  /// Finalize the prepared staement
  /**
  */
  void Finalize();

private:
  /// Check for valid database connection
  void CheckDatabase();

  /// Check for valid statement
  void CheckStmt();

  void* m_db;    ///< associated SQLite3 database
  void* m_stmt;  ///< associated SQLite3 statement
};


/// Represents a SQLite3 database object
class WXDLLIMPEXP_SQLITE3 wxSQLite3Database
{
public:
  /// Default constructor
  /**
  * Initializes a SQLite database object.
  * The SQLite database object can only be used in the same thread in which it was created.
  */
  wxSQLite3Database();

  /// Destructor
  /**
  * Destructs a SQLite database object.
  * The database will be closed implicitly if it is still open.
  */
  virtual ~wxSQLite3Database();

  /// Open a SQLite3 database
  /**
  * Opens the sqlite database file "filename". The "filename" is UTF-8 encoded.
  * If the database could not be opened (or created) successfully, then an exception is thrown.
  * If the database file does not exist, then a new database will be created as needed.
  * \param[in] fileName Name of the database file.
  */
  void Open(const wxString& fileName);

  /// Close a SQLite3 database
  /**
  * Take care that all prepared statements have been finalized!
  */
  void Close();

  /// Begin transaction
  /**
  * In SQLite transactions can be deferred, immediate, or exclusive.
  * Deferred means that no locks are acquired on the database until the database is first accessed.
  * Thus with a deferred transaction, the BEGIN statement itself does nothing. Locks are not
  * acquired until the first read or write operation. The first read operation against a database
  * creates a SHARED lock and the first write operation creates a RESERVED lock. Because the
  * acquisition of locks is deferred until they are needed, it is possible that another thread or
  * process could create a separate transaction and write to the database after the BEGIN on the
  * current thread has executed. If the transaction is immediate, then RESERVED locks are acquired
  * on all databases as soon as the BEGIN command is executed, without waiting for the database to
  * be used. After a BEGIN IMMEDIATE, it is guaranteed that no other thread or process will be able
  * to write to the database or do a BEGIN IMMEDIATE or BEGIN EXCLUSIVE. Other processes can continue
  * to read from the database, however. An exclusive transaction causes EXCLUSIVE locks to be acquired
  * on all databases. After a BEGIN EXCLUSIVE, it is guaranteed that no other thread or process will
  * be able to read or write the database until the transaction is complete. 
  *
  * \param[in] transactionType type of transaction (default: DEFERRED).
  */
  void Begin(wxSQLite3TransactionType transactionType = WXSQLITE_TRANSACTION_DEFAULT);

  /// Commit transaction
  /**
  */
  void Commit();

  /// Rollback transaction
  /**
  */
  void Rollback();
  
  /// Get the auto commit state
  /**
  * Test to see whether or not the database connection is in autocommit mode.
  * \return TRUE if it is and FALSE if not.
  * Autocommit mode is on by default. Autocommit is disabled by a BEGIN statement
  * and reenabled by the next COMMIT or ROLLBACK. 
  */
  bool GetAutoCommit();

  /// Check whether a table with the given name exists
  /**
  * \param tableName name of the table
  * \return TRUE if the table exists, FALSE otherwise
  */
  bool TableExists(const wxString& tableName);

  /// Check the syntax of an SQL statement given as a wxString
  /**
  * \param sql query string
  * \return TRUE if the syntax is correct, FALSE otherwise
  */
  bool CheckSyntax(const wxString& sql);

  /// Check the syntax of an SQL statement given as a statement buffer
  /**
  * \param sql query string
  * \return TRUE if the syntax is correct, FALSE otherwise
  */
  bool CheckSyntax(const wxSQLite3StatementBuffer& sql);

  /// Check the syntax of an SQL statement given as a utf-8 character string
  /**
  * \param sql query string
  * \return TRUE if the syntax is correct, FALSE otherwise
  */
  bool CheckSyntax(const char* sql);

  /// Execute a insert, update or delete SQL statement given as a wxString
  /**
  * \param sql query string
  * \return the number of database rows that were changed (or inserted or deleted)
  */
  int ExecuteUpdate(const wxString& sql);

  /// Execute a insert, update or delete SQL statement given as a statement buffer
  /**
  * \param sql query string
  * \return the number of database rows that were changed (or inserted or deleted)
  */
  int ExecuteUpdate(const wxSQLite3StatementBuffer& sql);

  /// Execute a insert, update or delete SQL statement given as a utf-8 character string
  /**
  * \param sql query string
  * \return the number of database rows that were changed (or inserted or deleted)
  */
  int ExecuteUpdate(const char* sql);

  /// Execute a SQL query statement given as a wxString
  /**
  * \param sql query string
  * \return result set instance
  */
  wxSQLite3ResultSet ExecuteQuery(const wxString& sql);

  /// Execute a SQL query statement given as a statement buffer
  /**
  * \param sql query string
  * \return result set instance
  */
  wxSQLite3ResultSet ExecuteQuery(const wxSQLite3StatementBuffer& sql);

  /// Execute a SQL query statement given as a utf-8 character string
  /**
  * \param sql query string
  * \return result set instance
  */
  wxSQLite3ResultSet ExecuteQuery(const char* sql);

  /// Execute a scalar SQL query statement given as a wxString
  /**
  * Allows to easily retrieve the result of queries returning a single integer result
  * like SELECT COUNT(*) FROM table WHERE condition.
  * \param sql query string
  * \return first column of first row as an int
  */
  int ExecuteScalar(const wxString& sql);

  /// Execute a scalar SQL query statement given as a statement buffer
  /**
  * Allows to easily retrieve the result of queries returning a single integer result
  * like SELECT COUNT(*) FROM table WHERE condition.
  * \param sql query string
  * \return first column of first row as an int
  */
  int ExecuteScalar(const wxSQLite3StatementBuffer& sql);

  /// Execute a scalar SQL query statement given as a utf-8 character string
  /**
  * Allows to easily retrieve the result of queries returning a single integer result
  * like SELECT COUNT(*) FROM table WHERE condition.
  * \param sql query string
  * \return first column of first row as an int
  */
  int ExecuteScalar(const char* sql);

  /// Get the result table for a SQL query statement given as a wxString
  /**
  * Returns all resulting rows of the query for later processing.
  * \param sql query string
  * \return table instance
  */
  wxSQLite3Table GetTable(const wxString& sql);

  /// Get the result table for a SQL query statement given as a statement buffer
  /**
  * Returns all resulting rows of the query for later processing.
  * \param sql query string
  * \return table instance
  */
  wxSQLite3Table GetTable(const wxSQLite3StatementBuffer& sql);

  /// Get the result table for a SQL query statement given as a utf-8 character string
  /**
  * Returns all resulting rows of the query for later processing.
  * \param sql query string
  * \return table instance
  */
  wxSQLite3Table GetTable(const char* sql);

  /// Prepare a SQL query statement given as a wxString for parameter binding
  /**
  * \param sql query string
  * \return statement instance
  */
  wxSQLite3Statement PrepareStatement(const wxString& sql);

  /// Prepare a SQL query statement given as a statement buffer for parameter binding
  /**
  * \param sql query string
  * \return statement instance
  */
  wxSQLite3Statement PrepareStatement(const wxSQLite3StatementBuffer& sql);

  /// Prepare a SQL query statement given as a utf-8 character string for parameter binding
  /**
  * \param sql query string
  * \return statement instance
  */
  wxSQLite3Statement PrepareStatement(const char* sql);

  /// Get the row id of last inserted row
  /**
  * Each entry in an SQLite table has a unique integer key.
  * (The key is the value of the INTEGER PRIMARY KEY column if there is such a column,
  * otherwise the key is generated at random. The unique key is always available as the
  * ROWID, OID, or _ROWID_ column.)
  * \return the integer key of the most recent insert in the database.
  */
  wxLongLong GetLastRowId();

  /// Interrupt a long running query
  /**
  * Causes any pending database operation to abort and return at its earliest opportunity.
  * This method is typically called in response to a user action such as pressing "Cancel"
  * or Ctrl-C where the user wants a long query operation to halt immediately.
  */
  void Interrupt();

  /// Set the busy timeout
  /**
  * This method sets a busy handler that sleeps for a while when a table is locked.
  * The handler will sleep multiple times until at least "ms" milliseconds of sleeping
  * have been done.
  * Calling this routine with an argument less than or equal to zero turns off all busy handlers.
  * \param milliSeconds timeout in milliseconds
  */
  void SetBusyTimeout(int milliSeconds);

  /// Create a user-defined scalar function
  /**
  * Registers a SQL scalar function with the database.
  * \param name
  * \param argCount number of arguments the scalar function takes.
  *                 If this argument is -1 then the scalar function may take any number of arguments.
  * \param function instance of an scalar function
  * \return TRUE on successful registration, FALSE otherwise
  */
  bool CreateFunction(const wxString& name, int argCount, wxSQLite3ScalarFunction& function);

  /// Create a user-defined aggregate function
  /**
  * Registers a SQL aggregate function with the database.
  * \param name
  * \param argCount number of arguments the aggregate function takes.
  *                 If this argument is -1 then the aggregate function may take any number of arguments.
  * \param function instance of an aggregate function
  * \return TRUE on successful registration, FALSE otherwise
  */
  bool CreateFunction(const wxString& name, int argCount, wxSQLite3AggregateFunction& function);

  /// Create a user-defined authorizer function
  /**
  * Registers an authorizer object with the SQLite library. The authorizer is invoked
  * (at compile-time, not at run-time) for each attempt to access a column of a table in the database.
  * The authorizer should return SQLITE_OK if access is allowed, SQLITE_DENY if the entire SQL statement
  * should be aborted with an error and SQLITE_IGNORE if the column should be treated as a NULL value.
  * \param authorizer instance of an authorizer function
  * \return TRUE on successful registration, FALSE otherwise
  */
  bool SetAuthorizer(wxSQLite3Authorizer& authorizer);

  /// Get the version of the underlying SQLite3 library
  /**
  * \return a string which contains the version number of the library
  */
  static wxString GetVersion();

private:
  /// Private copy constructor
  wxSQLite3Database(const wxSQLite3Database& db);

  /// Private assignment constructor
  wxSQLite3Database& operator=(const wxSQLite3Database& db);

  /// Prepare a SQL statement (internal use only)
  void* Prepare(const char* sql);

  /// Check for valid database connection
  void CheckDatabase();

  void* m_db;             ///< associated SQLite3 database
  int   m_busyTimeoutMs;  ///< Timeout in milli seconds
};

#endif

