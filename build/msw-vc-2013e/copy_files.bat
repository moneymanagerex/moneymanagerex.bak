xcopy /Y /E /I ..\..\resources\*.wav %1res
xcopy /Y /E /I ..\..\resources\*.css %1res
copy /Y ..\..\3rd\Chart.js\Chart.js %1res
xcopy /Y /E /I ..\..\po\*.mo %1po\en
rem xcopy /Y /E /I ..\..\doc\help\*.* %1help
