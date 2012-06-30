@echo off
cls

rem Generated file names
rem ----------------------------------------------
set mmex_database=mmex.db
set generated_h_file=../src/mmex_db_view.h

rem Definition Scripts 
rem ----------------------------------------------
set python_script=mmex.py
set mmex_database_tables=../sql/tables_v1.sql
rem ----------------------------------------------

@echo =========================================================================
@echo Generating Database Access Routines 
@echo from Python Script: %python_script%
@echo.
@echo =========================================================================

rem if not exist ..\tools\sqlite3.exe goto batch_error
if not exist ..\tools\sqlite3.exe goto generate_mmex_db_view
pause
@echo.
@echo.

if exist %mmex_database% goto generate_mmex_db_view
@echo Created Temporary Database: %mmex_database%
@echo.
@echo Using Database definition file: %mmex_database_tables%
@echo.
..\tools\sqlite3 %mmex_database% ".read %mmex_database_tables%"

:generate_mmex_db_view
@echo Generating file: %generated_h_file%
@echo.
@echo Using command: python %python_script% %mmex_database_tables%
@echo ...output redirected to file: %generated_h_file%
python %python_script% %mmex_database_tables% > %generated_h_file%
@echo.
goto batch_end

:batch_error
@echo Cannot locate the command: sqlite3
@echo.

:batch_end
@echo =========================================================================
@echo Completed.
pause
